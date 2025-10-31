#include "SpriteRenderer.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "SDKmisc.h"
#include "DirectXTex.h"
#include <DDSTextureLoader.h>

// Convenience macros for safe effect variable retrieval
#define SAFE_GET_PASS(Technique, name, var)   {assert(Technique!=NULL); var = Technique->GetPassByName( name );						assert(var->IsValid());}
#define SAFE_GET_TECHNIQUE(effect, name, var) {assert(effect!=NULL); var = effect->GetTechniqueByName( name );						assert(var->IsValid());}
#define SAFE_GET_SCALAR(effect, name, var)    {assert(effect!=NULL); var = effect->GetVariableByName( name )->AsScalar();			assert(var->IsValid());}
#define SAFE_GET_VECTOR(effect, name, var)    {assert(effect!=NULL); var = effect->GetVariableByName( name )->AsVector();			assert(var->IsValid());}
#define SAFE_GET_MATRIX(effect, name, var)    {assert(effect!=NULL); var = effect->GetVariableByName( name )->AsMatrix();			assert(var->IsValid());}
#define SAFE_GET_SAMPLER(effect, name, var)   {assert(effect!=NULL); var = effect->GetVariableByName( name )->AsSampler();			assert(var->IsValid());}
#define SAFE_GET_RESOURCE(effect, name, var)  {assert(effect!=NULL); var = effect->GetVariableByName( name )->AsShaderResource();	assert(var->IsValid());}

SpriteRenderer::SpriteRenderer(const std::vector<std::wstring>& textureFilenames)
{
	m_textureFilenames = textureFilenames;
}

SpriteRenderer::~SpriteRenderer()
{
	releaseShader();
	destroy();
}

HRESULT SpriteRenderer::reloadShader(ID3D11Device* pDevice)
{
	HRESULT hr;
	WCHAR path[MAX_PATH];

	// Find and load the rendering effect
	V_RETURN(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, L"shader\\SpriteRenderer.fxo"));
	std::ifstream is(path, std::ios_base::binary);
	is.seekg(0, std::ios_base::end);
	std::streampos pos = is.tellg();
	is.seekg(0, std::ios_base::beg);
	std::vector<char> effectBuffer((unsigned int)pos);
	is.read(&effectBuffer[0], pos);
	is.close();
	
	V_RETURN(D3DX11CreateEffectFromMemory((const void*)&effectBuffer[0], effectBuffer.size(), 0, pDevice, &m_pEffect));
	
	assert(m_pEffect->IsValid());

	SAFE_GET_TECHNIQUE(m_pEffect, "Render", m_technique);
	SAFE_GET_PASS(m_technique, "P0", m_pass);

	SAFE_GET_MATRIX(m_pEffect, "g_ViewProjection", m_viewProjectionEV);
	SAFE_GET_VECTOR(m_pEffect, "g_CameraRight", m_cameraRightEV);
	SAFE_GET_VECTOR(m_pEffect, "g_CameraUp", m_cameraUpEV);
	SAFE_GET_RESOURCE(m_pEffect, "g_Sprites", m_spriteTexturesEV);
	
	return hr;
}

void SpriteRenderer::releaseShader()
{
	SAFE_RELEASE(m_pEffect);
}

HRESULT SpriteRenderer::create(ID3D11Device* pDevice)
{
	HRESULT hr;

	// Create the sprite buffer
	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ibd.ByteWidth = static_cast<int>(sizeof(SpriteVertex)) * m_spriteCountMax;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.Usage = D3D11_USAGE_DEFAULT;

	V(pDevice->CreateBuffer(&ibd, nullptr, &m_pVertexBuffer));

	// Define the input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] = // http://msdn.microsoft.com/en-us/library/bb205117%28v=vs.85%29.aspx
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "RADIUS",    0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEX_INDEX", 0, DXGI_FORMAT_R32_SINT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TIME",      0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ALPHA",     0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DISTANCE",  0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Create the input layout
	D3DX11_PASS_DESC pd;
	V_RETURN(m_pass->GetDesc(&pd));
	V_RETURN(pDevice->CreateInputLayout(layout, numElements, pd.pIAInputSignature,
		pd.IAInputSignatureSize, &m_pInputLayout));

	// Load textures
	for (const auto& path : m_textureFilenames)
	{
		m_spriteSRV.push_back(nullptr);
		V(DirectX::CreateDDSTextureFromFile(pDevice, path.c_str(), nullptr, &m_spriteSRV.back()));

		if (hr != S_OK)
			std::wcerr << "ERROR: File \"" << path << "\" could not be loaded." << std::endl;
	}

	return S_OK;
}

void SpriteRenderer::destroy()
{
	for (auto& tex : m_spriteSRV)
		SAFE_RELEASE(tex);
	m_spriteSRV.clear();

	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pInputLayout);
}

void SpriteRenderer::renderSprites(
	ID3D11DeviceContext* context, 
	const std::vector<SpriteVertex>& sprites,
	const CFirstPersonCamera& camera, 
	int count, 
	int offset)
{
	assert(count >= 0);
	assert(offset >= 0);

	int count_max = static_cast<int>(std::min(sprites.size(), m_spriteCountMax));

	// Upload the vertex buffer to the GPU
	D3D11_BOX box; 
	box.left = 0; 
	box.right = count_max * static_cast<int>(sizeof(SpriteVertex));
	box.top = 0; 
	box.bottom = 1; 
	box.front = 0; 
	box.back = 1;
	context->UpdateSubresource(m_pVertexBuffer, 0, &box, sprites.data(), 0, 0);

	// Bind the terrain vertex buffer to the input assembler stage 
	ID3D11Buffer* vbs[] = { m_pVertexBuffer, };
	unsigned int strides[] = { sizeof(SpriteVertex), }, offsets[] = { 0, };
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);

	// Tell the input assembler stage which primitive topology to use
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetInputLayout(m_pInputLayout);

	// Set the shader effect variables
	m_viewProjectionEV->SetMatrix((float*)&(camera.GetViewMatrix() * camera.GetProjMatrix()));
	m_cameraRightEV->SetFloatVector((float*)&camera.GetWorldRight());
	m_cameraUpEV->SetFloatVector((float*)&camera.GetWorldUp());
	m_spriteTexturesEV->SetResourceArray(m_spriteSRV.data(), 0, static_cast<int>(m_spriteSRV.size()));

	m_pass->Apply(0, context);


	context->Draw(std::min(offset + count, count_max), std::min(offset, count_max));
}

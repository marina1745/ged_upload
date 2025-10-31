#pragma once

#include <DirectXMath.h>

#include "GameEffect.h"
#include "Mesh.h"

class Projectile;

// Virtual/abstract class for all GameObjects
class GameObject
{
public:
	std::string name;
	
	// = 0 denotes a pure virtual (i.e. abstract) function
	virtual DirectX::XMMATRIX getWorldMatrix() const = 0;
};

// Class for GameObjects which only act as parents for other GameObjects (Wrapper class)
class ParentObject : public GameObject
{
public:
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	DirectX::XMMATRIX getWorldMatrix() const override
	{
		return worldMatrix;
	}
};

// Class for all Gameobjects which should be rendered
class MeshObject : public GameObject
{
public:
	// The GameObject's parent, used for transform
	std::weak_ptr<GameObject> parent;

	DirectX::XMVECTOR position = {0, 0, 0};
	DirectX::XMVECTOR rotation = { 0, 0, 0 };
	DirectX::XMVECTOR scale = { 1, 1, 1 };

	std::shared_ptr<Mesh> mesh = nullptr;

	// Renders the GameObject
	HRESULT render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& camera, const DirectX::XMMATRIX& light) const
	{
		if (!mesh)
			return S_FALSE;

		HRESULT hr;

		DirectX::XMMATRIX world = getWorldMatrix();
		DirectX::XMMATRIX worldViewProj = world * camera;
		DirectX::XMMATRIX lightWorldViewProj = world * light;
		V(g_gameEffect.worldEV->SetMatrix((float*)&world));
		V(g_gameEffect.worldNormalsEV->SetMatrix((float*)&XMMatrixTranspose(XMMatrixInverse(nullptr, world))));
		V(g_gameEffect.worldViewProjectionEV->SetMatrix((float*)&worldViewProj));
		V(g_gameEffect.lightWorldViewProjEV->SetMatrix((float*)&lightWorldViewProj));
		
		mesh->render(context, g_gameEffect.meshPass, g_gameEffect.diffuseEV, g_gameEffect.specularEV, g_gameEffect.glowEV);

		return hr;
	}

	HRESULT renderDepthOnly(ID3D11DeviceContext* context, const DirectX::XMMATRIX& viewProj) const
	{
		if (!mesh)
			return S_FALSE;

		HRESULT hr;

		DirectX::XMMATRIX worldViewProj = getWorldMatrix() * viewProj;
		V(g_gameEffect.worldViewProjectionEV->SetMatrix((float*)&worldViewProj));

		mesh->render(context, g_gameEffect.meshShadowPass, g_gameEffect.diffuseEV, g_gameEffect.specularEV, g_gameEffect.glowEV);

		return hr;
	}

	// Computes the GameObject's transformation matrix
	DirectX::XMMATRIX getParentMatrix() const
	{
		return DirectX::XMMatrixScalingFromVector(scale)
			* DirectX::XMMatrixRotationRollPitchYawFromVector(rotation)
			* DirectX::XMMatrixTranslationFromVector(position);
	}

	// Computes the GameObject's world matrix
	virtual DirectX::XMMATRIX getWorldMatrix() const override
	{
		if (auto parent_ptr = parent.lock()) // weak_ptr can be casted to bool
			return getParentMatrix() * parent_ptr->getWorldMatrix();
		else
			return getParentMatrix();
	}
};

// Class for enemy ships
class EnemyObject : public MeshObject
{
public:
	std::shared_ptr<EnemyObject> type = nullptr;

	DirectX::XMVECTOR velocity = { 0, 0, 0 };
	int health = 1;
	float size = 1;

	virtual DirectX::XMMATRIX getWorldMatrix() const override
	{
		if (type)
			return type->getWorldMatrix() * MeshObject::getWorldMatrix();
		else
			return MeshObject::getWorldMatrix();
	}

	void update(float fElapsedTime)
	{
		using namespace DirectX;
		position += velocity * fElapsedTime;
	}
};

// Class for weapons
class WeaponObject : public MeshObject
{
public:
	DirectX::XMVECTOR spawnpoint = { 0, 0, 0 };
	float cooldown = 1;
	bool active = false;

	std::shared_ptr<Projectile> projectile;

	bool update(float fElpasedTime) 
	{
		secondsSinceLastShot += fElpasedTime;
		
		if (active && secondsSinceLastShot >= cooldown)
		{
			secondsSinceLastShot = 0.0f;
			return true;
		}

		return false;
	}

private:
	float secondsSinceLastShot = 0.0f;
};
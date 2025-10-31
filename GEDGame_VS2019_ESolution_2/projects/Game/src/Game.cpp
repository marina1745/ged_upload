#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <cmath>


#include "dxut.h"
#include "DXUTmisc.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"

#include "d3dx11effect.h"

#include "Terrain.h"
#include "Mesh.h"
#include "GameEffect.h"
#include "SpriteRenderer.h"
#include "ConfigParser.h"
#include "GameObject.h"
#include "Particle.h"

#include "debug.h"


// Help macros
#define DEG2RAD( a ) ( (a) * XM_PI / 180.f )

using namespace std;
using namespace DirectX;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

// Camera
struct CAMERAPARAMS {
	float   fovy;
	float   aspect;
	float   nearPlane;
	float   farPlane;
}                                       g_cameraParams;
float                                   g_cameraMoveScaler = 1000.f;
float                                   g_cameraRotateScaler = 0.005f;
CFirstPersonCamera                      g_camera;               // A first person camera

// User Interface
CDXUTDialogResourceManager              g_dialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg                         g_settingsDlg;          // Device settings dialog
CDXUTTextHelper*                        g_txtHelper = nullptr;
CDXUTDialog                             g_hud;                  // dialog for standard controls
CDXUTDialog                             g_sampleUI;             // dialog for sample specific controls

// Scene information
XMVECTOR                                g_lightDir = XMVector3Normalize({1,0.3,0.2});
XMVECTOR                                g_ClearColor = { 0.025525f * 5.0f, 0.045511f * 5.0f, 0.088005f * 5.0f, 1.0f };
XMVECTOR                                g_gravity = {0, -9.81, 0};

// Rendering
GameEffect								g_gameEffect; // CPU part of Shader
std::unique_ptr<SpriteRenderer>         g_spriteRenderer = nullptr;
ID3D11RenderTargetView*                 g_DefaultRenderTarget = nullptr;
ID3D11DepthStencilView*                 g_DefaultDepthStencil = nullptr;
ID3D11Texture2D*                        g_ShadowMap = nullptr;
ID3D11ShaderResourceView*               g_ShadowMapSRV = nullptr;
ID3D11DepthStencilView*                 g_ShadowStencil = nullptr;
D3D11_VIEWPORT                          g_DefaultViewports[1];
D3D11_VIEWPORT                          g_ShadowViewport[1];
void*                                   g_nullptrs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

// Config
ConfigParser                            g_ConfigParser;

// Scene data
bool                                    g_cameraMovement = false;
bool                                    g_debugShadows = false;

std::map<std::string, std::shared_ptr<Mesh>>    g_meshes;

std::vector<std::shared_ptr<EnemyObject>>       g_enemyPrototypes;
std::map<std::string, std::shared_ptr<Projectile>>        g_projectilePrototypes;
std::unique_ptr<Explosion>                      g_ExplosionPrototype = nullptr;

Terrain     									g_terrain;
std::shared_ptr<ParentObject>                   g_cameraObject = nullptr;
std::shared_ptr<ParentObject>                   g_terrainObject = nullptr;
std::vector<std::shared_ptr<MeshObject>>        g_gameObjects;
std::vector<std::shared_ptr<WeaponObject>>      g_weaponObjects;
std::list<EnemyObject>                          g_enemyObjects;
std::list<Projectile>                           g_Projectiles;
std::list<Explosion>                            g_Explosions;

std::vector<SpriteVertex>                       g_sprites;

float                                   g_timeSinceLastEnemy = 5.0f;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_TOGGLESPIN          4
#define IDC_TOGGLEMOVE          5
#define IDC_TOGGLESHADOWDEBUG   6
#define IDC_RELOAD_SHADERS		101

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *, UINT , const CD3D11EnumDeviceInfo *,
                                       DXGI_FORMAT, bool, void* );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext );

void clearFrameBuffers(ID3D11DeviceContext* pd3dImmediateContext, XMVECTOR clearColor);

void renderShadowMap(ID3D11DeviceContext* pd3dImmediateContext, ID3D11DepthStencilView* shadowStencil, D3D11_VIEWPORT* shadowViewPort, const XMMATRIX& viewProj);

void renderObjects(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX& viewProj, const XMMATRIX& lightViewProj);

void renderSprites(ID3D11DeviceContext* pd3dImmediateContext);

void InitApp();
void DeinitApp();
void RenderText();

void ReleaseShader();
HRESULT ReloadShader(ID3D11Device* pd3dDevice);

void CreateGameObjects();
void CreateEnemyPrototypes();
void CreateWeaponObjects();
void CreateProjectilePrototypes(std::vector<std::wstring>& sprite_filenames);
void CreateExplosionPrototype(std::vector<std::wstring>& sprite_filenames);

void SpawnEnemy();
void drawShadowMap(ID3D11DeviceContext* pd3dImmediateContext);

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Old Direct3D Documentation:
    // Start > All Programs > Microsoft DirectX SDK (June 2010) > Windows DirectX Graphics Documentation

    // DXUT Documentaion:
    // Start > All Programs > Microsoft DirectX SDK (June 2010) > DirectX Documentation for C++ : The DirectX Software Development Kit > Programming Guide > DXUT
	
    // New Direct3D Documentaion (just for reference, use old documentation to find explanations):
    // http://msdn.microsoft.com/en-us/library/windows/desktop/hh309466%28v=vs.85%29.aspx


    // Initialize COM library for windows imaging components
    /*HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (hr != S_OK)
    {
        MessageBox(0, L"Error calling CoInitializeEx", L"Error", MB_OK | MB_ICONERROR);
        exit(-1);
    }*/


    // Set DXUT callbacks
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    //DXUTSetIsInGammaCorrectMode(false);

    InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"GED Game" ); // You may change the title

    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 1280, 720 );

    DXUTMainLoop(); // Enter into the DXUT render loop

    DXUTShutdown();
    DeinitApp();

    return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    HRESULT hr;
    WCHAR path[MAX_PATH];

    // Parse the config file
    V(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, L"game.cfg"));
	char pathA[MAX_PATH];
	size_t size;
	wcstombs_s(&size, pathA, path, MAX_PATH);

    if (!g_ConfigParser.load(pathA))
        MessageBoxA(NULL, "Could not load configfile \"game.cfg\" ", "File not found", MB_ICONERROR | MB_OK);

    // Intialize the user interface
    g_settingsDlg.Init( &g_dialogResourceManager );
    g_hud.Init( &g_dialogResourceManager );
    g_sampleUI.Init( &g_dialogResourceManager );
    g_hud.SetCallback( OnGUIEvent );
    int iY = 30;
    int iYo = 26;
    g_hud.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22 );
    g_hud.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3 );
    g_hud.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2 );
	g_hud.AddButton (IDC_RELOAD_SHADERS, L"Reload shaders (F5)", 0, iY += 24, 170, 22, VK_F5);
    g_sampleUI.SetCallback( OnGUIEvent ); 
    iY = 10;
    iY += 16;
    g_sampleUI.AddCheckBox(IDC_TOGGLEMOVE, L"Toggle Movement", -40, iY += 24, 150, 22, g_cameraMovement);
    g_sampleUI.AddCheckBox(IDC_TOGGLESHADOWDEBUG, L"Toggle Shadow Debugging", -40, iY += 24, 150, 22, g_debugShadows);

    // Create meshes
    for (auto &m : g_ConfigParser.get_Meshes())
        g_meshes.emplace(m.identifier, std::make_shared<Mesh>(m.pathMesh, m.pathDiffuse, m.pathSpecular, m.pathGlow));

    std::vector<std::wstring> sprite_filenames;

    CreateGameObjects();
    CreateEnemyPrototypes();
    CreateProjectilePrototypes(sprite_filenames);
    CreateWeaponObjects();
    CreateExplosionPrototype(sprite_filenames);

    // Create the sprite renderer object
    g_spriteRenderer = std::make_unique<SpriteRenderer>(sprite_filenames);
}

void CreateGameObjects()
{
    // Create parent game objects
    g_cameraObject = std::make_shared<ParentObject>();
    g_cameraObject->name = "Camera";
    g_terrainObject = std::make_shared<ParentObject>();
    g_terrainObject->name = "Terrain";
    
    // Create mesh game objects
    for (auto& o : g_ConfigParser.get_Objects())
    {
        auto new_gameObject = make_shared<MeshObject>();

        new_gameObject->name = o.identifier;

        new_gameObject->position = { o.pos_x, o.pos_y, o.pos_z };
        new_gameObject->rotation = { DEG2RAD(o.rot_x), DEG2RAD(o.rot_y), DEG2RAD(o.rot_z) };
        new_gameObject->scale = { o.scale, o.scale, o.scale };

        auto mesh_it = g_meshes.find(o.meshIdentifier);
        if (mesh_it != g_meshes.end())
            new_gameObject->mesh = mesh_it->second;
        else
            std::cerr << "ERROR: Mesh with identifier " << o.meshIdentifier << " could not be found\n";

        if (o.parentIdentifier == "camera")
            new_gameObject->parent = g_cameraObject;
        else if (o.parentIdentifier == "terrain")
            new_gameObject->parent = g_terrainObject;
        else
            for (auto& n : g_gameObjects)
                if (n->name == o.parentIdentifier)
                    new_gameObject->parent = n;

        g_gameObjects.push_back(new_gameObject);
    }
}

void CreateEnemyPrototypes()
{
    // https://en.wikipedia.org/wiki/Prototype_pattern
    for (auto& e : g_ConfigParser.get_Enemies())
    {
        auto new_enemy = std::make_shared<EnemyObject>();

        new_enemy->position = { e.pos_x, e.pos_y, e.pos_z };
        new_enemy->rotation = { DEG2RAD(e.rot_x), DEG2RAD(e.rot_y), DEG2RAD(e.rot_z) };
        new_enemy->scale = { e.scale, e.scale, e.scale };

        new_enemy->health = e.hp;
        new_enemy->velocity = { e.speed, e.speed, e.speed };
        new_enemy->size = e.size * e.scale;

        auto mesh_it = g_meshes.find(e.meshIdentifier);
        if (mesh_it != g_meshes.end())
            new_enemy->mesh = mesh_it->second;
        else
            std::cerr << "ERROR: Mesh with identifier " << e.meshIdentifier << " could not be found\n";

        g_enemyPrototypes.push_back(new_enemy);
    }
}

void CreateWeaponObjects()
{
    for (auto& w : g_ConfigParser.get_Weapons())
    {
        auto new_weaponObject = make_shared<WeaponObject>();

        auto mesh_it = g_meshes.find(w.meshIdentifer);
        if (mesh_it != g_meshes.end())
            new_weaponObject->mesh = mesh_it->second;
        else
            std::cerr << "ERROR: Mesh with identifier " << w.meshIdentifer << " could not be found\n";

        for (auto& o : g_gameObjects)
            if (o->name == w.parentIdentifier)
                new_weaponObject->parent = o;

        auto proj_it = g_projectilePrototypes.find(w.projectile_identifier);
        if (proj_it != g_projectilePrototypes.end())
            new_weaponObject->projectile = proj_it->second;
        else
            std::cerr << "ERROR: Projectile with identifier " << w.projectile_identifier << " could not be found\n";

        new_weaponObject->cooldown = 1.0f / w.firerate;
        new_weaponObject->spawnpoint = { w.spawnpoint_x, w.spawnpoint_y, w.spawnpoint_z };

        g_weaponObjects.push_back(new_weaponObject);
    }
}

void CreateProjectilePrototypes(std::vector<std::wstring>& sprite_filenames)
{
    for (auto& p : g_ConfigParser.get_Projectiles())
    {
        auto new_proj = std::make_shared<Projectile>();

        new_proj->velocity = { p.projectileSpeed, p.projectileSpeed, p.projectileSpeed };

        new_proj->damage = p.damage;
        new_proj->useGravity = p.gravity;

        new_proj->size = p.spriteSize;
        new_proj->spriteIndex = static_cast<int>(sprite_filenames.size());

        sprite_filenames.push_back(std::wstring(p.spritePath.begin(), p.spritePath.end()));
        g_projectilePrototypes.emplace(p.identifier, new_proj);
    }
}

void CreateExplosionPrototype(std::vector<std::wstring>& sprite_filenames)
{
    g_ExplosionPrototype = std::make_unique<Explosion>(sprite_filenames.size(), g_ConfigParser.get_Explosion().particle_count);
    
    g_ExplosionPrototype->size = g_ConfigParser.get_Explosion().scale;
    g_ExplosionPrototype->duration = g_ConfigParser.get_Explosion().duration;
    
    sprite_filenames.push_back(
        wstring(
            g_ConfigParser.get_Explosion().spritePath.begin(),
            g_ConfigParser.get_Explosion().spritePath.end()));
}

//--------------------------------------------------------------------------------------
// Deinitialize the app 
//--------------------------------------------------------------------------------------
void DeinitApp()
{
    g_gameObjects.clear();
    g_enemyObjects.clear();
    g_enemyPrototypes.clear();
    g_meshes.clear();
    g_spriteRenderer = nullptr;
    g_cameraObject = nullptr;
    g_terrainObject = nullptr;
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_txtHelper->Begin();
    g_txtHelper->SetInsertionPos( 5, 5 );
    g_txtHelper->SetForegroundColor(XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f));
    g_txtHelper->DrawTextLine( DXUTGetFrameStats(true)); //DXUTIsVsyncEnabled() ) );
    g_txtHelper->DrawTextLine( DXUTGetDeviceStats() );
    g_txtHelper->End();
}

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *, UINT, const CD3D11EnumDeviceInfo *,
        DXGI_FORMAT, bool, void* )
{
    return true;
}

//--------------------------------------------------------------------------------------
// Specify the initial device settings
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	UNREFERENCED_PARAMETER(pDeviceSettings);
	UNREFERENCED_PARAMETER(pUserContext);

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if (pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE)
        {
            DXUTDisplaySwitchingToREFWarning();
        }
    }
    // Enable anti aliasing
    pDeviceSettings->d3d11.sd.SampleDesc.Count = 4;
    pDeviceSettings->d3d11.sd.SampleDesc.Quality = 1;

#ifndef NDEBUG
    pDeviceSettings->d3d11.CreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
    std::cout << "Created D3D11 Device in debug mode" << std::endl;
#endif


    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice,
        const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	UNREFERENCED_PARAMETER(pBackBufferSurfaceDesc);
	UNREFERENCED_PARAMETER(pUserContext);

    HRESULT hr;

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext(); // http://msdn.microsoft.com/en-us/library/ff476891%28v=vs.85%29
    V_RETURN( g_dialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
    V_RETURN( g_settingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
    g_txtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_dialogResourceManager, 15 );

    V_RETURN( ReloadShader(pd3dDevice) );

    // Create the shadow map
    D3D11_TEXTURE2D_DESC shadow_desc;
    shadow_desc.Width = g_ConfigParser.get_Shadows().resolution;
    shadow_desc.Height = g_ConfigParser.get_Shadows().resolution;
    shadow_desc.MipLevels = 1;
    shadow_desc.ArraySize = 1;
    shadow_desc.SampleDesc.Count = 1;
    shadow_desc.SampleDesc.Quality = 0;
    shadow_desc.Usage = D3D11_USAGE_DEFAULT;
    shadow_desc.Format = DXGI_FORMAT_R32_TYPELESS;
    shadow_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    shadow_desc.CPUAccessFlags = 0;
    shadow_desc.MiscFlags = 0;
    V_RETURN(pd3dDevice->CreateTexture2D(&shadow_desc, nullptr, &g_ShadowMap));

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_desc;
    depth_stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
    depth_stencil_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_desc.Flags = 0;
    depth_stencil_desc.Texture2D.MipSlice = 0;
    V_RETURN(pd3dDevice->CreateDepthStencilView(g_ShadowMap, &depth_stencil_desc, &g_ShadowStencil));

    D3D11_SHADER_RESOURCE_VIEW_DESC shader_srv_desc;
    shader_srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
    shader_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shader_srv_desc.Texture2D.MipLevels = -1;
    shader_srv_desc.Texture2D.MostDetailedMip = 0;
    V_RETURN(pd3dDevice->CreateShaderResourceView(g_ShadowMap, &shader_srv_desc, &g_ShadowMapSRV));

    g_ShadowViewport[0].TopLeftX = 0;
    g_ShadowViewport[0].Width = g_ConfigParser.get_Shadows().resolution;
    g_ShadowViewport[0].Height = g_ConfigParser.get_Shadows().resolution;
    g_ShadowViewport[0].MinDepth = 0;
    g_ShadowViewport[0].MaxDepth = 1;

    // Create the terrain
	V_RETURN(g_terrain.create(pd3dDevice));
    
    // Update height values
    for (auto& g : g_gameObjects)
        if (g->parent.lock() == g_terrainObject) // Ugly: Locking a weak pointer just for a comparison
            g->position += {0, g_terrain.get_height_at(XMVectorGetX(g->position), XMVectorGetZ(g->position)), 0};
    
    // Create all meshes
    for (auto& m : g_meshes)
        V_RETURN(m.second->create(pd3dDevice));
    V_RETURN(Mesh::createInputLayout(pd3dDevice, g_gameEffect.meshPass));

    // Create the sprite renderer
    V_RETURN(g_spriteRenderer->create(pd3dDevice));
    
    // Initialize the camera
    float center_height = g_terrain.get_height_at(0.0f, 0.0f) + 20.0f;
	XMVECTOR vEye = XMVectorSet(0.0f, center_height, 0.0f, 0.0f);   // Camera eye is here
    XMVECTOR vAt = XMVectorSet(1.0f, center_height, 0.0f, 1.0f);               // ... facing at this position
    g_camera.SetViewParams(vEye, vAt); // http://msdn.microsoft.com/en-us/library/windows/desktop/bb206342%28v=vs.85%29.aspx
	g_camera.SetScalers(g_cameraRotateScaler, g_cameraMoveScaler);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	UNREFERENCED_PARAMETER(pUserContext);

    g_dialogResourceManager.OnD3D11DestroyDevice();
    g_settingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    
    SAFE_RELEASE(g_ShadowMap);
    SAFE_RELEASE(g_ShadowStencil);
    SAFE_RELEASE(g_ShadowMapSRV);

	// Destroy the terrain
	g_terrain.destroy();
    
    // Destroy meshes
    Mesh::destroyInputLayout();
    for (auto& m : g_meshes)
        m.second->destroy();

    // Destroy the sprite renderer
    g_spriteRenderer->destroy();

    SAFE_DELETE( g_txtHelper );
    ReleaseShader();
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
        const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	UNREFERENCED_PARAMETER(pSwapChain);
	UNREFERENCED_PARAMETER(pUserContext);

    HRESULT hr;
    
    // Intialize the user interface

    V_RETURN( g_dialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_settingsDlg.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

    g_hud.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_hud.SetSize( 170, 170 );
    g_sampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
    g_sampleUI.SetSize( 170, 300 );

    // Initialize the camera

    g_cameraParams.aspect = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_cameraParams.fovy = 0.785398f;
    g_cameraParams.nearPlane = 1.f;
    g_cameraParams.farPlane = 5000.f;

    g_camera.SetProjParams(g_cameraParams.fovy, g_cameraParams.aspect, g_cameraParams.nearPlane, g_cameraParams.farPlane);
	g_camera.SetRotateButtons(true, false, false);
    g_camera.SetEnablePositionMovement(g_cameraMovement);
	g_camera.SetScalers( g_cameraRotateScaler, g_cameraMoveScaler );
	g_camera.SetDrag( true );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	UNREFERENCED_PARAMETER(pUserContext);
    g_dialogResourceManager.OnD3D11ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Loads the effect from file
// and retrieves all dependent variables
//--------------------------------------------------------------------------------------
HRESULT ReloadShader(ID3D11Device* pd3dDevice)
{
    assert(pd3dDevice != NULL);

    HRESULT hr;

    ReleaseShader();
	V_RETURN(g_gameEffect.create(pd3dDevice));

    g_spriteRenderer->reloadShader(pd3dDevice);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Release resources created in ReloadShader
//--------------------------------------------------------------------------------------
void ReleaseShader()
{
    g_spriteRenderer->releaseShader();
	g_gameEffect.destroy();
}

//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
	UNREFERENCED_PARAMETER(pUserContext);

    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_dialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_settingsDlg.IsActive() )
    {
        g_settingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_hud.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_sampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
        
    // Use the mouse weel to control the movement speed
    if(uMsg == WM_MOUSEWHEEL) {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        g_cameraMoveScaler *= (1 + zDelta / 500.0f);
        if (g_cameraMoveScaler < 0.1f)
          g_cameraMoveScaler = 0.1f;
        g_camera.SetScalers(g_cameraRotateScaler, g_cameraMoveScaler);
    }

    // Pass all remaining windows messages to camera so it can respond to user input
    g_camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	UNREFERENCED_PARAMETER(nChar);
	UNREFERENCED_PARAMETER(bKeyDown);
	UNREFERENCED_PARAMETER(bAltDown);
	UNREFERENCED_PARAMETER(pUserContext);

    // Weapon input
    if (nChar == 'D')
        g_weaponObjects[0]->active = bKeyDown;
    if (nChar == 'A')
        g_weaponObjects[1]->active = bKeyDown;
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	UNREFERENCED_PARAMETER(nEvent);
	UNREFERENCED_PARAMETER(pControl);
	UNREFERENCED_PARAMETER(pUserContext);

    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:
            g_settingsDlg.SetActive( !g_settingsDlg.IsActive() ); break;
        case IDC_TOGGLEMOVE:
            g_cameraMovement = g_sampleUI.GetCheckBox(IDC_TOGGLEMOVE)->GetChecked();
	        g_camera.SetEnablePositionMovement(g_cameraMovement);
            break;
        case IDC_TOGGLESHADOWDEBUG:
            g_debugShadows = g_sampleUI.GetCheckBox(IDC_TOGGLESHADOWDEBUG)->GetChecked();
            break;
		case IDC_RELOAD_SHADERS:
			ReloadShader(DXUTGetD3D11Device ());
			break;
    }
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	UNREFERENCED_PARAMETER(pUserContext);
    // Update the camera's position based on user input 
    g_camera.FrameMove( fElapsedTime );
    g_cameraObject->worldMatrix = g_camera.GetWorldMatrix();

    // Remove enemies
    g_enemyObjects.remove_if( 
        [] (const EnemyObject& e)
        {
            if (e.health <= 0)
            {
                g_Explosions.push_back(*g_ExplosionPrototype.get());
                g_Explosions.back().position = e.position;
                g_Explosions.back().size *= e.size;
                g_Explosions.back().Init(
                    g_ConfigParser.get_Explosion().particle_min_velocity,
                    g_ConfigParser.get_Explosion().particle_max_velocity,
                    g_ConfigParser.get_Explosion().particle_min_lifetime,
                    g_ConfigParser.get_Explosion().particle_max_lifetime);
                return true;
            }
            return XMVectorGetX(XMVector3LengthEst(e.position)) > g_ConfigParser.get_SpawnBehaviour().despawn_radius;
        });
    // Update enemies
    for (auto& e : g_enemyObjects)
        e.update(fElapsedTime);
    // Spawn enemies
    g_timeSinceLastEnemy += fElapsedTime;
    if (g_timeSinceLastEnemy > g_ConfigParser.get_SpawnBehaviour().interval)
    {
        SpawnEnemy();
        g_timeSinceLastEnemy -= g_ConfigParser.get_SpawnBehaviour().interval;
    }

    // Remove Projectiles
    g_Projectiles.remove_if( 
        [] (const Projectile& p)
        {
            return XMVectorGetX(XMVector3LengthEst(p.position)) > g_ConfigParser.get_SpawnBehaviour().despawn_radius;
        });
    // Update Projectiles
    for (auto& p : g_Projectiles)
        p.update(fElapsedTime, g_gravity);
    // Shoot
    for (auto& w : g_weaponObjects)
        if (w->update(fElapsedTime))
        {
            g_Projectiles.push_back(*w->projectile); // Copy constructor
            auto& proj = g_Projectiles.back();

            proj.position = XMVector3Transform(w->spawnpoint, w->getWorldMatrix());
            proj.velocity *= g_camera.GetWorldAhead();
        }
    // Projectile collision and damage
    g_Projectiles.remove_if( 
        [] (const Projectile& p)
        {
            for (auto& e : g_enemyObjects)
                if (XMVectorGetX(XMVector3LengthSq(e.position - p.position)) < (p.size + e.size) * (p.size + e.size))
                {
                    e.health -= p.damage;
                    return true;
                }
            return false;
        });

    // Remove explosions
    g_Explosions.remove_if(
        [] (const Explosion& e) 
        {
            return e.time >= e.duration;
        });
    // Update explosions
    for (auto& e : g_Explosions)
        e.update(fElapsedTime, g_gravity);
}

void SpawnEnemy()
{
    int rand_num = rand() % g_enemyPrototypes.size();
    
    // Uses the copy constructor to copy the prototype in its current state
    g_enemyObjects.push_back(*g_enemyPrototypes[rand_num]);
    EnemyObject& enemy = g_enemyObjects.back();

    enemy.type = g_enemyPrototypes[rand_num];

    float spawn_circle = XM_2PI * static_cast<float>(rand()) / RAND_MAX;
    float spawn_height = (static_cast<float>(rand()) / RAND_MAX)
        * (g_ConfigParser.get_SpawnBehaviour().max_height - g_ConfigParser.get_SpawnBehaviour().min_height)
        + g_ConfigParser.get_SpawnBehaviour().min_height;
    float target_cicle = XM_2PI * static_cast<float>(rand()) / RAND_MAX;
    XMVECTOR target_pos = {
        g_ConfigParser.get_SpawnBehaviour().target_radius * sin(target_cicle),
        spawn_height* g_ConfigParser.get_terrain().height,
        g_ConfigParser.get_SpawnBehaviour().target_radius * cos(target_cicle) };
    
    enemy.position = {
        g_ConfigParser.get_SpawnBehaviour().spawn_radius * sin(spawn_circle),
        spawn_height * g_ConfigParser.get_terrain().height,
        g_ConfigParser.get_SpawnBehaviour().spawn_radius * cos(spawn_circle) };
    
    enemy.velocity *= XMVector3Normalize(target_pos - enemy.position);
    
    enemy.rotation = { 
        0.0f, 
        atan2(XMVectorGetX(enemy.velocity), XMVectorGetZ(enemy.velocity)), 
        0.0f };
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
        float fElapsedTime, void* pUserContext )
{
	UNREFERENCED_PARAMETER(pd3dDevice);
	UNREFERENCED_PARAMETER(fTime);
	UNREFERENCED_PARAMETER(pUserContext);

    HRESULT hr;

    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_settingsDlg.IsActive() )
    {
        g_settingsDlg.OnRender( fElapsedTime );
        return;
    }     

    // Show an error if the shader file could not be loaded
	if(g_gameEffect.effect == NULL) {
        g_txtHelper->Begin();
        g_txtHelper->SetInsertionPos( 5, 5 );
        g_txtHelper->SetForegroundColor( XMVectorSet( 1.0f, 1.0f, 0.0f, 1.0f ) );
        g_txtHelper->DrawTextLine( L"SHADER ERROR" );
        g_txtHelper->End();
        return;
    }

    g_DefaultRenderTarget = DXUTGetD3D11RenderTargetView();
    g_DefaultDepthStencil = DXUTGetD3D11DepthStencilView();
    
    clearFrameBuffers(pd3dImmediateContext, g_ClearColor);

    // Update variables that change once per frame
    XMMATRIX const view = g_camera.GetViewMatrix(); // http://msdn.microsoft.com/en-us/library/windows/desktop/bb206342%28v=vs.85%29.aspx
    XMMATRIX const proj = g_camera.GetProjMatrix(); // http://msdn.microsoft.com/en-us/library/windows/desktop/bb147302%28v=vs.85%29.aspx
    XMMATRIX const lightView = XMMatrixLookToLH({ 0, 0, 0 }, -g_lightDir, {0, 1, 0}); // Center ed on the origin
    XMMATRIX const lightProj = XMMatrixOrthographicLH(
        g_ConfigParser.get_SpawnBehaviour().despawn_radius * 2,
        g_ConfigParser.get_SpawnBehaviour().despawn_radius * 2,
       -g_ConfigParser.get_SpawnBehaviour().despawn_radius,
        g_ConfigParser.get_SpawnBehaviour().despawn_radius);
    XMMATRIX const viewProj = view * proj;
    XMMATRIX const lightViewProj = lightView * lightProj;

    renderShadowMap(pd3dImmediateContext, g_ShadowStencil, g_ShadowViewport, lightViewProj);
    
	V(g_gameEffect.lightDirEV->SetFloatVector( ( float* )&g_lightDir ));
    V(g_gameEffect.cameraPosWorldEV->SetFloatVector((float*)&g_camera.GetEyePt()));
    V(g_gameEffect.shadowEV->SetResource(g_ShadowMapSRV));
    
    renderObjects(pd3dImmediateContext, viewProj, lightViewProj);
    renderSprites(pd3dImmediateContext);
    if (g_debugShadows)
        drawShadowMap(pd3dImmediateContext);

    // Render HUD
    DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    V(g_hud.OnRender( fElapsedTime ));
    V(g_sampleUI.OnRender( fElapsedTime ));
    RenderText();
    DXUT_EndPerfEvent();

    static DWORD dwTimefirst = GetTickCount();
    if ( GetTickCount() - dwTimefirst > 2000 )
    {    
        OutputDebugString( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
        OutputDebugString( L"\n" );
        dwTimefirst = GetTickCount();
    }
}

void clearFrameBuffers(ID3D11DeviceContext* pd3dImmediateContext, XMVECTOR clearColor)
{
    // Clear the render target(s)
    pd3dImmediateContext->ClearRenderTargetView(g_DefaultRenderTarget, (float*)&clearColor);

    // Clear the depth stencil(s)
    pd3dImmediateContext->ClearDepthStencilView(g_DefaultDepthStencil, D3D11_CLEAR_DEPTH, 1.0, 0);
    pd3dImmediateContext->ClearDepthStencilView(g_ShadowStencil, D3D11_CLEAR_DEPTH, 1.0, 0);
    
    // Unbind all current resources
    pd3dImmediateContext->PSSetShaderResources(0, 128, (ID3D11ShaderResourceView**)(g_nullptrs));
}

void renderShadowMap(
    ID3D11DeviceContext* pd3dImmediateContext, 
    ID3D11DepthStencilView* shadowStencil,
    D3D11_VIEWPORT* shadowViewPort,
    const XMMATRIX& viewProj)
{
    // Save the current viewport
    UINT viewport_count = 1;
    pd3dImmediateContext->RSGetViewports(&viewport_count, g_DefaultViewports);
    
    // Set render targets to shadow mapping
    pd3dImmediateContext->OMSetRenderTargets(0, nullptr, g_ShadowStencil);
    pd3dImmediateContext->RSSetViewports(1, g_ShadowViewport);

    // Render objects to shadow map
    for (const auto& w : g_weaponObjects)
        w->renderDepthOnly(pd3dImmediateContext, viewProj);
    for (const auto& o : g_gameObjects)
        o->renderDepthOnly(pd3dImmediateContext, viewProj);
    for (const auto& e : g_enemyObjects)
        e.renderDepthOnly(pd3dImmediateContext, viewProj);
    g_terrain.renderDepthOnly(pd3dImmediateContext, viewProj);

    // Restore render targets
    pd3dImmediateContext->OMSetRenderTargets(1, &g_DefaultRenderTarget, g_DefaultDepthStencil);
    pd3dImmediateContext->RSSetViewports(1, g_DefaultViewports);
}

void renderObjects(
    ID3D11DeviceContext* pd3dImmediateContext,
    const XMMATRIX& viewProj,
    const XMMATRIX& lightViewProj)
{
    for (const auto& w : g_weaponObjects)
        w->render(pd3dImmediateContext, viewProj, lightViewProj);
    for (const auto& o : g_gameObjects)
        o->render(pd3dImmediateContext, viewProj, lightViewProj);
    for (const auto& e : g_enemyObjects)
        e.render(pd3dImmediateContext, viewProj, lightViewProj);
    g_terrain.render(pd3dImmediateContext, viewProj, lightViewProj);
}

void renderSprites(ID3D11DeviceContext* pd3dImmediateContext)
{
    size_t sprite_count =
        g_Projectiles.size()
        + g_Explosions.size()
        + g_Explosions.size() * g_ConfigParser.get_Explosion().particle_count;

    if (sprite_count <= 0)
        return;

    if (sprite_count > g_sprites.capacity())
        g_sprites.resize(sprite_count * 2);

    XMVECTOR camera_ahead = g_camera.GetWorldAhead();
    size_t i = 0;

    for (auto& p : g_Projectiles)
    {
        g_sprites[i] = p.GetSpriteVertex(camera_ahead);
        i++;
    }

    for (auto& e : g_Explosions)
    {
        g_sprites[i] = e.GetSpriteVertex(camera_ahead);
        g_sprites[i].time = e.time / e.duration;
        i++;

        for (auto& p : e.explosionParticles)
        {
            g_sprites[i] = p.GetSpriteVertex(camera_ahead, e.position);
            g_sprites[i].time = p.time / p.duration;
            i++;
        }
    }

    std::sort(g_sprites.begin(), g_sprites.end(),
        [](const SpriteVertex& a, const SpriteVertex& b)
        {
            return a.camera_distance > b.camera_distance;
        });

    g_spriteRenderer->renderSprites(pd3dImmediateContext, g_sprites, g_camera, static_cast<int>(i), 0);
}

void drawShadowMap(ID3D11DeviceContext* pd3dImmediateContext)
{
    ID3D11Buffer* vbs[] = { nullptr, };
    unsigned int strides[] = { 0, }, offsets[] = { 0, };
    pd3dImmediateContext->IASetVertexBuffers(0, 1, vbs, strides, offsets);
    pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    g_gameEffect.debugShadowPass->Apply(0, pd3dImmediateContext);
    pd3dImmediateContext->Draw(4, 0);
}
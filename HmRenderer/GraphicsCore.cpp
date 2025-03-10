#include "GraphicsCore.h"
#include "Renderer.h"

#define CAM_NEAR 0.1f
#define CAM_FAR 1000.0f

GraphicsCore::GraphicsCore()
	: m_pRenderer(nullptr)
{
}

GraphicsCore::~GraphicsCore()
{
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
}

void GraphicsCore::Initialize(HWND _hHwnd, int _iWidth, int _iHeight)
{
	CreateSwapChainAndDeviceContext(_hHwnd, _iWidth, _iHeight);
	CreateRTV();
	CreateDSV(_iWidth, _iHeight);
	CreateRasterizerState();
	CreateBlendState();
	CreateViewport(_iWidth, _iHeight);
	CreateWVPMatrix(_iWidth, _iHeight, CAM_NEAR, CAM_FAR);

	m_pRenderer = new Renderer;
	m_pRenderer->SetOwner(this);
	m_pRenderer->Initialize(_hHwnd, _iWidth, _iHeight);
}

void GraphicsCore::Update(float _fDeltaTime)
{
	m_pRenderer->Update(_fDeltaTime);
}

void GraphicsCore::Render(float _fDeltaTime)
{
	RenderBegin();
	m_pRenderer->Render(_fDeltaTime);
	RenderEnd();
}

void GraphicsCore::RenderBegin()
{
	float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), fClearColor);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
}

void GraphicsCore::RenderEnd()
{
	m_pSwapChain->Present(0, 0);
}

void GraphicsCore::CreateSwapChainAndDeviceContext(HWND _hHwnd, int _iWidth, int _iHeight)
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Width = _iWidth;
	SwapChainDesc.BufferDesc.Height = _iHeight;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = _hHwnd;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.Windowed = true;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED, &FeatureLevel, 1,
		D3D11_SDK_VERSION, &SwapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pContext);

	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateSwapChainAndDeviceContext(HWND _hHwnd, int _iWidth, int _iHeight) -> 생성 실패!");
}

void GraphicsCore::CreateRTV()
{
	HRESULT hResult;
	ID3D11Texture2D* pBackBufferPtr;

	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferPtr);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateRTV() -> SwapChain으로부터 BackBuffer 가져오기 실패!");

	hResult = m_pDevice->CreateRenderTargetView(pBackBufferPtr, NULL, &m_pRenderTargetView);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateRTV() -> BackBuffer를 이용하여 RTV 생성 실패!");

	pBackBufferPtr->Release();
	pBackBufferPtr = nullptr;
}

void GraphicsCore::CreateDSV(int _iWidth, int _iHeight)
{
	D3D11_TEXTURE2D_DESC DepthStencilDesc;

	DepthStencilDesc.Width = _iWidth;
	DepthStencilDesc.Height = _iHeight;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;

	HRESULT hResult;
	hResult = m_pDevice->CreateTexture2D(&DepthStencilDesc, NULL, m_pDepthStencilBuffer.GetAddressOf());
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateDSV(int _iWidth, int _iHeight) -> DepthTexture 생성 실패");

	hResult = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), NULL, m_pDepthStencilView.GetAddressOf());
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateDSV(int _iWidth, int _iHeight) -> DSV 생성 실패");
}

void GraphicsCore::CreateRasterizerState()
{
	HRESULT hResult;

	D3D11_RASTERIZER_DESC RasterizerStateDesc;
	ZeroMemory(&RasterizerStateDesc, sizeof(RasterizerStateDesc));
	RasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerStateDesc.CullMode = D3D11_CULL_BACK;
	RasterizerStateDesc.DepthClipEnable = true;
	hResult = m_pDevice->CreateRasterizerState(&RasterizerStateDesc, &m_pBackFaceCull);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateRasterizerState() -> Backface Rasterizer 생성 실패");

	ZeroMemory(&RasterizerStateDesc, sizeof(RasterizerStateDesc));
	RasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerStateDesc.CullMode = D3D11_CULL_FRONT;
	RasterizerStateDesc.DepthClipEnable = true;
	hResult = m_pDevice->CreateRasterizerState(&RasterizerStateDesc, &m_pFrontFaceCull);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateRasterizerState() -> Frontface Rasterizer 생성 실패");
}

void GraphicsCore::CreateBlendState()
{
	D3D11_BLEND_DESC BlendStateDesc;
	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));

	BlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	HRESULT hResult;
	hResult = m_pDevice->CreateBlendState(&BlendStateDesc, &m_pAlphaState);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateBlendState() -> Alpha BlendState 생성 실패");
	BlendStateDesc.RenderTarget[0].BlendEnable = FALSE;

	hResult = m_pDevice->CreateBlendState(&BlendStateDesc, &m_pNoAlphaState);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateBlendState() -> No Alpha BlendState 생성 실패");
}

void GraphicsCore::CreateViewport(int _iWidth, int _iHeight)
{
	// Setup the (default) viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)_iWidth;
	vp.Height = (FLOAT)_iHeight;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pContext->RSSetViewports(1, &vp);
}

void GraphicsCore::CreateWVPMatrix(int _iWidth, int _iHeight, float _fNear, float _fFar)
{
	float fFieldOfView = 3.141592654f / 4.0f;
	float fScreenAspect = (float)_iWidth / (float)_iHeight;

	DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fFieldOfView, fScreenAspect, _fNear, _fFar));
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_OrthoMatrix, DirectX::XMMatrixOrthographicLH((float)_iWidth, (float)_iHeight, _fNear, _fFar));
}

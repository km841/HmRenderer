#include "GraphicsCore.h"

GraphicsCore::GraphicsCore()
{
}

GraphicsCore::~GraphicsCore()
{
}

void GraphicsCore::Initialize(HWND _hHwnd, int _iWidth, int _iHeight)
{
	CreateSwapChainAndDeviceContext(_hHwnd, _iWidth, _iHeight);
	CreateRTV();
	CreateDSV(_iWidth, _iHeight);
}

void GraphicsCore::Update(float _fDeltaTime)
{
}

void GraphicsCore::Render(float _fDeltaTime)
{
	RenderBegin();
	RenderEnd();
}

void GraphicsCore::RenderBegin()
{
	float fClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
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
	ID3D11Texture2D* backBufferPtr;

	hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateRTV() -> SwapChain으로부터 BackBuffer 가져오기 실패!");

	hResult = m_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_pRenderTargetView);
	AssertEx(SUCCEEDED(hResult), L"void GraphicsCore::CreateRTV() -> BackBuffer를 이용하여 RTV 생성 실패!");

	backBufferPtr->Release();
	backBufferPtr = nullptr;
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

	m_pDevice->CreateTexture2D(&DepthStencilDesc, NULL, m_pDepthStencilBuffer.GetAddressOf());
	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), NULL, m_pDepthStencilView.GetAddressOf());
}

#pragma once
#include "Common.h"

class GraphicsCore
{
public:
	GraphicsCore();
	~GraphicsCore();

public:
	void Initialize(HWND _hHwnd, int _iWidth, int _iHeight);
	void Update(float _fDeltaTime);
	void Render(float _fDeltaTime);

	void RenderBegin();
	void RenderEnd();

private:
	void CreateSwapChainAndDeviceContext(HWND _hHwnd, int _iWidth, int _iHeight);
	void CreateRTV();
	void CreateDSV(int _iWidth, int _iHeight);

private:
	Matrix m_pProjectionMatrix;
	Matrix m_pWorldMatrix;
	Matrix m_pOrthoMatrix;

private:
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pContext;

private:
	ComPtr<IDXGISwapChain> m_pSwapChain;

private:
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

private:
	ComPtr<ID3D11RasterizerState> m_pBackFaceCull;
	ComPtr<ID3D11RasterizerState> m_pFrontFaceCull;
	ComPtr<ID3D11BlendState> m_pAlphaState;
	ComPtr<ID3D11BlendState> m_pNoAlphaState;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
};


#include "Graphics.h"
#include "GraphicsCore.h"
Graphics::Graphics()
	: m_pCore(nullptr)
{

}

Graphics::~Graphics()
{
	if (m_pCore)
	{
		delete m_pCore;
		m_pCore = nullptr;
	}

}

void Graphics::Initialize(HWND _hHwnd, int _iWidth, int _iHeight)
{
	m_pCore = new GraphicsCore;
	m_pCore->Initialize(_hHwnd, _iWidth, _iHeight);
}

void Graphics::Update(float _fDeltaTime)
{
	m_pCore->Update(_fDeltaTime);
}

void Graphics::Render(float _fDeltaTime)
{
	m_pCore->Render(_fDeltaTime);
}

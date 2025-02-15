#include "Graphics.h"
#include "GraphicsCore.h"
Graphics::Graphics()
	: m_pCore(nullptr)
{

}

Graphics::~Graphics()
{

}

void Graphics::Initialize()
{
	m_pCore->Initialize();
}

void Graphics::Update(float _fDeltaTime)
{
	m_pCore->Update(_fDeltaTime);
}

void Graphics::Render()
{
	m_pCore->RenderBegin();
	m_pCore->RenderEnd();
}

#pragma once
#include "Common.h"
class Shader
{
public:
	Shader();
	virtual ~Shader() {}

public:
	virtual void Initialize(ComPtr<ID3D11Device> _pDevice) {};

protected:
	virtual void CreateVertexShader(ComPtr<ID3D11Device> _pDevice) {};
	virtual void CreateInputLayout(ComPtr<ID3D11Device> _pDevice) {};
	virtual void CreatePixelShader(ComPtr<ID3D11Device> _pDevice) {};
	virtual void CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice) {};

	HRESULT CompileShaderFromFile(const LPCWSTR _strShaderFileName, const LPCSTR _strShaderEntryPoint, const LPCSTR _strShaderModel, ID3DBlob** const _ppBlobOut);

public:
	ComPtr<ID3D11VertexShader> GetVertexShader() { return m_pVertexShader; }
	ComPtr<ID3D11PixelShader> GetPixelShader() { return m_pPixelShader; }
	ComPtr<ID3D11InputLayout> GetInputLayout() { return m_pInputLayout; }

protected:
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11InputLayout> m_pInputLayout;
	ComPtr<ID3D11PixelShader> m_pPixelShader;
};


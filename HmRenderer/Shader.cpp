#include "Shader.h"

Shader::Shader()
{
}

HRESULT Shader::CompileShaderFromFile(const LPCWSTR _strShaderFileName, const LPCSTR _strShaderEntryPoint, const LPCSTR _strShaderModel, ID3DBlob** const _ppBlobOut)
{
	HRESULT hr;
	DWORD dwShaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(_strShaderFileName, NULL, NULL, _strShaderEntryPoint, _strShaderModel, dwShaderFlags, 0,
		_ppBlobOut, &pErrorBlob);
	return hr;
}

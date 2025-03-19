#include "Shader.h"
#include <fstream>

Shader::Shader()
{
}

HRESULT Shader::CompileShaderFromFile(const LPCWSTR _strShaderFileName, const LPCSTR _strShaderEntryPoint, const LPCSTR _strShaderModel, ID3DBlob** const _ppBlobOut)
{
	ID3DBlob* pErrorBlob;
	int compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(_strShaderFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_strShaderEntryPoint, _strShaderModel, compileFlag, 0, _ppBlobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		const char* errorString = NULL;
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
			errorString = "Could not compile shader; file not found";
		else if (pErrorBlob) {
			errorString = (const char*)pErrorBlob->GetBufferPointer();
			pErrorBlob->Release();
		}
		AssertEx(NULL, L"Shader::CreateShader() - Shader Compiler Error");
	}
	return hr;
}

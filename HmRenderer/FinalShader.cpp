#include "FinalShader.h"

FinalShader::FinalShader()
{
}

FinalShader::~FinalShader()
{
}

void FinalShader::Initialize(ComPtr<ID3D11Device> _pDevice)
{
	CreateVertexShader(_pDevice);
	CreateInputLayout(_pDevice);
	CreatePixelShader(_pDevice);
}

void FinalShader::CreateVertexShader(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;

	hResult = CompileShaderFromFile(L"../Resources/Shader/final.hlsl", "FinalVS", "vs_5_0", &m_vsBlob);
	hResult = _pDevice->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	AssertEx(SUCCEEDED(hResult), L"void FinalShader::CreateVertexShader(ComPtr<ID3D11Device> _pDevice) -> Vertex Shader 생성 실패!");
}

void FinalShader::CreateInputLayout(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;

	D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[1];
	InputLayoutDesc[0].SemanticName = "POSITION"; //match name in VS shader
	InputLayoutDesc[0].SemanticIndex = 0;
	InputLayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	InputLayoutDesc[0].InputSlot = 0;
	InputLayoutDesc[0].AlignedByteOffset = 0;
	InputLayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	InputLayoutDesc[0].InstanceDataStepRate = 0;

	UINT numElements = ARRAYSIZE(InputLayoutDesc);

	hResult = _pDevice->CreateInputLayout(InputLayoutDesc, numElements, m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), &m_pInputLayout);
	AssertEx(SUCCEEDED(hResult), L"void FinalShader::CreateInputLayout(ComPtr<ID3D11Device> _pDevice) -> Input Layout 생성 실패!");
}

void FinalShader::CreatePixelShader(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;

	hResult = CompileShaderFromFile(L"../Resources/Shader/final.hlsl", "FinalPS", "ps_5_0", &m_psBlob);
	hResult = _pDevice->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	AssertEx(SUCCEEDED(hResult), L"void FinalShader::CreatePixelShader(ComPtr<ID3D11Device> _pDevice) -> Pixel Shader 생성 실패!");
}

#include "ModelShader.h"

ModelShader::ModelShader()
{
}

ModelShader::~ModelShader()
{
}

void ModelShader::Initialize(ComPtr<ID3D11Device> _pDevice)
{
	CreateVertexShader(_pDevice);
	CreateInputLayout(_pDevice);
	CreatePixelShader(_pDevice);
	CreateConstantBuffer(_pDevice);
}

void ModelShader::CreateVertexShader(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;

	hResult = CompileShaderFromFile(L"../Resources/Shader/model_position.hlsl", "ModelPositionVS", "vs_5_0", &m_vsBlob);
	hResult = _pDevice->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	AssertEx(SUCCEEDED(hResult), L"void ModelShader::CreateVertexShader(ComPtr<ID3D11Device> _pDevice) -> Vertex Shader 생성 실패!");
}

void ModelShader::CreateInputLayout(ComPtr<ID3D11Device> _pDevice)
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
	AssertEx(SUCCEEDED(hResult), L"void ModelShader::CreateInputLayout(ComPtr<ID3D11Device> _pDevice) -> Input Layout 생성 실패!");
}

void ModelShader::CreatePixelShader(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;

	hResult = CompileShaderFromFile(L"../Resources/Shader/model_position.hlsl", "ModelPositionPS", "ps_5_0", &m_psBlob);
	hResult = _pDevice->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	AssertEx(SUCCEEDED(hResult), L"void ModelShader::CreatePixelShader(ComPtr<ID3D11Device> _pDevice) -> Pixel Shader 생성 실패!");
}

void ModelShader::CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice)
{
	ID3DBlob* pBlob = nullptr;
	HRESULT hResult;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hResult = _pDevice->CreateBuffer(&bd, NULL, &m_pMatrixBuffer);
	AssertEx(SUCCEEDED(hResult), L"void ModelShader::CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice) -> Constant Buffer 생성 실패!");
}

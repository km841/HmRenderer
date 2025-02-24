#include "RaycastShader.h"

RaycastShader::RaycastShader()
{
}

RaycastShader::~RaycastShader()
{
}

void RaycastShader::Initialize(ComPtr<ID3D11Device> _pDevice, int _iWidth, int _iHeight)
{
	CreateVertexShader(_pDevice);
	CreateInputLayout(_pDevice);
	CreatePixelShader(_pDevice);
	CreateConstantBuffer(_pDevice, _iWidth, _iHeight);
}

void RaycastShader::UpdateConstantBuffer(ComPtr<ID3D11DeviceContext> _pContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubResource = {};
	HRESULT hr = _pContext->Map(m_pWindowSizeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	AssertEx(SUCCEEDED(hr), L"ConstantBuffer::PushData() - ConstantBuffer�� �����͸� ����ִ� �������� ���� �߻�");
	memcpy(mappedSubResource.pData, &m_WindowData, sizeof(WindowSizeBuffer));
	_pContext->Unmap(m_pWindowSizeBuffer.Get(), 0);
}

void RaycastShader::CreateVertexShader(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;
	hResult = CompileShaderFromFile(L"../Resources/Shader/raycast.hlsl", "RayCastVS", "vs_5_0", &m_vsBlob);
	hResult = _pDevice->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	AssertEx(SUCCEEDED(hResult), L"void RaycastShader::CreateVertexShader(ComPtr<ID3D11Device> _pDevice) -> Vertex Shader 생성 실패!");
}

void RaycastShader::CreateInputLayout(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;
	// Shader Input Layout
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
	AssertEx(SUCCEEDED(hResult), L"void RaycastShader::CreateInputLayout(ComPtr<ID3D11Device> _pDevice) -> Input Layout 생성 실패!");
}

void RaycastShader::CreatePixelShader(ComPtr<ID3D11Device> _pDevice)
{
	HRESULT hResult;
	// Compile and create the pixel shader
	hResult = CompileShaderFromFile(L"../Resources/Shader/raycast.hlsl", "RayCastPS", "ps_5_0", &m_psBlob);
	hResult = _pDevice->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	AssertEx(SUCCEEDED(hResult), L"void RaycastShader::CreatePixelShader(ComPtr<ID3D11Device> _pDevice) -> Pixel Shader 생성 실패!");
}

void RaycastShader::CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice, int _iWidth, int _iHeight)
{
	HRESULT hResult;
	m_WindowData.fWindowSize[0] = 1.f / _iWidth;
	m_WindowData.fWindowSize[1] = 1.f / _iHeight;
	m_WindowData.iIsBone = false;
	m_WindowData.iIsCartilage = false;

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA BufferInitData;
	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&BufferInitData, sizeof(BufferInitData));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(WindowSizeBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferInitData.pSysMem = &m_WindowData;
	hResult = _pDevice->CreateBuffer(&bd, &BufferInitData, &m_pWindowSizeBuffer);
	AssertEx(SUCCEEDED(hResult), L"void RaycastShader::CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice, int _iWidth, int _iHeight) -> Constant Buffer 생성 실패!");
}

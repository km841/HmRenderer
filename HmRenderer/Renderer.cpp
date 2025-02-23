#include "Renderer.h"
#include "GraphicsCore.h"
#include "ModelShader.h"
#include "RaycastShader.h"
#include "TimeManager.h"
#include "Camera.h"

#define DEVICE m_pOwner->GetDevice()
#define CONETXT m_pOwner->GetContext()
#define VOLUME_SIZE 256

Renderer::Renderer()
	: m_pModelShader(nullptr)
	, m_pRaycastShader(nullptr)
	, m_pOwner(nullptr)
	, m_fRotation(1.0f)
{
}

Renderer::~Renderer()
{
	if (m_pModelShader)
	{
		delete m_pModelShader;
		m_pModelShader = nullptr;
	}

	if (m_pRaycastShader)
	{
		delete m_pRaycastShader;
		m_pRaycastShader = nullptr;
	}

	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
}

void Renderer::Initialize(HWND _hHwnd, int _iWidth, int _iHeight)
{
	m_pCamera = new Camera;
	m_pCamera->Initialize(_iWidth, _iHeight);

	m_pModelShader = new ModelShader;
	m_pModelShader->Initialize(DEVICE);

	m_pRaycastShader = new RaycastShader;
	m_pRaycastShader->Initialize(DEVICE, _iWidth, _iHeight);

	CreateRenderTexture(_iWidth, _iHeight);
	CreateSampler();
	LoadVolume(_T("../Resources/Model/foot.raw"));
	CreateCube();
	CreateViewProjMatrix();
}

void Renderer::Update(float _fDeltaTime)
{
	m_pCamera->Update(_fDeltaTime);
	// rotate rendered volume around y-axis (oo so fancy :P)
	m_fRotation += 1.2f * _fDeltaTime;
	DirectX::XMStoreFloat4(&m_matRotation, DirectX::XMQuaternionRotationRollPitchYaw(0, m_fRotation, 0));
}

void Renderer::Render(float _fDeltaTime)
{
	float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };

	// Set vertex buffer
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	CONETXT->IASetVertexBuffers(0, 1, m_pCubeVB.GetAddressOf(), &stride, &offset);
	// Set index buffer
	CONETXT->IASetIndexBuffer(m_pCubeIB.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	CONETXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the input layout
	CONETXT->IASetInputLayout(m_pModelShader->GetInputLayout().Get());

	//----------------------------------------------------------------------------//
	// Create our MVP transforms 
	//-----------------------------------------------------------------------------//
	DirectX::XMMATRIX matScale = DirectX::XMMatrixScaling(1, 1, 1);
	DirectX::XMMATRIX matRotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_matRotation)); //DirectX::XMMatrixRotationRollPitchYaw(0, rot, 0);
	DirectX::XMMATRIX matTransform = DirectX::XMMatrixTranslation(0, 0, 0);

	DirectX::XMMATRIX matWorld = matScale * matRotation * matTransform;

	DirectX::XMMATRIX matWorldFinal = matWorld;


	Matrix CamVP = m_pCamera->GetVP();
	MatrixBuffer cb;
	cb.WVP = XMMatrixMultiply(XMLoadFloat4x4(&CamVP), matWorldFinal);
	CONETXT->UpdateSubresource(m_pModelShader->GetMatrixBuffer().Get(), 0, NULL, &cb, 0, 0);

	//-----------------------------------------------------------------------------//
	// Back and front buffer for faces of the volume
	//-----------------------------------------------------------------------------//

	// Set the vertex shader ~ simple model shader
	CONETXT->VSSetShader(m_pModelShader->GetVertexShader().Get(), NULL, 0);
	CONETXT->VSSetConstantBuffers(0, 1, m_pModelShader->GetMatrixBuffer().GetAddressOf());

	// Set the pixel shader ~ simple model shader
	CONETXT->PSSetShader(m_pModelShader->GetPixelShader().Get(), NULL, 0);

	CONETXT->RSSetState(m_pOwner->GetBackFaceRS().Get());
	CONETXT->ClearRenderTargetView(m_pModelRTVBack.Get(), clearColor);
	CONETXT->OMSetRenderTargets(1, m_pModelRTVBack.GetAddressOf(), NULL);
	CONETXT->DrawIndexed(36, 0, 0);		// Draw back faces

	// Back-face culling
	CONETXT->RSSetState(m_pOwner->GetFrontFaceRS().Get());
	CONETXT->ClearRenderTargetView(m_pModelRTVFront.Get(), clearColor);
	CONETXT->OMSetRenderTargets(1, m_pModelRTVFront.GetAddressOf(), NULL);
	CONETXT->DrawIndexed(36, 0, 0);		// Draw front faces

	//-----------------------------------------------------------------------------//
	// Ray-casting / Volume Rendering 
	//-----------------------------------------------------------------------------//

	// Set the input layout
	CONETXT->IASetInputLayout(m_pModelShader->GetInputLayout().Get());

	// Render to standard render target
	CONETXT->OMSetRenderTargets(1, m_pOwner->GetRTV().GetAddressOf(), NULL);

	// Set the vertex shader to the Volume Renderer vertex program
	CONETXT->VSSetShader(m_pRaycastShader->GetVertexShader().Get(), NULL, 0);
	CONETXT->VSSetConstantBuffers(0, 1, m_pModelShader->GetMatrixBuffer().GetAddressOf());

	// Set the pixel shader to the Volume Render pixel program
	CONETXT->PSSetShader(m_pRaycastShader->GetPixelShader().Get(), NULL, 0);
	CONETXT->PSSetConstantBuffers(0, 1, m_pRaycastShader->GetWindowSizeBuffer().GetAddressOf());

	// Set texture sampler
	CONETXT->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());

	// pass in our textures )
	CONETXT->PSSetShaderResources(0, 1, m_pVolRSV.GetAddressOf()); // the loaded RAW file
	CONETXT->PSSetShaderResources(1, 1, m_pModelSRVFront.GetAddressOf()); // the front facing RT 
	CONETXT->PSSetShaderResources(2, 1, m_pModelSRVBack.GetAddressOf()); // the back facing RT

	// Draw the cube
	CONETXT->DrawIndexed(36, 0, 0);

	// Un-bind textures
	ID3D11ShaderResourceView* nullRV[3] = { NULL, NULL, NULL };
	CONETXT->PSSetShaderResources(0, 3, nullRV);
}

void Renderer::CreateCube()
{
	// Create ray-cast vertex and index buffers
	HRESULT hResult = S_OK;
	DirectX::XMFLOAT3 vertices[] =
	{
		DirectX::XMFLOAT3(-1.f, -1.f, -1.f),
		DirectX::XMFLOAT3(-1.f, -1.f, 1.f),
		DirectX::XMFLOAT3(-1.f, 1.f, -1.f),
		DirectX::XMFLOAT3(-1.f, 1.f, 1.f),
		DirectX::XMFLOAT3(1.f, -1.f, -1.f),
		DirectX::XMFLOAT3(1.f, -1.f, 1.f),
		DirectX::XMFLOAT3(1.f, 1.f, -1.f),
		DirectX::XMFLOAT3(1.f, 1.f, 1.f),
	};
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DirectX::XMFLOAT3) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA SubResourceDataDesc;
	ZeroMemory(&SubResourceDataDesc, sizeof(SubResourceDataDesc));
	SubResourceDataDesc.pSysMem = vertices;
	hResult = (DEVICE->CreateBuffer(&bd, &SubResourceDataDesc, &m_pCubeVB));

	// Create index buffer
	WORD indices[] =
	{
		0, 1, 2,
		2, 1, 3,

		0, 4, 1,
		1, 4, 5,

		0, 2, 4,
		4, 2, 6,

		1, 5, 3,
		3, 5, 7,

		2, 3, 6,
		6, 3, 7,

		5, 4, 7,
		7, 4, 6,
	};
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	ZeroMemory(&SubResourceDataDesc, sizeof(SubResourceDataDesc));
	SubResourceDataDesc.pSysMem = indices;
	hResult = (DEVICE->CreateBuffer(&bd, &SubResourceDataDesc, &m_pCubeIB));
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateCube() -> Cube 생성 실패!"));
}

void Renderer::CreateSampler()
{
	HRESULT hResult = S_OK;
	D3D11_SAMPLER_DESC SamplerDesc;
	ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hResult = DEVICE->CreateSamplerState(&SamplerDesc, &m_pSamplerLinear);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateSampler() -> Sampler 생성 실패!"));
}

void Renderer::CreateRenderTexture(int _iWidth, int _iHeight)
{
	HRESULT hResult;
	D3D11_TEXTURE2D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.ArraySize = 1;
	descTex.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descTex.Width = _iWidth;//m_D3D->m_ScreenWidth;
	descTex.Height = _iHeight;
	descTex.MipLevels = D3D11_BIND_VERTEX_BUFFER;
	descTex.SampleDesc.Count = 1;
	descTex.CPUAccessFlags = 0;

	// front facing RT
	hResult = DEVICE->CreateTexture2D(&descTex, NULL, &m_pModelTex2DFront);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateRenderTexture(int _iWidth, int _iHeight) -> ModelTexture Front 생성 실패!"));
	// Create resource view
	hResult = DEVICE->CreateShaderResourceView(m_pModelTex2DFront.Get(), NULL, &m_pModelSRVFront);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateRenderTexture(int _iWidth, int _iHeight) -> ModelFront SRV 생성 실패!"));
	// Create render target view
	hResult = DEVICE->CreateRenderTargetView(m_pModelTex2DFront.Get(), NULL, &m_pModelRTVFront);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateRenderTexture(int _iWidth, int _iHeight) -> ModelFront RTV 생성 실패!"));

	// back facing RT
	hResult = DEVICE->CreateTexture2D(&descTex, NULL, &m_pModelText2DBack);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateRenderTexture(int _iWidth, int _iHeight) -> ModelTexture Back 생성 실패!"));
	// Create resource view
	hResult = DEVICE->CreateShaderResourceView(m_pModelText2DBack.Get(), NULL, &m_pModelSRVBack);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateRenderTexture(int _iWidth, int _iHeight) -> ModelBack SRV 생성 실패!"));
	// Create render target view
	hResult = DEVICE->CreateRenderTargetView(m_pModelText2DBack.Get(), NULL, &m_pModelRTVBack);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateRenderTexture(int _iWidth, int _iHeight) -> ModelBack SRV 생성 실패!"));
}

void Renderer::LoadVolume(const wstring& _strFilename)
{
	HRESULT hResult;
	HANDLE hFile = CreateFileW(_strFilename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, OPEN_EXISTING, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, _T("Opening volume data file failed."), _T("Error"), MB_ICONERROR | MB_OK);

	}
	BYTE* pBuffer = (BYTE*)malloc(VOLUME_SIZE * VOLUME_SIZE * VOLUME_SIZE * sizeof(BYTE));

	DWORD numberOfBytesRead = 0;
	if (ReadFile(hFile, pBuffer, VOLUME_SIZE * VOLUME_SIZE * VOLUME_SIZE, &numberOfBytesRead, NULL) == 0)
	{
		MessageBox(NULL, _T("Reading volume data failed."), _T("Error"), MB_ICONERROR | MB_OK);
	}

	CloseHandle(hFile);

	D3D11_TEXTURE3D_DESC descTex;
	ZeroMemory(&descTex, sizeof(descTex));
	descTex.Height = VOLUME_SIZE;
	descTex.Width = VOLUME_SIZE;
	descTex.Depth = VOLUME_SIZE;
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R8_UNORM;
	descTex.Usage = D3D11_USAGE_DEFAULT;
	descTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_SHADER_RESOURCE;
	descTex.CPUAccessFlags = 0;
	// Initial data
	D3D11_SUBRESOURCE_DATA SubResourceDataDesc;
	ZeroMemory(&SubResourceDataDesc, sizeof(SubResourceDataDesc));
	SubResourceDataDesc.pSysMem = pBuffer;
	SubResourceDataDesc.SysMemPitch = VOLUME_SIZE;
	SubResourceDataDesc.SysMemSlicePitch = VOLUME_SIZE * VOLUME_SIZE;
	// Create texture
	hResult = (DEVICE->CreateTexture3D(&descTex, &SubResourceDataDesc, &m_pVolumeTex3D));
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::LoadVolume(const wstring& _strFilename) -> 3D Texture 생성 실패!"));

	// Create a resource view of the texture
	hResult = (DEVICE->CreateShaderResourceView(m_pVolumeTex3D.Get(), NULL, &m_pVolRSV));
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::LoadVolume(const wstring& _strFilename) -> Volume SRV 생성 실패!"));

	free(pBuffer);
}

void Renderer::CreateViewProjMatrix()
{
	// Initialize the view matrix
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.f, 1.5f, -5.0f, 0.f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.f, 0.0f, 0.f, 0.f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMMATRIX matView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eye, at, up));

	// Initialize the projection matrix
	DirectX::XMMATRIX matProjection = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 1.f, 0.1f, 10.f));

	// View-projection matrix	
	XMStoreFloat4x4(&m_ViewProj, XMMatrixMultiply(matProjection, matView));
}

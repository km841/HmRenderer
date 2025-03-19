#include "Renderer.h"
#include "GraphicsCore.h"
#include "ModelShader.h"
#include "RaycastShader.h"
#include "FinalShader.h"
#include "TimeManager.h"
#include "Camera.h"
#include "InputManager.h"

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

	if (m_pFinalShader)
	{
		delete m_pFinalShader;
		m_pFinalShader = nullptr;
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

	m_pFinalShader = new FinalShader;
	m_pFinalShader->Initialize(DEVICE);

	CreateRenderTexture(_iWidth, _iHeight);
	CreateLUT(_iWidth, _iHeight);
	CreateSampler();
	LoadVolume(_T("../Resources/Model/foot.raw"));
	CreateCube();
	CreateViewProjMatrix();
	CreateUAV(_iWidth, _iHeight);
	CreateRect();
	LoadSTL("../Resources/STL/300_polygon_sphere_100mm.STL");
}

void Renderer::Update(float _fDeltaTime)
{
	m_pCamera->Update(_fDeltaTime);
	// rotate rendered volume around y-axis (oo so fancy :P)
	m_fRotation += 1.2f * _fDeltaTime;
	DirectX::XMStoreFloat4(&m_matRotation, DirectX::XMQuaternionRotationRollPitchYaw(0, m_fRotation, 0));

	if (IS_DOWN(EKeyType::G))
	{
		m_pRaycastShader->ShowBone(m_pRaycastShader->IsShowBone() ? false : true);
		m_pRaycastShader->UpdateConstantBuffer(CONETXT);
	}

	if (IS_DOWN(EKeyType::H))
	{
		m_pRaycastShader->ShowCartilage(m_pRaycastShader->IsShowCartilage() ? false : true);
		m_pRaycastShader->UpdateConstantBuffer(CONETXT);
	}

	
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

	//CONETXT->CSSetShader(m_pRaycastShader->GetComputeShader().Get(), NULL, 0);

	// pass in our textures )
	CONETXT->PSSetShaderResources(0, 1, m_pVolRSV.GetAddressOf()); // the loaded RAW file
	CONETXT->PSSetShaderResources(1, 1, m_pModelSRVFront.GetAddressOf()); // the front facing RT 
	CONETXT->PSSetShaderResources(2, 1, m_pModelSRVBack.GetAddressOf()); // the back facing RT
	CONETXT->PSSetShaderResources(3, 1, m_pLUTSRV.GetAddressOf()); // the back facing RT

	//CONETXT->CSSetUnorderedAccessViews(0, 1, m_pRWUAV.GetAddressOf(), nullptr);
	//CONETXT->Dispatch((800 + 7) / 8, (600 + 7) / 8, 1);

	//CONETXT->VSSetShader(m_pFinalShader->GetVertexShader().Get(), NULL, 0);
	//CONETXT->VSSetConstantBuffers(0, 1, m_pModelShader->GetMatrixBuffer().GetAddressOf());

	//CONETXT->PSSetShader(m_pFinalShader->GetPixelShader().Get(), NULL, 0);
	//CONETXT->PSSetConstantBuffers(0, 1, m_pRaycastShader->GetWindowSizeBuffer().GetAddressOf());
	//CONETXT->PSSetShaderResources(1, 1, m_pRWSRV.GetAddressOf()); // the back facing RT

	//CONETXT->IASetVertexBuffers(0, 1, m_pRectVB.GetAddressOf(), &stride, &offset);
	//// Set index buffer
	//CONETXT->IASetIndexBuffer(m_pRectIB.Get(), DXGI_FORMAT_R16_UINT, 0);
	//CONETXT->RSSetState(m_pOwner->GetBackFaceRS().Get());
	// Draw the cube
	//CONETXT->DrawIndexed(36, 0, 0);

	// Set vertex buffer
	stride = sizeof(Vec3);
	offset = 0;
	CONETXT->IASetVertexBuffers(0, 1, m_pMeshVB.GetAddressOf(), &stride, &offset);
	// Set index buffer
	CONETXT->IASetIndexBuffer(m_pMeshIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology
	CONETXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the input layout
	CONETXT->IASetInputLayout(m_pModelShader->GetInputLayout().Get());

	CONETXT->VSSetShader(m_pFinalShader->GetVertexShader().Get(), NULL, 0);
	CONETXT->PSSetShader(m_pFinalShader->GetPixelShader().Get(), NULL, 0);
	CONETXT->VSSetConstantBuffers(0, 1, m_pModelShader->GetMatrixBuffer().GetAddressOf());

	CONETXT->RSSetState(m_pOwner->GetNoneRS().Get());
	CONETXT->OMSetRenderTargets(1, m_pOwner->GetRTV().GetAddressOf(), NULL);
	CONETXT->DrawIndexed(static_cast<uint32>(m_vecMeshIndices.size()), 0, 0);		// Draw back faces

	// Un-bind textures
	ID3D11ShaderResourceView* nullRV[4] = { NULL, NULL, NULL, NULL };
	CONETXT->PSSetShaderResources(0, 4, nullRV);
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

void Renderer::CreateRect()
{
	DirectX::XMFLOAT3 vertices[] =
	{
		DirectX::XMFLOAT3(-1.f, -1.f, 0.f),
		DirectX::XMFLOAT3(-1.f,  1.f, 0.f), 
		DirectX::XMFLOAT3(1.f, -1.f, 0.f), 
		DirectX::XMFLOAT3(1.f,  1.f, 0.f), 
	};

	// Vertex Buffer 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA SubResourceDataDesc;
	ZeroMemory(&SubResourceDataDesc, sizeof(SubResourceDataDesc));
	SubResourceDataDesc.pSysMem = vertices;

	HRESULT hResult = DEVICE->CreateBuffer(&bd, &SubResourceDataDesc, &m_pRectVB);
	AssertEx(SUCCEEDED(hResult), _T("Renderer::CreateQuad() -> Quad 생성 실패!"));

	// 인덱스 데이터 (두 개의 삼각형으로 Quad를 구성)
	WORD quadIndices[] =
	{
		0, 1, 2,  // 첫 번째 삼각형
		2, 1, 3   // 두 번째 삼각형
	};

	// Index Buffer 생성
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(quadIndices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&SubResourceDataDesc, sizeof(SubResourceDataDesc));
	SubResourceDataDesc.pSysMem = quadIndices;

	hResult = DEVICE->CreateBuffer(&bd, &SubResourceDataDesc, &m_pRectIB);
	AssertEx(SUCCEEDED(hResult), _T("Renderer::CreateQuad() -> Index Buffer 생성 실패!"));
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

void Renderer::CreateLUT(int _iWidth, int _iHeight)
{
	D3D11_TEXTURE1D_DESC TexDesc = {};
	TexDesc.Width = 256;  // 0~255 범위의 밀도 값
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // RGBA 색상 저장
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	std::vector<uint8> vecTransFunc(256 * 4);

	for (int i = 0; i < 256; ++i) 
	{
		float fNormDensity = i / 255.0f;
		if (fNormDensity < 0.35f)
		{  
			vecTransFunc[i * 4 + 0] = 0;   
			vecTransFunc[i * 4 + 1] = 0;   
			vecTransFunc[i * 4 + 2] = 0;  
			vecTransFunc[i * 4 + 3] = 0;   
		}
		else 
		{
			vecTransFunc[i * 4 + 0] = 255;
			vecTransFunc[i * 4 + 1] = 255;
			vecTransFunc[i * 4 + 2] = 255;
			vecTransFunc[i * 4 + 3] = 255; 
		}
	}

	D3D11_SUBRESOURCE_DATA InitDataDesc = { vecTransFunc.data(), 0, 0 };
	HRESULT hResult = DEVICE->CreateTexture1D(&TexDesc, &InitDataDesc, &m_pLUT);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateLUT(int _iWidth, int _iHeight) -> LUT Texture 생성 실패!"));

	hResult = DEVICE->CreateShaderResourceView(m_pLUT.Get(), NULL, &m_pLUTSRV);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateLUT(int _iWidth, int _iHeight) -> LUT SRV 생성 실패!"));
}

void Renderer::CreateUAV(int _iWidth, int _iHeight)
{
	D3D11_TEXTURE2D_DESC TexDesc = {};
	TexDesc.Width = _iWidth;          
	TexDesc.Height = _iHeight;        
	TexDesc.MipLevels = 1;                
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
	TexDesc.SampleDesc.Count = 1;          
	TexDesc.Usage = D3D11_USAGE_DEFAULT;   
	TexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;

	HRESULT hResult = DEVICE->CreateTexture2D(&TexDesc, nullptr, &m_pRWTex);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateUAV(int _iWidth, int _iHeight) -> RW Texture 생성 실패!"));

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.Format = TexDesc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0; 

	hResult = DEVICE->CreateUnorderedAccessView(m_pRWTex.Get(), &UAVDesc, &m_pRWUAV);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateUAV(int _iWidth, int _iHeight) -> UAV 생성 실패!"));

	hResult = DEVICE->CreateShaderResourceView(m_pRWTex.Get(), NULL, &m_pRWSRV);
	AssertEx(SUCCEEDED(hResult), _T("void Renderer::CreateUAV(int _iWidth, int _iHeight) -> SRV 생성 실패!"));
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

void Renderer::LoadSTL(const string& _strFilename)
{
	if (OpenMesh::IO::read_mesh(m_Mesh, _strFilename))
	{
		for (const auto& v : m_Mesh.vertices())
		{
			auto point = m_Mesh.point(v);
			Vec3 vertex{ point[0], point[1], point[2] };
			vertex.Normalize();
			m_vecMeshVertices.push_back(vertex);
		}

		for (const auto& f : m_Mesh.faces())
		{
			std::vector<uint32> vecFaceIndices;
			for (const auto& v : m_Mesh.fv_range(f))
			{
				vecFaceIndices.push_back(v.idx());
			}

			if (vecFaceIndices.size() == 3)
			{
				m_vecMeshIndices.push_back(vecFaceIndices[0]);
				m_vecMeshIndices.push_back(vecFaceIndices[1]);
				m_vecMeshIndices.push_back(vecFaceIndices[2]);
			}
		}

		// Create Vertex Buffer
		{
			uint32 count = static_cast<uint32>(m_vecMeshVertices.size());
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = sizeof(Vec3) * count;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA subData = {};
			subData.pSysMem = m_vecMeshVertices.data();

			HRESULT hr = DEVICE->CreateBuffer(&desc, &subData, &m_pMeshVB);
			AssertEx(SUCCEEDED(hr), L"Mesh::CreateVertexBuffer() Failed!");
		}

		// Create Index Buffer
		{
			uint32 indexCount = static_cast<uint32>(m_vecMeshIndices.size());
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = sizeof(uint32) * indexCount;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA subData = {};
			subData.pSysMem = m_vecMeshIndices.data();

			HRESULT hr = DEVICE->CreateBuffer(&desc, &subData, &m_pMeshIB);
			AssertEx(SUCCEEDED(hr), L"Mesh::CreateIndexBuffer() Failed!");
		}
	}
}

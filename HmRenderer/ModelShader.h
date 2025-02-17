#pragma once
#include "Shader.h"
#include "Common.h"

class ModelShader :
    public Shader
{
public:
    ModelShader();
    virtual ~ModelShader();

public:
    void Initialize(ComPtr<ID3D11Device> _pDevice);
    ComPtr<ID3D11Buffer> GetMatrixBuffer() { return m_pMatrixBuffer; }
   
private:
    void CreateVertexShader(ComPtr<ID3D11Device> _pDevice);
    void CreateInputLayout(ComPtr<ID3D11Device> _pDevice);
    void CreatePixelShader(ComPtr<ID3D11Device> _pDevice);
    void CreateConstantBuffer(ComPtr<ID3D11Device> _pDevice);


private:
    ComPtr<ID3D11Buffer> m_pMatrixBuffer;
    ComPtr<ID3DBlob> m_vsBlob;
    ComPtr<ID3DBlob> m_psBlob;
};


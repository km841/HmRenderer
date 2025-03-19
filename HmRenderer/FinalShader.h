#pragma once
#include "Shader.h"
class FinalShader :
    public Shader
{
public:
    FinalShader();
    virtual ~FinalShader();

public:
    void Initialize(ComPtr<ID3D11Device> _pDevice);

private:
    void CreateVertexShader(ComPtr<ID3D11Device> _pDevice);
    void CreateInputLayout(ComPtr<ID3D11Device> _pDevice);
    void CreatePixelShader(ComPtr<ID3D11Device> _pDevice);


private:
    ComPtr<ID3D11Buffer> m_pMatrixBuffer;
    ComPtr<ID3DBlob> m_vsBlob;
    ComPtr<ID3DBlob> m_psBlob;
};


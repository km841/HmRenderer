#pragma once
class GraphicsCore
{
public:
	GraphicsCore();
	~GraphicsCore();

public:
	void Initialize();
	void Update(float _fDeltaTime);

	void RenderBegin();
	void RenderEnd();
};


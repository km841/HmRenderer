#pragma once
class Renderer
{
public:
	Renderer();
	~Renderer();

public:
	void Initialize();
	void Update(float _fDeltaTime);
	void Render(float _fDeltaTime);

private:
	void CreateCube();



};


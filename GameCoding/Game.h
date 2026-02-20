#pragma once
#include "Graphics.h"
#include "GameObject.h" // юс╫ц
#include "Pipeline.h"

class SceneManager;

class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

	shared_ptr<SceneManager> GetSceneManager() { return _scene; }

	shared_ptr<Pipeline> GetPipeline() { return _pipeline; }

private:
	HWND _hwnd;

	shared_ptr<Graphics> _graphics;
	shared_ptr<Pipeline> _pipeline;

private:
	shared_ptr<SceneManager> _scene;
};

extern unique_ptr<Game> GGame;
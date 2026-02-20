#include "pch.h"
#include "Game.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "SceneManager.h"

unique_ptr<Game> GGame = make_unique<Game>();

Game::Game()
{
	
}

Game::~Game()
{

}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;

	_graphics = make_shared<Graphics>(_hwnd);
	_pipeline = make_shared<Pipeline>(_graphics->GetDeviceContext());

	_scene = make_shared<SceneManager>(_graphics);

	SCENE->LoadScene(L"Test");
}

void Game::Update()
{
	_graphics->RenderBegin();
	SCENE->Update();
	_graphics->RenderEnd();
}

void Game::Render()
{
	//_graphics->RenderBegin();

	// 오브젝트 그리기
	

	//_graphics->RenderEnd();
}

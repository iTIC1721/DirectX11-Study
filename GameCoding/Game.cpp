#include "pch.h"
#include "Game.h"

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

	// GameObject
	_gameObject = make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());
}

void Game::Update()
{
	// 오브젝트 Update
	_gameObject->Update();
}

void Game::Render()
{
	_graphics->RenderBegin();

	// 오브젝트 그리기
	{
		_gameObject->Render(_pipeline);
	}

	_graphics->RenderEnd();
}

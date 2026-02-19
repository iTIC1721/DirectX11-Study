#include "pch.h"
#include "Game.h"
#include "Camera.h"
#include "MeshRenderer.h"

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
	{
		_gameObject->GetOrAddTransform();
		_gameObject->AddComponent(make_shared<MeshRenderer>(_graphics->GetDevice(), _graphics->GetDeviceContext()));
		// ...
	}

	_camera = make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());
	{
		_camera->GetOrAddTransform();
		_camera->AddComponent(make_shared<Camera>());
	}
}

void Game::Update()
{
	// 오브젝트 Update
	_gameObject->Update();
	_camera->Update();
}

void Game::Render()
{
	_graphics->RenderBegin();

	// 오브젝트 그리기
	{
		//_gameObject->Render(_pipeline);
		// TEMP
		_gameObject->GetMeshRenderer()->Render(_pipeline);
	}

	_graphics->RenderEnd();
}

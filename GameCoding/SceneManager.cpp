#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "ResourceManager.h"
#include "Game.h"
#include "Mesh.h"

SceneManager::SceneManager(shared_ptr<Graphics> graphics)
	: _graphics(graphics)
{

}

void SceneManager::Init()
{
	if (_activeScene == nullptr) return;

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::Update()
{
	if (_activeScene == nullptr) return;

	_activeScene->Update();
	_activeScene->LateUpdate();

	_activeScene->FixedUpdate();
}

void SceneManager::LoadScene(wstring sceneName)
{
	_activeScene = LoadTestScene();

	Init();
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>();

	// Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());
		
		{
			camera->GetOrAddTransform();
			camera->AddComponent(make_shared<Camera>());
			
			scene->AddGameObject(camera);
		}
	}

	// GameObject
	{
		shared_ptr<GameObject> gameObject = make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());

		{
			gameObject->GetOrAddTransform();

			auto meshRenderer = make_shared<MeshRenderer>(_graphics->GetDevice(), _graphics->GetDeviceContext());
			gameObject->AddComponent(meshRenderer);

			auto mesh = RESOURCES->Get<Mesh>(L"Rectangle");
			meshRenderer->SetMesh(mesh);

			auto material = RESOURCES->Get<Material>(L"Default");
			meshRenderer->SetMaterial(material);

			scene->AddGameObject(gameObject);
		}
	}

	return scene;
}

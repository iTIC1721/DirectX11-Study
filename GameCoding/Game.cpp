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
	//_graphics = new Graphics(hwnd);
	_vertexBuffer = make_shared<VertexBuffer>(_graphics->GetDevice());
	_indexBuffer = make_shared<IndexBuffer>(_graphics->GetDevice());
	_inputLayout = make_shared<InputLayout>(_graphics->GetDevice());

	CreateGeometry();
	CreateVS();
	CreateInputLayout();
	CreatePS();

	CreateRasterizerState();
	CreateSamplerState();
	CreateBlendState();

	CreateSRV();
	CreateConstantBuffer();
}

void Game::Update()
{
	// SRT
	_localPosition.x += 0.001f;

	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	Matrix matWorld = matScale * matRotation * matTranslation;
	_transformData.matWorld = matWorld;
	
	// TransformData를 넣기
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));
	
	_graphics->GetDeviceContext()->Map(
		_constantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subResource
	);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_graphics->GetDeviceContext()->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	_graphics->RenderBegin();

	// 오브젝트 그리기
	{
		// IA
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;

		auto _deviceContext = _graphics->GetDeviceContext();

		_deviceContext->IASetVertexBuffers(
			0,
			1,
			_vertexBuffer->GetComPtr().GetAddressOf(),
			&stride,
			&offset
		);
		_deviceContext->IASetIndexBuffer(
			_indexBuffer->GetComPtr().Get(),
			DXGI_FORMAT_R32_UINT,
			0
		);
		_deviceContext->IASetInputLayout(_inputLayout->GetComPtr().Get());
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// VS
		_deviceContext->VSSetShader(
			_vertexShader.Get(),
			nullptr,
			0
		);
		_deviceContext->VSSetConstantBuffers(
			0, 
			1, 
			_constantBuffer.GetAddressOf()
		);

		// RS
		// 코딩은 불가능하지만 여러 설정이 가능
		_deviceContext->RSSetState(_rasterizerState.Get());

		// PS
		_deviceContext->PSSetShader(
			_pixelShader.Get(),
			nullptr,
			0
		);
		_deviceContext->PSSetShaderResources(
			0, 
			1,
			_shaderResourceView.GetAddressOf()
		);
		_deviceContext->PSSetSamplers(
			0, 
			1, 
			_samplerState.GetAddressOf()
		);

		// OM
		_deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);

		_deviceContext->DrawIndexed(_indices.size(), 0, 0);
	}

	_graphics->RenderEnd();
}

void Game::CreateGeometry()
{
	// Vertex Data
	{
		_vertices.resize(4);

		// 13
		// 02
		_vertices[0].position = Vec3(-0.5f, -0.5f, 0);
		_vertices[0].uv = Vec2(0.f, 1.f);
		//_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0);
		_vertices[1].uv = Vec2(0.f, 0.f);
		//_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0);
		_vertices[2].uv = Vec2(1.f, 1.f);
		//_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);
		
		_vertices[3].position = Vec3(0.5f, 0.5f, 0);
		_vertices[3].uv = Vec2(1.f, 0.f);
		//_vertices[3].color = Color(1.f, 1.f, 1.f, 1.f);
	}

	// Vertex Buffer
	{
		_vertexBuffer->Create(_vertices);
	}

	// Index Data
	{
		_indices = { 0, 1, 2, 2, 1, 3 };
	}

	// Index Buffer
	{
		_indexBuffer->Create(_indices);
	}
}

void Game::CreateInputLayout()
{
	vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	_inputLayout->Create(layout, _vsBlob);
}

void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob);

	HRESULT hr = _graphics->GetDevice()->CreateVertexShader(
		_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(),
		nullptr,
		_vertexShader.GetAddressOf()
	);

	CHECK(hr);
}

void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob);

	HRESULT hr = _graphics->GetDevice()->CreatePixelShader(
		_psBlob->GetBufferPointer(),
		_psBlob->GetBufferSize(),
		nullptr,
		_pixelShader.GetAddressOf()
	);

	CHECK(hr);
}

void Game::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;

	HRESULT hr = _graphics->GetDevice()->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
	CHECK(hr);
}

void Game::CreateSamplerState()
{
	// 셰이더에서 텍스쳐 uv좌표 색상을 꺼낼 때와 관련
	// 크기를 벗어났을 때
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;

	HRESULT hr = _graphics->GetDevice()->CreateSamplerState(&desc, _samplerState.GetAddressOf());
	CHECK(hr);
}

void Game::CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = _graphics->GetDevice()->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hr);
}

void Game::CreateSRV()
{
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;

	HRESULT hr = ::LoadFromWICFile(L"miku.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	// 셰이더 리소스 뷰 생성
	hr = ::CreateShaderResourceView(
		_graphics->GetDevice().Get(),
		img.GetImages(), 
		img.GetImageCount(), 
		md,
		_shaderResourceView.GetAddressOf()
	);
	CHECK(hr);
}

void Game::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;	// CPU write + GPU read
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(TransformData);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = _graphics->GetDevice()->CreateBuffer(
		&desc, 
		nullptr, 
		_constantBuffer.GetAddressOf()
	);
	CHECK(hr);
}

void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 comfileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		comfileFlag,
		0,
		blob.GetAddressOf(),
		nullptr
	);

	CHECK(hr);
}

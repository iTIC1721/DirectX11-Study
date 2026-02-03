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
	_width = GWinSizeX;
	_height = GWinSizeY;

	// TODO
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewport();

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
	//_transformData.offset.x += 0.0003f;
	//_transformData.offset.y += 0.0003f;

	// TransformData를 넣기
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));
	
	_deviceContext->Map(
		_constantBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subResource
	);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_deviceContext->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	RenderBegin();

	// 삼각형 그리기
	{
		// IA
		uint32 stride = sizeof(Vertex);
		uint32 offset = 0;
		_deviceContext->IASetVertexBuffers(
			0,
			1,
			_vertexBuffer.GetAddressOf(),
			&stride,
			&offset
		);
		_deviceContext->IASetIndexBuffer(
			_indexBuffer.Get(),
			DXGI_FORMAT_R32_UINT,
			0
		);
		_deviceContext->IASetInputLayout(_inputLayout.Get());
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

	RenderEnd();
}

void Game::RenderBegin()
{
	// 그림을 그릴 타겟 뷰 선택
	_deviceContext->OMSetRenderTargets(
		1, 
		_renderTargetView.GetAddressOf(), 
		nullptr
	);

	// 타겟 뷰의 색 초기화
	_deviceContext->ClearRenderTargetView(
		_renderTargetView.Get(),
		_clearColor
	);

	// 뷰 포트 설정
	_deviceContext->RSSetViewports(1, &_viewport);
}

void Game::RenderEnd()
{
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}

void Game::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));	// desc 안의 내용을 0으로 초기화
	{
		desc.BufferDesc.Width = _width;
		desc.BufferDesc.Height = _height;
		desc.BufferDesc.RefreshRate.Numerator = 60;	// 주사율과 관련
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;	// 멀티샘플링 숫자와 관련
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 최종적으로 화면에 그리는 용도로 사용하겠음
		desc.BufferCount = 1;
		desc.OutputWindow = _hwnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}

void Game::CreateRenderTargetView()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(
		0, 
		__uuidof(ID3D11Texture2D), 
		(void**)backBuffer.GetAddressOf()
	);
	CHECK(hr);

	hr = _device->CreateRenderTargetView(
		backBuffer.Get(), 
		nullptr, 
		_renderTargetView.GetAddressOf()
	);
	CHECK(hr);
}

void Game::SetViewport()
{
	_viewport.TopLeftX = 0.f;
	_viewport.TopLeftY = 0.f;
	_viewport.Width = static_cast<float>(_width);
	_viewport.Height = static_cast<float>(_height);
	_viewport.MinDepth = 0.f;
	_viewport.MaxDepth = 1.f;
}

void Game::CreateGeometry()
{
	// Vertex Data
	{
		_vertices.resize(4);

		// 13
		// 02
		_vertices[0].position = Vec3(-0.5f, -0.5f, 0);
		_vertices[0].uv = Vec2(0.f, 5.f);
		//_vertices[0].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0);
		_vertices[1].uv = Vec2(0.f, 0.f);
		//_vertices[1].color = Color(0.f, 1.f, 0.f, 1.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0);
		_vertices[2].uv = Vec2(5.f, 5.f);
		//_vertices[2].color = Color(0.f, 0.f, 1.f, 1.f);
		
		_vertices[3].position = Vec3(0.5f, 0.5f, 0);
		_vertices[3].uv = Vec2(5.f, 0.f);
		//_vertices[3].color = Color(1.f, 1.f, 1.f, 1.f);
	}

	// Vertex Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// Immutable: GPU에 의해서 읽을수만 있음 (CPU는 접근 불가)
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// -> Vertex Buffer를 만드는 데에 사용하겠다
		desc.ByteWidth = (uint32)(sizeof(Vertex) * _vertices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _vertices.data();	// 첫 번째 데이터의 메모리상 위치

		// 버퍼 생성
		HRESULT hr = _device->CreateBuffer(
			&desc, 
			&data, 
			_vertexBuffer.GetAddressOf()
		);

		CHECK(hr);
	}

	// Index Data
	{
		_indices = { 0, 1, 2, 2, 1, 3 };
	}

	// Index Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// Immutable: GPU에 의해서 읽을수만 있음 (CPU는 접근 불가)
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// -> Vertex Buffer를 만드는 데에 사용하겠다
		desc.ByteWidth = (uint32)(sizeof(uint32) * _indices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _indices.data();	// 첫 번째 데이터의 메모리상 위치

		// 버퍼 생성
		HRESULT hr = _device->CreateBuffer(
			&desc,
			&data,
			_indexBuffer.GetAddressOf()
		);

		CHECK(hr);
	}
}

void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	const int32 numElement = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	_device->CreateInputLayout(
		layout,
		numElement,
		_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(),
		_inputLayout.GetAddressOf()
	);
}

void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob);

	HRESULT hr = _device->CreateVertexShader(
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

	HRESULT hr = _device->CreatePixelShader(
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

	HRESULT hr = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
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

	HRESULT hr = _device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
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

	HRESULT hr = _device->CreateBlendState(&desc, _blendState.GetAddressOf());
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
		_device.Get(), 
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

	HRESULT hr = _device->CreateBuffer(
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

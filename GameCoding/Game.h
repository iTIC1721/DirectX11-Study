#pragma once
class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private:
	void RenderBegin();
	void RenderEnd();

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();

private:
	void CreateGeometry();
	void CreateInputLayout();	// Geometry를 어떻게 해석해야 하는지
	void CreateVS();
	void CreatePS();

	void CreateRasterizerState();
	void CreateSamplerState();
	void CreateBlendState();
	void CreateSRV();	// Shader Resource View

	void CreateConstantBuffer();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;

private:
	// DX
	// vv GPU 모델에 상관없이 작동하게 하는 인터페이스들 vv
	// ComPtr: 메모리 관리를 안전하고 편하게 하기 위해 내부적으로 자동으로 관리해줌
	ComPtr<ID3D11Device> _device = nullptr;	// 어떤 리소스를 만들 때 device를 통해 만듦
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;	// 이미 만들어진 리소스에 명령/행동을 시킴
	ComPtr<IDXGISwapChain> _swapChain = nullptr;	// DXGI: 그래픽 분야 중 느리게 발전하는 부분(ex. swapchain)을 위해 라이브러리를 분리함
	
	// RTV
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// Misc
	D3D11_VIEWPORT _viewport = { 0 };
	float _clearColor[4] = { 0.f, 0.f, 0.f, 0.f };

private:
	// Geometry
	vector<Vertex> _vertices;
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;

	vector<uint32> _indices;
	ComPtr<ID3D11Buffer> _indexBuffer = nullptr;

	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// VS
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
	ComPtr<ID3DBlob> _vsBlob = nullptr;

	// RS
	ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;

	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	// SRV
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;

	ComPtr<ID3D11SamplerState> _samplerState = nullptr;
	ComPtr<ID3D11BlendState> _blendState = nullptr;

private:
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;
};


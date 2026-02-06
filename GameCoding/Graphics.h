#pragma once

class Graphics
{
public:
	Graphics(HWND hwnd);
	~Graphics();

	void RenderBegin();
	void RenderEnd();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();

private:
	HWND _hwnd;
	uint32 _width = GWinSizeX;
	uint32 _height = GWinSizeY;

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
};


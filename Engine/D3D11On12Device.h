#pragma once


class D3D11On12Device
{
public:
	D3D11On12Device();
	~D3D11On12Device();

	void Init(ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, vector<ComPtr<ID3D12Resource>> swapchains, ComPtr<ID3D12CommandQueue> cmdQueue);

	ComPtr<ID3D11DeviceContext> GetD3D11DeviceContext() { return _d3d11DeviceContext; }
	ComPtr<ID3D11On12Device> GetD3D11on12Device() { return _d3d11On12Device; }
	ComPtr<ID2D1Factory3> GetD2DFactory() { return _d2dFactory; }
	ComPtr<ID2D1Device2> GetD2DDevice() { return _d2dDevice; }
	ComPtr<ID2D1DeviceContext2>	GetD2DDeviceContext() { return _d2dDeviceContext; }
	ComPtr<ID2D1Bitmap1> GetD3D11On12RT(uint8 index) { return _d2dRenderTargets[index]; }
	ComPtr<ID3D11Resource> GetWrappedBackBuffer(uint8 index) { return _wrappedBackBuffers[index]; }
	ComPtr<IDWriteFactory> GetDWriteFactory() { return _dWriteFactory; }

	ComPtr<ID2D1SolidColorBrush> GetSolidColorBrush() { return _textBrush; }
	ComPtr<IDWriteTextFormat> GetTextFormat() { return _textFormat; }


private:
	static const UINT FrameCount = 2;

	ComPtr<ID3D11DeviceContext> _d3d11DeviceContext;
	ComPtr<ID3D11On12Device>	_d3d11On12Device;
	ComPtr<IDWriteFactory>		_dWriteFactory;
	ComPtr<ID2D1Factory3>		_d2dFactory;
	ComPtr<ID2D1Device2>		_d2dDevice;
	ComPtr<ID2D1DeviceContext2>	_d2dDeviceContext;
	ComPtr<ID2D1Bitmap1>		_d2dRenderTargets[FrameCount];
	ComPtr<ID3D11Resource>		_wrappedBackBuffers[FrameCount];

	// ¸®¼Ò½º
	ComPtr<ID2D1SolidColorBrush> _textBrush;
	ComPtr<IDWriteTextFormat> _textFormat;
};


#include "pch.h"
#include "D3D11On12Device.h"

D3D11On12Device::D3D11On12Device()
{
}

D3D11On12Device::~D3D11On12Device()
{

}

void D3D11On12Device::Init(ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, vector<ComPtr<ID3D12Resource>> swapchains, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	ComPtr<ID3D11Device> d3d11Device;
	D3D11On12CreateDevice(device.Get(), D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
		reinterpret_cast<IUnknown**>(cmdQueue.GetAddressOf()), 1, 0, &d3d11Device, &_d3d11DeviceContext, nullptr);
	d3d11Device.As(&_d3d11On12Device);

	// D2D/Dwrite 컴포넌트 생성
	{
		D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &_d2dFactory);

		ComPtr<IDXGIDevice> dxgiDevice;
		_d3d11On12Device.As(&dxgiDevice);
		_d2dFactory->CreateDevice(dxgiDevice.Get(), &_d2dDevice);
		_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_d2dDeviceContext);
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_dWriteFactory);
	}

	float dpiX;
	float dpiY;

#pragma warning(push)
#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
	_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY
	);

	for (int i = 0; i < swapchains.size(); ++i)
	{
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		_d3d11On12Device->CreateWrappedResource(
			swapchains[i].Get(),
			&d3d11Flags,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT,
			IID_PPV_ARGS(&_wrappedBackBuffers[i])
		);

		ComPtr<IDXGISurface> surface;
		_wrappedBackBuffers[i].As(&surface);
		_d2dDeviceContext->CreateBitmapFromDxgiSurface(
			surface.Get(),
			&bitmapProperties,
			&_d2dRenderTargets[i]
		);
	}

	// Create D2D/DWrite objects for rendering text.
	{
		_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &_textBrush);
		_dWriteFactory->CreateTextFormat(
			L"Verdana",
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			30,
			L"ko-kr",
			&_textFormat
		);
		_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}
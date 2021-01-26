// This is the main DLL file.

#include "stdafx.h"

#include "D3D11Scene.h"

namespace D3D11Scene {

	IntPtr D3D11TestScene::GetRenderTarget()
	{
		return IntPtr(sharedTexture);
	}	

	HRESULT D3D11TestScene::PrepareSharedTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, UINT miscFlags)
	{
		D3D11_TEXTURE2D_DESC textureDesc = { 0, };
		HRESULT hr;

		/* Texture size doesn't need to be identical to that of backbuffer */
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags =
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.MiscFlags = miscFlags;

		ID3D11Texture2D* sharedTexture1;
		hr = device->CreateTexture2D(&textureDesc, nullptr, &sharedTexture1);
		if (FAILED(hr)) {
			printf("Couldn't create ID3D11Texture2D");
			return hr;
		}

		IDXGIKeyedMutex* keyed;
		if ((miscFlags & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX) != 0 && keyedMutex) {
			hr = sharedTexture1->QueryInterface(_uuidof(IDXGIKeyedMutex), reinterpret_cast<void**>(&keyed)); //??
			if (FAILED(hr)) {
				printf("Couldn't get IDXGIKeyedMutex");
				return hr;
			}
		}

		IDXGIResource* dxgiResource;
		hr = sharedTexture1->QueryInterface(_uuidof(IDXGIResource), reinterpret_cast<void**>(&dxgiResource)); //??
		if (FAILED(hr)) {
			printf("Couldn't get IDXGIResource handle");
			return hr;
		}

		HANDLE handle;
		if ((miscFlags & D3D11_RESOURCE_MISC_SHARED_NTHANDLE) != 0) {
			IDXGIResource1* dxgiResource1;
			hr = dxgiResource->QueryInterface(_uuidof(IDXGIResource1), reinterpret_cast<void**>(&dxgiResource1)); //??

			if (FAILED(hr)) {
				printf("Couldn't get IDXGIResource1");
				return hr;
			}

			hr = dxgiResource1->CreateSharedHandle(nullptr,
				DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, nullptr,
				&handle);
		}
		else {
			hr = dxgiResource->GetSharedHandle(&handle);
		}

		if (FAILED(hr)) {
			printf("Couldn't get shared handle from texture");
			return hr;
		}

		sharedTexture = sharedTexture1;

		if (keyed && keyedMutex)
			keyedMutex = keyed;

		return S_OK;
	}
	HRESULT D3D11TestScene::PrepareD3d11Device()
	{
		HRESULT hr;
		static const D3D_FEATURE_LEVEL feature_levels[] = {
		  D3D_FEATURE_LEVEL_11_1,
		  D3D_FEATURE_LEVEL_11_0,
		  D3D_FEATURE_LEVEL_10_1,
		  D3D_FEATURE_LEVEL_10_0,
		  D3D_FEATURE_LEVEL_9_3,
		  D3D_FEATURE_LEVEL_9_2,
		  D3D_FEATURE_LEVEL_9_1
		};
		D3D_FEATURE_LEVEL selectedLevel;

		IDXGIFactory1* factory1;
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory1));
		if (FAILED(hr)) {
			printf("IDXGIFactory1 is unavailable, hr 0x%x", (unsigned int)hr);
			return hr;
		}

		IDXGIFactory2* factory2;
		hr = factory1->QueryInterface(_uuidof(IDXGIFactory2), reinterpret_cast<void**>(&factory2)); //??
		if (FAILED(hr)) {
			printf("IDXGIFactory2 is unavailable, hr 0x%x", (unsigned int)hr);
			return hr;
		}

		IDXGIAdapter1* adapter;
		hr = factory1->EnumAdapters1(0, &adapter);
		if (FAILED(hr)) {
			printf("IDXGIAdapter1 is unavailable, hr 0x%x", (unsigned int)hr);
			return hr;
		}
		ID3D11Device* device1 = device;
		ID3D11DeviceContext* deviceContext = context;
		hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN,
			NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, feature_levels,
			G_N_ELEMENTS(feature_levels), D3D11_SDK_VERSION, &device1,
			&selectedLevel, &deviceContext);

		/* Try again with excluding D3D_FEATURE_LEVEL_11_1 */
		if (FAILED(hr)) {
			hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN,
				NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, &feature_levels[1],
				G_N_ELEMENTS(feature_levels) - 1, D3D11_SDK_VERSION, &device1,
				&selectedLevel, &deviceContext);
		}

		if (FAILED(hr)) {
			printf("ID3D11Device is unavailable, hr 0x%x", (unsigned int)hr);
			return hr;
		}

		device = device1;
		context = deviceContext;
		factory = factory2;
		return hr;
	}	
}

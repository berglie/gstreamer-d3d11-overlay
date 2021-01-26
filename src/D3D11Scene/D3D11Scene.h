// D3D11Scene.h
#pragma once
#include <windows.h>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dxgi.lib")
#pragma  comment(lib, "d3d11.lib")
#pragma  comment(lib, "d3dcompiler.lib")
#pragma  comment(lib, "winmm.lib")


// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>

#include <d3dcompiler.h>
#include <D3D11.h>
#include <dxgi1_2.h>
using namespace System;
#define TEXTURE_PATH ".\\DirectX.bmp"
#define EFFECT_PATH ".\\TextureMap.fx"
#define G_N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))
#define CHECK_STR_NULL(str) ((str) ? (str) : "(NULL)")

namespace D3D11Scene {
	public ref class D3D11TestScene
	{
	public:		
		D3D11TestScene()
		{		
			HRESULT hr;
			ID3D11SamplerState* sampler;
			ID3D11PixelShader* ps;
			ID3D11VertexShader* vs;
			ID3D11InputLayout* layout;
			ID3D11Buffer* vertex;
			ID3D11Buffer* index;

			miscFlags = D3D11_RESOURCE_MISC_SHARED;
			format = DXGI_FORMAT_B8G8R8A8_UNORM;

			sharedTexture = NULL;

			hr = PrepareD3d11Device();
			hr = PrepareSharedTexture(width, height,format, miscFlags);		
		}	
		
		IntPtr GetRenderTarget();

	private:
		ID3D11Device*              device;
		ID3D11Texture2D*			sharedTexture;
		IDXGIFactory2* factory;
		ID3D11DeviceContext* context;
		IDXGIKeyedMutex* keyedMutex;
		DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
		UINT miscFlags = 0;
		UINT width = 640;
		UINT height = 480;
		static char* textureFormat = nullptr;

	private:		
		HRESULT PrepareSharedTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, UINT miscFlags);
		HRESULT PrepareD3d11Device();
	};
}

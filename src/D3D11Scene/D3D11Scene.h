// D3D11Scene.h
#pragma once
#include <windows.h>
#pragma  comment(lib, "d3d11.lib")

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <d3d11.h>
#include <dxgi1_2.h>

using namespace System;
#define G_N_ELEMENTS(arr)		(sizeof (arr) / sizeof ((arr)[0]))

namespace D3D11Scene {
	public ref class D3D11TestScene
	{
	public:		
		D3D11TestScene();
		HANDLE sharedHandle;
		IntPtr GetSharedHandle();
		IntPtr GetRenderTarget();

	private:
		ID3D11Device*              device;
		ID3D11Texture2D*			sharedTexture;
		IDXGIFactory2* factory;
		ID3D11DeviceContext* context;
		IDXGIKeyedMutex* keyedMutex;
		DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
		UINT miscFlags = 0;
		UINT width = 1280;
		UINT height = 720;
		static char* textureFormat = nullptr;

	private:		
		HRESULT PrepareSharedTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, UINT miscFlags);
		HRESULT PrepareD3d11Device();
	};
}

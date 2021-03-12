// D3D11Scene.h
#pragma once
#include <windows.h>
#pragma  comment(lib, "d3d11.lib")
#pragma  comment(lib, "dxgi.lib")

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
		D3D11TestScene(unsigned int width, unsigned int height);
		~D3D11TestScene() { Release(); }
		HANDLE sharedHandle;
		IntPtr GetSharedHandle();
		IntPtr GetRenderTarget();
		void Release();
	private:
		ID3D11Device* _device;
		ID3D11Texture2D* _sharedTexture;
		IDXGIFactory2* _factory;
		ID3D11DeviceContext* _context;
		IDXGIKeyedMutex* _keyedMutex;
		DXGI_FORMAT _format = DXGI_FORMAT_B8G8R8A8_UNORM;
		UINT _miscFlags = 0;
		static char* _textureFormat = nullptr;

	private:		
		HRESULT PrepareSharedTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, UINT miscFlags);
		HRESULT PrepareD3d11Device();
	};
}

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

//struct SimpleVertex
//{
//    D3DXVECTOR3 Pos;
//    D3DXVECTOR2 Tex;
//};
//
//SimpleVertex VERTEX_DATA[] =
//{
//    { D3DXVECTOR3(  0.5f,   0.5f, 0), D3DXVECTOR2(1.0f, 0.0f) },
//    { D3DXVECTOR3(  0.5f,  -0.5f, 0), D3DXVECTOR2(1.0f, 1.0f) },
//    { D3DXVECTOR3( -0.5f,  -0.5f, 0), D3DXVECTOR2(0.0f, 1.0f) },
//	{ D3DXVECTOR3( -0.5f,   0.5f, 0), D3DXVECTOR2(0.0f, 0.0f) },
//};

//USHORT INDEX_DATA[] =
//{
//    0, 1, 2, 0, 2, 3
//};
//
//float ClearColor[4] = { 0, 0, 0, 0};

namespace D3D11Scene {
	// REVIEW: this Vertext data is not needed
	typedef struct
	{
		struct {
			FLOAT x;
			FLOAT y;
			FLOAT z;
		} position;
		struct {
			FLOAT x;
			FLOAT y;
		} texture;
	} VertexData;

	public ref class D3D11TestScene
	{
	public:
		HANDLE sharedHandle;
		D3D11TestScene()
		{
			HRESULT hr;
			ID3D11SamplerState* sampler;
			ID3D11PixelShader* ps;
			ID3D11VertexShader* vs;
			ID3D11InputLayout* layout;
			ID3D11Buffer* vertex;
			ID3D11Buffer* index;

			SetFlags();
			SetFormat();

			sharedTexture = NULL;
			renderTargetView = NULL;

			hr = PrepareD3d11Device();
			OnResize();
			hr = PrepareShader(&sampler, &ps, &vs, &layout, &vertex, &index);
			hr = PrepareSharedTexture(width, height,format, miscFlags);
			CreateGeometry(sampler, ps, vs, layout, vertex, index);

		}

		void SetFlags();
		void SetFormat();
		// REVIEW: HANDLE should be IntPtr, not Int32
		Int32 GetSharedHandle();
		IntPtr GetRenderTarget();
		void Render();
		void OnResize();

	private:
		ID3D11Device*              device;
		ID3D11Texture2D*			sharedTexture;
		// REVIEW: You don't need ID3D11RenderTargetView
		ID3D11RenderTargetView*		renderTargetView;
		IDXGIFactory2* factory;
		ID3D11DeviceContext* context;
		IDXGIKeyedMutex* keyedMutex;
		DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
		UINT miscFlags = 0;
		UINT width = 640;
		UINT height = 480;
		static char* textureFormat = nullptr;
		// REVIEW: For D3D9 interop, NT handle + keyed-mutex combination is not need
		// and not supported
		static bool useNtHandle = FALSE;
		static bool useKeyedMutex = FALSE;

	private:

		HRESULT	PrepareShader(ID3D11SamplerState** sampler, ID3D11PixelShader** ps,
			ID3D11VertexShader** vs, ID3D11InputLayout** layout,
			ID3D11Buffer** vertex, ID3D11Buffer** index);
		HRESULT PrepareSharedTexture(unsigned int width, unsigned int height, DXGI_FORMAT format, UINT miscFlags);
		HRESULT PrepareD3d11Device();
		HRESULT CompileD3D(const char* source, bool isPixelShader, ID3DBlob** code);
		void CreateGeometry(ID3D11SamplerState* sampler, ID3D11PixelShader* ps,
			ID3D11VertexShader* vs, ID3D11InputLayout* layout,
			ID3D11Buffer* vertex, ID3D11Buffer* index);
	};
}

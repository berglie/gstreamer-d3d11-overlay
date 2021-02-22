#pragma once

#pragma  comment(lib, "d3d9.lib")

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>

#include "d3d9.h"
#include "D3D11_1.h"
#include <D3D11.h>

using namespace System;
using namespace System::Windows::Interop;
using namespace System::Windows;

namespace System { namespace Windows { namespace Interop
{
	public enum class D3DResourceTypeEx : int
	{
		ID3D10Texture2D = 0,
		ID3D11Texture2D = 1
	};

	public ref class D3DImageEx : D3DImage
	{
		static D3DImageEx();		
	public:
		IntPtr CreateBackBuffer(D3DResourceTypeEx resourceType, IntPtr pResource);
		IntPtr GetBackbuffer();
	private:
		static IDirect3D9Ex*               m_D3D9;
		static IDirect3DDevice9Ex*         m_D3D9Device;
		static IntPtr m_backBuffer;
		static D3DFORMAT ConvertDXGIToD3D9Format(DXGI_FORMAT format);
		static HRESULT GetSharedSurface(HANDLE hSharedHandle, void** ppUnknown, UINT width, UINT height, DXGI_FORMAT format);
		static HRESULT GetSharedHandle(IUnknown *pUnknown, HANDLE * pHandle);
		static HRESULT InitD3D9(HWND hWnd);
	};
}}}

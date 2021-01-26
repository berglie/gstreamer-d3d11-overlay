// This is the main DLL file.

#include "stdafx.h"

#include "D3D11Scene.h"

namespace D3D11Scene {

	void D3D11TestScene::SetFlags()
	{
		/* NT handle needs to be used with keyed mutex */
		if (useNtHandle) {
			miscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX | D3D11_RESOURCE_MISC_SHARED_NTHANDLE;
		}
		else if (useKeyedMutex) {
			miscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
		}
		else {
			miscFlags = D3D11_RESOURCE_MISC_SHARED;
		}
	}

	void D3D11TestScene::SetFormat()
	{
		if (textureFormat != "RGBA") {
			printf("Use DXGI_FORMAT_R8G8B8A8_UNORM (RGBA) format");
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		else if (textureFormat != "RGB10A2_LE") {
			printf("Use DXGI_FORMAT_R10G10B10A2_UNORM (RGB10A2_LE) format");
			format = DXGI_FORMAT_R10G10B10A2_UNORM;
		}
		else {
			printf("Use DXGI_FORMAT_B8G8R8A8_UNORM format");
			format = DXGI_FORMAT_B8G8R8A8_UNORM;
		}
	}

	// REVIEW: IntPtr
	Int32 D3D11TestScene::GetSharedHandle()
	{
		Int32 handle = (Int32)sharedHandle;
		return handle;
	}

	IntPtr D3D11TestScene::GetRenderTarget()
	{
		return IntPtr(sharedTexture);
	}

	// REVIEW: This method is not needed
	void D3D11TestScene::Render()
	{
		ID3D11RenderTargetView* pRenderTargetView;
		HRESULT hr = S_OK;

		hr = device->CreateRenderTargetView(sharedTexture, NULL, &pRenderTargetView);
		context->OMSetRenderTargets(1, &pRenderTargetView, NULL);
		renderTargetView = pRenderTargetView;

		context->DrawIndexed(6, 0, 0);

		context->OMSetRenderTargets(0, 0, 0);
		if (renderTargetView)
			renderTargetView->Release();

		context->Flush();
	}

	// REVIEW: This method is not needed
	void D3D11TestScene::OnResize()
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		context->RSSetViewports(1, &viewport);
	}

	// REVIEW: This method is not needed
	HRESULT	D3D11TestScene::PrepareShader(ID3D11SamplerState** sampler, ID3D11PixelShader** ps,
		ID3D11VertexShader** vs, ID3D11InputLayout** layout,
		ID3D11Buffer** vertex, ID3D11Buffer** index)
	{
		static const char psCode[] =
			"Texture2D shaderTexture;\n"
			"SamplerState samplerState;\n"
			"\n"
			"struct PS_INPUT\n"
			"{\n"
			"  float4 Position: SV_POSITION;\n"
			"  float3 Texture: TEXCOORD0;\n"
			"};\n"
			"\n"
			"struct PS_OUTPUT\n"
			"{\n"
			"  float4 Plane: SV_Target;\n"
			"};\n"
			"\n"
			"PS_OUTPUT main(PS_INPUT input)\n"
			"{\n"
			"  PS_OUTPUT output;\n"
			"  output.Plane = shaderTexture.Sample(samplerState, input.Texture);\n"
			"  return output;\n"
			"}\n";

		static const char vsCode[] =
			"struct VS_INPUT\n"
			"{\n"
			"  float4 Position : POSITION;\n"
			"  float4 Texture : TEXCOORD0;\n"
			"};\n"
			"\n"
			"struct VS_OUTPUT\n"
			"{\n"
			"  float4 Position: SV_POSITION;\n"
			"  float4 Texture: TEXCOORD0;\n"
			"};\n"
			"\n"
			"VS_OUTPUT main(VS_INPUT input)\n"
			"{\n"
			"  return input;\n"
			"}\n";

		D3D11_SAMPLER_DESC samplerDesc = {  };
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ID3D11SamplerState* sampler_state;
		HRESULT hr = device->CreateSamplerState(&samplerDesc, &sampler_state);
		if (FAILED(hr)) {
			printf("Couldn't create ID3D11SamplerState");
			return hr;
		}

		ID3DBlob* code;
		hr = CompileD3D(psCode, TRUE, &code);
		if (FAILED(hr))
			return hr;

		ID3D11PixelShader* pixelShader;
		hr = device->CreatePixelShader(code->GetBufferPointer(),
			code->GetBufferSize(), nullptr, &pixelShader);
		if (FAILED(hr)) {
			printf("Couldn't create ID3D11PixelShader");
			return hr;
		}

		hr = CompileD3D(vsCode, FALSE, &code); //code.ReleaseAndGetAddressOf()
		if (FAILED(hr))
			return hr;

		ID3D11VertexShader* vertexShader;
		hr = device->CreateVertexShader(code->GetBufferPointer(),
			code->GetBufferSize(), nullptr, &vertexShader);
		if (FAILED(hr)) {
			printf("Couldn't create ID3D11VertexShader");
			return hr;
		}

		D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3D11InputLayout* inputLayout;
		hr = device->CreateInputLayout(input_desc, G_N_ELEMENTS(input_desc),
			code->GetBufferPointer(), code->GetBufferSize(), &inputLayout);
		if (FAILED(hr)) {
			printf("Couldn't create ID3D11InputLayout");
			return hr;
		}

		D3D11_BUFFER_DESC bufferDesc = { 0, };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(VertexData) * 4;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ID3D11Buffer* vertexBuffer;
		hr = device->CreateBuffer(&bufferDesc, nullptr, &vertexBuffer);
		if (FAILED(hr)) {
			printf("Couldn't create ID3D11Buffer for vertex buffer");
			return hr;
		}

		D3D11_MAPPED_SUBRESOURCE map;
		hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		if (FAILED(hr)) {
			printf("Couldn't map vertex buffer");
			return hr;
		}

		VertexData* vertexData = (VertexData*)map.pData;
		vertexData[0].position.x = -1.0f;
		vertexData[0].position.y = -1.0f;
		vertexData[0].position.z = 0.0f;
		vertexData[0].texture.x = 0.0f;
		vertexData[0].texture.y = 1.0f;

		vertexData[1].position.x = -1.0f;
		vertexData[1].position.y = 1.0f;
		vertexData[1].position.z = 0.0f;
		vertexData[1].texture.x = 0.0f;
		vertexData[1].texture.y = 0.0f;

		vertexData[2].position.x = 1.0f;
		vertexData[2].position.y = 1.0f;
		vertexData[2].position.z = 0.0f;
		vertexData[2].texture.x = 1.0f;
		vertexData[2].texture.y = 0.0f;

		vertexData[3].position.x = 1.0f;
		vertexData[3].position.y = -1.0f;
		vertexData[3].position.z = 0.0f;
		vertexData[3].texture.x = 1.0f;
		vertexData[3].texture.y = 1.0f;

		context->Unmap(vertexBuffer, 0);

		ID3D11Buffer* indexBuffer;
		bufferDesc.ByteWidth = sizeof(WORD) * 2 * 3;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		hr = device->CreateBuffer(&bufferDesc, nullptr, &indexBuffer);
		if (FAILED(hr)) {
			printf("Couldn't create ID3D11Buffer for index buffer");
			return hr;
		}

		hr = context->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
		if (FAILED(hr)) {
			printf("Couldn't map index buffer");
			return hr;
		}

		WORD* indices = (WORD*)map.pData;
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 3;
		indices[4] = 0;
		indices[5] = 2;

		context->Unmap(indexBuffer, 0);

		*sampler = sampler_state;
		*ps = pixelShader;
		*vs = vertexShader;
		*layout = inputLayout;
		*vertex = vertexBuffer;
		*index = indexBuffer;

		return S_OK;
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

		sharedHandle = handle;
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

	// REVIEW: This method is not needed
	void D3D11TestScene::CreateGeometry(ID3D11SamplerState* sampler, ID3D11PixelShader* ps,
		ID3D11VertexShader* vs, ID3D11InputLayout* layout,
		ID3D11Buffer* vertex, ID3D11Buffer* index)
	{
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(layout);
		ID3D11Buffer* buf = vertex;
		UINT offsets = 0;
		UINT stride = sizeof(VertexData);
		context->IASetVertexBuffers(0, 1, &buf, &stride, &offsets);
		context->IASetIndexBuffer(index, DXGI_FORMAT_R16_UINT, 0);

		ID3D11SamplerState* sampler_state = sampler;
		context->PSSetSamplers(0, 1, &sampler_state);
		context->VSSetShader(vs, nullptr, 0);
		context->PSSetShader(ps, nullptr, 0);
	}

	// REVIEW: This method is not needed
	HRESULT D3D11TestScene::CompileD3D(const char* source, bool isPixelShader, ID3DBlob** code)
	{
		HRESULT hr;
		const char* shaderTarget = "ps_4_0";

		if (!isPixelShader)
			shaderTarget = "vs_4_0";

		ID3DBlob* blob;
		ID3DBlob* error;
		hr = D3DCompile(source, strlen(source), nullptr, nullptr,
			nullptr, "main", shaderTarget, 0, 0, &blob, &error);

		if (FAILED(hr)) {
			const char* err = nullptr;
			if (error)
				err = (const char*)error->GetBufferPointer();

			printf("Couldn't compile pixel shader, error: %s",
				CHECK_STR_NULL(err));
			return hr;
		}

		*code = blob;

		return S_OK;
	}
}

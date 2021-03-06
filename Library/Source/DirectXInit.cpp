#include "./Header/DirectXInit.h"
#include <string>

DirectXInit::DirectXInit() :
	Win32API(),
	clearColor{ 0, 0, 0, 0 },
	dev{},
	dxgiFactory{},
	swapchain{},
	cmdAllocator{},
	cmdList{},
	cmdQueue{},
	rtvHeaps{},
	adapters{},
	tmpAdapter{},
	levels{ D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 },
	featureLevel{},
	cmdQueueDesc{},
	swapchainDesc{},
	heapDesc{},
	backBuffer(2),
	fence{},
	fenceVal(0),
	windowWidth{ wrc.right },
	windowHeight{ wrc.bottom },
	depthResDesc{},
	depthBuffer{},
	dsvHeapDesc{},
	dsvHeap{},
	dsvDesc{},
	bbIndex{}
{
}

#ifdef UNICODE
DirectXInit::DirectXInit(RECT windowSize, LPCWSTR title, LPCWSTR className) :
	Win32API(windowSize, className, title),
	clearColor{ 0, 0, 0, 0 },
	dev{},
	dxgiFactory{},
	swapchain{},
	cmdAllocator{},
	cmdList{},
	cmdQueue{},
	rtvHeaps{},
	adapters{},
	tmpAdapter{},
	levels{ D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 },
	featureLevel{},
	cmdQueueDesc{},
	swapchainDesc{},
	heapDesc{},
	backBuffer(2),
	fence{},
	fenceVal(0),
	windowWidth{ wrc.right },
	windowHeight{ wrc.bottom },
	depthResDesc{},
	depthBuffer{},
	dsvHeapDesc{},
	dsvHeap{},
	dsvDesc{},
	bbIndex{}
{
}
#else
DirectXInit::DirectXInit(RECT windowSize, LPCSTR title, LPCSTR className) :
	Win32API(windowSize, className, title),
	clearColor{ 0, 0, 0, 0 },
	dev{},
	dxgiFactory{},
	swapchain{},
	cmdAllocator{},
	cmdList{},
	cmdQueue{},
	rtvHeaps{},
	adapters{},
	tmpAdapter{},
	levels{ D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 },
	featureLevel{},
	cmdQueueDesc{},
	swapchainDesc{},
	heapDesc{},
	backBuffer(2),
	fence{},
	fenceVal(0),
	windowWidth{ wrc.right },
	windowHeight{ wrc.bottom },
	depthResDesc{},
	depthBuffer{},
	dsvHeapDesc{},
	dsvHeap{},
	dsvDesc{},
	bbIndex{}
{
}
#endif // UNICODE

void DirectXInit::DebugLayer()
{
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
}

HRESULT DirectXInit::Init()
{
	HRESULT hr;

	WindowInit();
	DebugLayer();

	// DXGI?t?@?N?g???[??????
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		return hr;
	}

	for (int i = 0; dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter); //???I?z????????????
	}

	for (size_t i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc); //?A?_?v?^?[????????????

		// ?\?t?g?E?F?A?f?o?C?X??????
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		std::wstring strDesc = adesc.Description; //?A?_?v?^?[??
		// Intel UHD Graphics (?I???{?[?h?O???t?B?b?N)??????
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; //???p
			break;
		}
	}

	for (int i = 0; i < _countof(levels); i++)
	{
		// ???p?????A?_?v?^?[???f?o?C?X??????
		hr = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (hr == S_OK)
		{
			// ?f?o?C?X???????o???????_?????[?v????????
			featureLevel = levels[i];
			break;
		}
	}
	if (FAILED(hr))
	{
		// ?f?o?C?X?????????????????^?[??
		return hr;
	}

	// ?R?}???h?A???P?[?^??????
	hr = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	// ?R?}???h???X?g??????
	hr = dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&cmdList)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	if (FAILED(hr))
	{
		return hr;
	}

	swapchainDesc.Width = windowWidth;
	swapchainDesc.Height = windowHeight;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //?F??????????
	swapchainDesc.SampleDesc.Count = 1; //?}???`?T???v????????
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; //?o?b?N?o?b?t?@?p
	swapchainDesc.BufferCount = 2; //?o?b?t?@????2????????
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //?t???b?v?????j??
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain1> swapchain1;
	hr = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		hWnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1
	);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = swapchain1.As(&swapchain);
	if (FAILED(hr))
	{
		return hr;
	}

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //?????_?[?^?[?Q?b?g?r???[
	heapDesc.NumDescriptors = 2; //???\??2??
	hr = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(hr))
	{
		return hr;
	}

	for (UINT i = 0; i < 2; i++)
	{
		// ?X???b?v?`?F?C???????o?b?t?@??????
		hr = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffer[i]));
		if (FAILED(hr))
		{
			return hr;
		}
		// ?X?f?X?N???v?^?q?[?v???n???h????????
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
			CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
				i,
				dev->GetDescriptorHandleIncrementSize(heapDesc.Type)
			);
		// ?????_?[?^?[?Q?b?g?r???[??????
		dev->CreateRenderTargetView(backBuffer[i].Get(), nullptr, handle);
	}

	CreateDepthBuffer();

	hr = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	return hr;
}

void DirectXInit::ClearScreen()
{
#pragma region ChangeResourceBarrier

	// ?o?b?N?o?b?t?@????????????(2????????0????1??)
	bbIndex = swapchain->GetCurrentBackBufferIndex();

	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer[bbIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

#pragma endregion

#pragma region DrawingDestination

	// ?`???????w??
	// ?????_?[?^?[?Q?b?g?r???[?p?f?B?X?N???v?^?q?[?v???n???h????????
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(),
			(INT)bbIndex,
			dev->GetDescriptorHandleIncrementSize(heapDesc.Type)
		);
	// ?[?x?X?e???V???r???[?p?f?B?X?N???v?^?q?[?v???n???h????????
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

#pragma endregion

#pragma region ClearScreen

	// ?????N???A
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

#pragma endregion
}

void DirectXInit::ScreenFlip()
{
#pragma region RetrunResourceBarrier

	// ???\?[?X?o???A??????
	cmdList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer[bbIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);

#pragma endregion

#pragma region FlashCommand

	// ???????N???[?Y
	cmdList->Close();
	// ?R?}???h???X?g?????s
	ID3D12CommandList* cmdLists[] = { cmdList.Get() }; //?R?}???h???X?g???z??
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	// ?R?}???h???X?g?????s??????????
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

#pragma endregion

#pragma region ScreenFlip

	cmdAllocator->Reset(); //?L???[???N???A
	cmdList->Reset(cmdAllocator.Get(), nullptr); //?????R?}???h???X?g????????????

	// ?o?b?t?@???t???b?v(???\??????????)
	swapchain->Present(1, 0);

#pragma endregion
}

HRESULT DirectXInit::CreateDepthBuffer()
{
	HRESULT hr;

	// ???\?[?X????
	depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		windowWidth,
		windowHeight,
		1,
		0,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	// ???\?[?X????
	hr = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, //?[?x?l???????????g?p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);
	if (FAILED(hr))
	{
		return hr;
	}

	// ?[?x?r???[?p?f?X?N???v?^?q?[?v????
	dsvHeapDesc.NumDescriptors = 1; //?[?x?r???[??1??
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; //?f?v?X?X?e???V???r???[
	hr = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	if (FAILED(hr))
	{
		return hr;
	}

	// ?[?x?r???[????
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; //?[?x?l?t?H?[?}?b?g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	return hr;
}

int DirectXInit::SetWindowSize(int width, int height)
{
	if (width < wrc.left || height < wrc.bottom)
	{
		return -1;
	}

	windowWidth = width;
	windowHeight = height;

	wrc.right = width;
	wrc.bottom = height;

	return 0;
}

LONG DirectXInit::GetWindowWidth() const
{
	return windowWidth;
}

LONG DirectXInit::GetWindowHeight() const
{
	return windowHeight;
}

float DirectXInit::GetWindowWidthF() const
{
	return (float)windowWidth;
}

float DirectXInit::GetWindowHeightF() const
{
	return (float)windowHeight;
}

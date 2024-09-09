#include "DxHandler.h"

#include "initguid.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"

#include <QDebug>

using namespace Microsoft::WRL;

DxHandler::DxHandler()
{

}

DxHandler::~DxHandler()
{

}

bool DxHandler::init()
{
#if defined(_DEBUG)
    // Enable the D3D12 debug layer.
    {

        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }
#endif

    ComPtr<IDXGIFactory4> dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr))
    {
        return false;
    }

    ComPtr<IDXGIAdapter1> adapter;
    for (UINT adapterIndex = 0;
        DXGI_ERROR_NOT_FOUND !=
        dxgiFactory->EnumAdapters1(adapterIndex, &adapter);
        ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        hr = adapter->GetDesc1(&desc);
        if (FAILED(hr))
            continue;

        qDebug() << __FUNCTION__ << "DXGI Adapter:  " << desc.Description
            << " - LUID: " << desc.AdapterLuid.HighPart << "." << desc.AdapterLuid.LowPart;

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // Don't select the Basic Render Driver adapter.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12,
        // but don't create the actual device yet.
        if (SUCCEEDED(
            D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }
    }

#if !defined(NDEBUG)
    if (!adapter)
    {
        if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter))))
        {
            adapter.Reset();
        }
    }
#endif

    ComPtr<ID3D12Device> device;
    hr = D3D12CreateDevice(adapter.Get(),
        D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    if (FAILED(hr))
    {
        return false;
    }

    static const D3D_FEATURE_LEVEL s_featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
    {
        _countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    hr = device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
        &featLevels, sizeof(featLevels));
    if (SUCCEEDED(hr))
    {
        featureLevel = featLevels.MaxSupportedFeatureLevel;
    }

    //qDebug() << __FUNCTION__ << "DXGI Adapter:  " << desc.Description
    //    << " - LUID: " << desc.AdapterLuid.HighPart << "." << desc.AdapterLuid.LowPart;

    LUID luid = device->GetAdapterLuid();

    qDebug() << __FUNCTION__ << luid.HighPart << "." << luid.LowPart;

    return true;
}

void DxHandler::update()
{

}
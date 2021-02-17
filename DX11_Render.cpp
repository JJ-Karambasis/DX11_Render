#include <Windows.h>
#include <ak_common.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>

int CALLBACK 
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLineArgs, int CmdLineOpts)
{
    ak_window* Window = AK_CreateWindow(1280, 720, "DX11_Render");
    if(!Window)
        return -1;
    
    ak_v2i Resolution = {};
    AK_GetWindowResolution(Window, (ak_u16*)&Resolution.w, (ak_u16*)&Resolution.h);
    
    IDXGIFactory* Factory = NULL;
    CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory);
    
    D3D_FEATURE_LEVEL FeatureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };
    
    ID3D11Device* Device = NULL;
    ID3D11DeviceContext* DeviceContext = NULL;
    D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, 
                      FeatureLevels, AK_Count(FeatureLevels), D3D11_SDK_VERSION, &Device, 
                      NULL, &DeviceContext);
    
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = Resolution.w;
    SwapChainDesc.BufferDesc.Height = Resolution.h;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = 2;
    SwapChainDesc.OutputWindow = AK_GetPlatformWindow(Window);
    SwapChainDesc.Windowed = TRUE;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    IDXGISwapChain* SwapChain = NULL;
    Factory->CreateSwapChain(Device, &SwapChainDesc, &SwapChain);
    
    ID3D11Texture2D* BackBuffer = NULL;
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
    
    ID3D11Texture2D* MultisampleBuffer = NULL;
    DXGI_FORMAT MultisampleFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;    
    //UINT MaxSupportedSampleCount = (Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? 8 : 4;    
    UINT MaxSupportedSampleCount = 8;
    UINT MultisampleQualityLevel;
    if(SUCCEEDED(Device->CheckMultisampleQualityLevels(MultisampleFormat, 
                                                       MaxSupportedSampleCount, 
                                                       &MultisampleQualityLevel)))
    {
        D3D11_TEXTURE2D_DESC TextureDescription = {};
        TextureDescription.Width = Resolution.w;
        TextureDescription.Height = Resolution.h;
        TextureDescription.MipLevels = 1;
        TextureDescription.ArraySize = 1;
        TextureDescription.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;    
        TextureDescription.SampleDesc.Count = MaxSupportedSampleCount;
        TextureDescription.SampleDesc.Quality = MultisampleQualityLevel-1;
        TextureDescription.Usage = D3D11_USAGE_DEFAULT;
        TextureDescription.BindFlags = D3D11_BIND_RENDER_TARGET;        
        Device->CreateTexture2D(&TextureDescription, NULL, &MultisampleBuffer);        
    }
    else
    {
        return -1;
    }
    
    
    ID3D11RenderTargetView* RenderTargetView = NULL;
    {
        D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDescription = {};
        RenderTargetViewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        RenderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        Device->CreateRenderTargetView(MultisampleBuffer, &RenderTargetViewDescription, &RenderTargetView);
    }
    
    ID3D11Buffer* VertexBuffer = NULL;    
    ID3D11Buffer* IndexBuffer = NULL;
    
    {        
        ak_vertex_p4_c4 Triangle[] = 
        {
            {AK_V4(-0.5f, -0.5f, 0.0f, 1.0f), AK_Blue4()*0.5f}, 
            {AK_V4( 0.5f, -0.5f, 0.0f, 1.0f), AK_Green4()*0.5f}, 
            {AK_V4( 0.0f,  0.5f, 0.0f, 1.0f), AK_Red4()*0.5f}
        };        
        
        D3D11_BUFFER_DESC BufferDescription = {};
        BufferDescription.Usage = D3D11_USAGE_DEFAULT;
        BufferDescription.ByteWidth = sizeof(Triangle);
        BufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = Triangle;
        
        Device->CreateBuffer(&BufferDescription, &InitData, &VertexBuffer);
    }
    
    {
        ak_u16 TriangleIndices[] = {0, 1, 2};
        D3D11_BUFFER_DESC BufferDescription = {};
        BufferDescription.Usage = D3D11_USAGE_DEFAULT;
        BufferDescription.ByteWidth = sizeof(TriangleIndices);
        BufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = TriangleIndices;
        
        Device->CreateBuffer(&BufferDescription, &InitData, &IndexBuffer);
    }  
    
    const ak_char* VertexTarget = (Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "vs_5_0" : "vs_4_0";
    const ak_char* PixelTarget = (Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "ps_5_0" : "ps_4_0";        
    
    UINT ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS|D3DCOMPILE_DEBUG|D3DCOMPILE_WARNINGS_ARE_ERRORS|D3DCOMPILE_SKIP_OPTIMIZATION;
    
    ID3DBlob* VertexShaderCode = NULL;
    ID3DBlob* PixelShaderCode = NULL;
    
    {
        ID3DBlob* ShaderErrorMessage = NULL;
        HRESULT HResult = D3DCompileFromFile(L"../DX11_Shader.hlsl", NULL, NULL, "ObjectVertexShader", VertexTarget, ShaderFlags, 0, &VertexShaderCode, &ShaderErrorMessage);
        if(FAILED(HResult))
        {
            AK_MessageBoxOk("Error", (ak_char*)ShaderErrorMessage->GetBufferPointer());
            return -1;
        }
        
        HResult = D3DCompileFromFile(L"../DX11_Shader.hlsl", NULL, NULL, "ObjectPixelShader", PixelTarget, ShaderFlags, 0, &PixelShaderCode, &ShaderErrorMessage);
        if(FAILED(HResult))
        {
            AK_MessageBoxOk("Error", (ak_char*)ShaderErrorMessage->GetBufferPointer());
            return -1;
        }
    }
    
    ID3D11VertexShader* VertexShader = NULL;
    ID3D11PixelShader* PixelShader = NULL;
    
    Device->CreateVertexShader(VertexShaderCode->GetBufferPointer(), VertexShaderCode->GetBufferSize(), NULL, &VertexShader);
    Device->CreatePixelShader(PixelShaderCode->GetBufferPointer(), PixelShaderCode->GetBufferSize(), NULL, &PixelShader);
    
    D3D11_INPUT_ELEMENT_DESC InputElementDesc[] = 
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, 
        {"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    
    ID3D11InputLayout* InputLayout = NULL;
    Device->CreateInputLayout(InputElementDesc, AK_Count(InputElementDesc), VertexShaderCode->GetBufferPointer(), 
                              VertexShaderCode->GetBufferSize(), &InputLayout);    
    
    D3D11_RASTERIZER_DESC RasterizerDesc = {};
    RasterizerDesc.FillMode = D3D11_FILL_SOLID;
    RasterizerDesc.CullMode = D3D11_CULL_BACK;
    RasterizerDesc.FrontCounterClockwise = TRUE;
    ID3D11RasterizerState* RasterizerState = NULL;
    Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
        
    for(;;)
    {
        MSG Message;
        while(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
        {
            switch(Message.message)
            {
                case WM_QUIT:
                {
                    return 0;
                } break;
                
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                } 
            }
        }    
        
        DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
        
        D3D11_VIEWPORT Viewport = {};
        Viewport.Width = (ak_f32)Resolution.w;
        Viewport.Height = (ak_f32)Resolution.h;
        Viewport.MinDepth = 0.0f;        
        Viewport.MaxDepth = 1.0f;        
        DeviceContext->RSSetViewports(1, &Viewport);
        
        ak_color4f Color = AK_White4();
        DeviceContext->ClearRenderTargetView(RenderTargetView, (const float*)&Color);
        
        DeviceContext->PSSetShader(PixelShader, NULL, 0);
        DeviceContext->VSSetShader(VertexShader, NULL, 0);
        
        DeviceContext->RSSetState(RasterizerState);
        DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        DeviceContext->IASetInputLayout(InputLayout);
        
        UINT Stride = sizeof(ak_vertex_p4_c4);
        UINT Offset = 0;
        DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
        DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        
        DeviceContext->DrawIndexed(3, 0, 0);                
        DeviceContext->ResolveSubresource(BackBuffer, 0, MultisampleBuffer, 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);        
        SwapChain->Present(1, 0);
    }            
}

#define AK_COMMON_IMPLEMENTATION
#include <ak_common.h>
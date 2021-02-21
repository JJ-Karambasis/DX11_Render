#include <Windows.h>
#include <ak_common.h>

#include <imgui.cpp>
#include <imgui_widgets.cpp>
#include <imgui_draw.cpp>
#include <imgui_tables.cpp>
#include <imgui_demo.cpp>

#include <backends/imgui_impl_win32.cpp>
#include <backends/imgui_impl_dx11.cpp>

#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

#define RENDER_TARGET_VIEW_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM

struct dx11_context
{
    IDXGIFactory* Factory;
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
    IDXGISwapChain* SwapChain;
    ID3D11Texture2D* BackBuffer;
    ID3D11RenderTargetView* RenderTargetView;    
    UINT SampleCount;
    UINT QualityLevel;
    ak_v2u Resolution;
};

struct game_render_target
{
    ak_v2u Resolution;
    ID3D11Texture2D* MultisampleBuffer;
    ID3D11RenderTargetView* MultisampleView;
    ID3D11Texture2D* GameBuffer;
    ID3D11ShaderResourceView* GameView;
};

void DeleteGameRenderTarget(game_render_target* RenderTarget)
{
    RELEASE_COM(RenderTarget->MultisampleBuffer);
    RELEASE_COM(RenderTarget->MultisampleView);
    RELEASE_COM(RenderTarget->GameBuffer);
}

void CreateGameRenderTarget(ID3D11Device* Device, game_render_target* RenderTarget, ak_u32 SampleCount, ak_u32 QualityLevel, 
                            ak_v2u Resolution)
{
    D3D11_TEXTURE2D_DESC MultisampleTextureDescription = {};
    MultisampleTextureDescription.Width = Resolution.w;
    MultisampleTextureDescription.Height = Resolution.h;
    MultisampleTextureDescription.MipLevels = 1;
    MultisampleTextureDescription.ArraySize = 1;
    MultisampleTextureDescription.Format = RENDER_TARGET_VIEW_FORMAT;
    MultisampleTextureDescription.SampleDesc.Count = SampleCount;
    MultisampleTextureDescription.SampleDesc.Quality = QualityLevel-1;
    MultisampleTextureDescription.Usage = D3D11_USAGE_DEFAULT;
    MultisampleTextureDescription.BindFlags = D3D11_BIND_RENDER_TARGET;        
    Device->CreateTexture2D(&MultisampleTextureDescription, NULL, &RenderTarget->MultisampleBuffer);        
    
    D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDescription = {};
    RenderTargetViewDescription.Format = RENDER_TARGET_VIEW_FORMAT;
    RenderTargetViewDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    Device->CreateRenderTargetView(RenderTarget->MultisampleBuffer, &RenderTargetViewDescription, &RenderTarget->MultisampleView);
    
    D3D11_TEXTURE2D_DESC GameTextureDescription = {};
    GameTextureDescription.Width = Resolution.w;
    GameTextureDescription.Height = Resolution.h;
    GameTextureDescription.MipLevels = 1;
    GameTextureDescription.ArraySize = 1;
    GameTextureDescription.Format = RENDER_TARGET_VIEW_FORMAT;
    GameTextureDescription.SampleDesc.Count = 1;
    GameTextureDescription.SampleDesc.Quality = 0;
    GameTextureDescription.Usage = D3D11_USAGE_DEFAULT;
    GameTextureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Device->CreateTexture2D(&GameTextureDescription, NULL, &RenderTarget->GameBuffer);
    Device->CreateShaderResourceView(RenderTarget->GameBuffer, NULL, &RenderTarget->GameView);
    
    RenderTarget->Resolution = Resolution;
}

void CreateRenderTarget(dx11_context* Context)
{    
    Context->SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&Context->BackBuffer);
    Context->Device->CreateRenderTargetView(Context->BackBuffer, NULL, &Context->RenderTargetView);
}

void CleanupRenderTarget(dx11_context* Context)
{
    RELEASE_COM(Context->BackBuffer);
    RELEASE_COM(Context->RenderTargetView);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WindowProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{    
    if (ImGui_ImplWin32_WndProcHandler(Window, Message, WParam, LParam))
        return true;
    
    local dx11_context* Context = NULL;
    
    switch (Message)
    {
        case WM_CREATE:
        {
            CREATESTRUCT* CreateStruct = (CREATESTRUCT*)LParam;
            Context = (dx11_context*)CreateStruct->lpCreateParams;
        } break;
        
        case WM_SIZE:
        {
            if(Context)
            {
                if (Context->Device != NULL && WParam != SIZE_MINIMIZED)
                {
                    CleanupRenderTarget(Context);
                    Context->Resolution = AK_V2((UINT)LOWORD(LParam), (UINT)HIWORD(LParam));
                    Context->SwapChain->ResizeBuffers(0, Context->Resolution.w, Context->Resolution.h, DXGI_FORMAT_UNKNOWN, 0);
                    CreateRenderTarget(Context);
                }            
                return 0;
            }
        } break;
        
        case WM_SYSCOMMAND:        
        {
            if ((WParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
        } break;
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        
        case WM_DPICHANGED:
        {
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
            {
                //const int dpi = HIWORD(wParam);
                //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
                const RECT* suggested_rect = (RECT*)LParam;
                SetWindowPos(Window, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
            }
        } break;
    }
    
    return DefWindowProc(Window, Message, WParam, LParam);
}

int CALLBACK 
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLineArgs, int CmdLineOpts)
{   
    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_VREDRAW|CS_HREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = GetModuleHandle(0);
    WindowClass.lpszClassName = "DX11_Render";    
    RegisterClassEx(&WindowClass);
    
    dx11_context Context = {};    
    
    Context.Resolution = {1280, 720};
    RECT WindowRect = {0, 0, (LONG)Context.Resolution.w, (LONG)Context.Resolution.h};
    AdjustWindowRectEx(&WindowRect, WS_OVERLAPPEDWINDOW|WS_VISIBLE, FALSE, 0);        
    HWND Window = CreateWindow(WindowClass.lpszClassName, "DX11_Render", (WS_OVERLAPPEDWINDOW|WS_VISIBLE), CW_USEDEFAULT, CW_USEDEFAULT, 
                               WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, 0, 0, GetModuleHandle(0), &Context);
    
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO* IO = &ImGui::GetIO();
    IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard|ImGuiConfigFlags_DockingEnable|ImGuiConfigFlags_ViewportsEnable;
    
    ImGui::StyleColorsDark();
    ImGuiStyle* Style = &ImGui::GetStyle();
    if (IO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        Style->WindowRounding = 0.0f;
        Style->Colors[ImGuiCol_WindowBg].w = 1.0f;
    }    
    
    CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Context.Factory);
    
    D3D_FEATURE_LEVEL FeatureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };
    
    D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, 
                      FeatureLevels, AK_Count(FeatureLevels), D3D11_SDK_VERSION, &Context.Device, 
                      NULL, &Context.DeviceContext);
    
    Context.SampleCount = (Context.Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? 8 : 4;
	//Context.SampleCount = 1; 
    if(FAILED(Context.Device->CheckMultisampleQualityLevels(RENDER_TARGET_VIEW_FORMAT, Context.SampleCount, &Context.QualityLevel)))        
        return false;    
    
    ImGui_ImplWin32_Init(Window);
    ImGui_ImplDX11_Init(Context.Device, Context.DeviceContext);
    
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = Context.Resolution.w;
    SwapChainDesc.BufferDesc.Height = Context.Resolution.h;
    SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapChainDesc.SampleDesc.Count = 1;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = 2;
    SwapChainDesc.OutputWindow = Window;
    SwapChainDesc.Windowed = TRUE;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;    
    Context.Factory->CreateSwapChain(Context.Device, &SwapChainDesc, &Context.SwapChain);    
    
    CreateRenderTarget(&Context);
    
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
        
        Context.Device->CreateBuffer(&BufferDescription, &InitData, &VertexBuffer);
    }
    
    {
        ak_u16 TriangleIndices[] = {0, 1, 2};
        D3D11_BUFFER_DESC BufferDescription = {};
        BufferDescription.Usage = D3D11_USAGE_DEFAULT;
        BufferDescription.ByteWidth = sizeof(TriangleIndices);
        BufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
        
        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = TriangleIndices;
        
        Context.Device->CreateBuffer(&BufferDescription, &InitData, &IndexBuffer);
    }  
    
    const ak_char* VertexTarget = (Context.Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "vs_5_0" : "vs_4_0";
    const ak_char* PixelTarget = (Context.Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "ps_5_0" : "ps_4_0";        
    
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
    
    Context.Device->CreateVertexShader(VertexShaderCode->GetBufferPointer(), VertexShaderCode->GetBufferSize(), NULL, &VertexShader);
    Context.Device->CreatePixelShader(PixelShaderCode->GetBufferPointer(), PixelShaderCode->GetBufferSize(), NULL, &PixelShader);
    
    D3D11_INPUT_ELEMENT_DESC InputElementDesc[] = 
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, 
        {"Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    
    ID3D11InputLayout* InputLayout = NULL;
    Context.Device->CreateInputLayout(InputElementDesc, AK_Count(InputElementDesc), VertexShaderCode->GetBufferPointer(), 
                                      VertexShaderCode->GetBufferSize(), &InputLayout);    
    
    D3D11_RASTERIZER_DESC RasterizerDesc = {};
    RasterizerDesc.FillMode = D3D11_FILL_SOLID;
    RasterizerDesc.CullMode = D3D11_CULL_BACK;
    RasterizerDesc.FrontCounterClockwise = TRUE;
    ID3D11RasterizerState* RasterizerState = NULL;
    Context.Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
    
    game_render_target GameRenderTarget[2] = {};
    
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
        
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        ImGuiViewport* MainViewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(MainViewport->Pos);
        ImGui::SetNextWindowSize(MainViewport->Size);
        ImGuiID ID = AK_HashFunction("Main Window");
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushID(ID);
        ImGui::Begin("Main Window", NULL, ImGuiWindowFlags_NoDecoration);
        ImGui::DockSpace(ID, MainViewport->Size, ImGuiDockNodeFlags_None, NULL);
        ImGui::End();
        ImGui::PopID();
        ImGui::PopStyleVar();
        
        ImGui::SetNextWindowPos(MainViewport->Pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(MainViewport->Size, ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game Viewport 0");
        {
            ak_v2u WindowDim = AK_V2u(ImGui::GetWindowSize());
            if(GameRenderTarget[0].Resolution != WindowDim)
            {
                DeleteGameRenderTarget(&GameRenderTarget[0]);
                CreateGameRenderTarget(Context.Device, &GameRenderTarget[0], Context.SampleCount, Context.QualityLevel, WindowDim);
            }
            
            Context.DeviceContext->OMSetRenderTargets(1, &GameRenderTarget[0].MultisampleView, NULL);
            
            D3D11_VIEWPORT Viewport = {};
            Viewport.Width = (ak_f32)WindowDim.w;
            Viewport.Height = (ak_f32)WindowDim.h;
            Viewport.MinDepth = 0.0f;        
            Viewport.MaxDepth = 1.0f;        
            Context.DeviceContext->RSSetViewports(1, &Viewport);
            
            ak_color4f Color = AK_White4();
            Context.DeviceContext->ClearRenderTargetView(GameRenderTarget[0].MultisampleView, (const float*)&Color);
            
            Context.DeviceContext->PSSetShader(PixelShader, NULL, 0);
            Context.DeviceContext->VSSetShader(VertexShader, NULL, 0);
            
            Context.DeviceContext->RSSetState(RasterizerState);
            Context.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            Context.DeviceContext->IASetInputLayout(InputLayout);
            
            UINT Stride = sizeof(ak_vertex_p4_c4);
            UINT Offset = 0;
            Context.DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
            Context.DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
            
            Context.DeviceContext->DrawIndexed(3, 0, 0);                           
            
            Context.DeviceContext->ResolveSubresource(GameRenderTarget[0].GameBuffer, 0, GameRenderTarget[0].MultisampleBuffer, 0, RENDER_TARGET_VIEW_FORMAT);
            
            ImVec2 TopLeft = ImGui::GetCursorScreenPos();
            ImVec2 BottomRight = TopLeft+ImGui::GetWindowSize();
            
            ImGui::GetWindowDrawList()->AddImage((void*)GameRenderTarget[0].GameView, TopLeft, BottomRight, ImVec2(0, 0), ImVec2(1, 1));
        }
        ImGui::End();
        ImGui::PopStyleVar();
        
        ImGui::SetNextWindowPos(MainViewport->Pos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(MainViewport->Size, ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Game Viewport 1");
        {
            AK_ConsoleLog("Is Window Docked %d\n", ImGui::IsWindowDocked());
            
            ak_v2u WindowDim = AK_V2u(ImGui::GetWindowSize());
            if(GameRenderTarget[1].Resolution != WindowDim)
            {
                DeleteGameRenderTarget(&GameRenderTarget[1]);
                CreateGameRenderTarget(Context.Device, &GameRenderTarget[1], Context.SampleCount, Context.QualityLevel, WindowDim);
            }
            
            Context.DeviceContext->OMSetRenderTargets(1, &GameRenderTarget[1].MultisampleView, NULL);
            
            D3D11_VIEWPORT Viewport = {};
            Viewport.Width = (ak_f32)WindowDim.w;
            Viewport.Height = (ak_f32)WindowDim.h;
            Viewport.MinDepth = 0.0f;        
            Viewport.MaxDepth = 1.0f;        
            Context.DeviceContext->RSSetViewports(1, &Viewport);
            
            ak_color4f Color = AK_White4();
            Context.DeviceContext->ClearRenderTargetView(GameRenderTarget[1].MultisampleView, (const float*)&Color);
            
            Context.DeviceContext->PSSetShader(PixelShader, NULL, 0);
            Context.DeviceContext->VSSetShader(VertexShader, NULL, 0);
            
            Context.DeviceContext->RSSetState(RasterizerState);
            Context.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            Context.DeviceContext->IASetInputLayout(InputLayout);
            
            UINT Stride = sizeof(ak_vertex_p4_c4);
            UINT Offset = 0;
            Context.DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
            Context.DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
            
            Context.DeviceContext->DrawIndexed(3, 0, 0);                           
            
            Context.DeviceContext->ResolveSubresource(GameRenderTarget[1].GameBuffer, 0, GameRenderTarget[1].MultisampleBuffer, 0, RENDER_TARGET_VIEW_FORMAT);
            
            ImVec2 TopLeft = ImGui::GetCursorScreenPos();
            ImVec2 BottomRight = TopLeft+ImGui::GetWindowSize();
            
            ImGui::GetWindowDrawList()->AddImage((void*)GameRenderTarget[1].GameView, TopLeft, BottomRight, ImVec2(0, 0), ImVec2(1, 1));
        }
        ImGui::End();
        ImGui::PopStyleVar();
        
        ImGui::ShowDemoWindow();
        
        ImGui::Render();
        
        Context.DeviceContext->OMSetRenderTargets(1, &Context.RenderTargetView, NULL);        
        D3D11_VIEWPORT Viewport = {};
        Viewport.Width = (ak_f32)Context.Resolution.w;
        Viewport.Height = (ak_f32)Context.Resolution.h;
        Viewport.MinDepth = 0.0f;        
        Viewport.MaxDepth = 1.0f;        
        Context.DeviceContext->RSSetViewports(1, &Viewport);
        
        ak_color4f Color = AK_White4();
        Context.DeviceContext->ClearRenderTargetView(Context.RenderTargetView, (const float*)&Color);
        
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        
        // Update and Render additional Platform Windows
        if (IO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }        
        
        
        Context.SwapChain->Present(1, 0);
    }            
}

#define AK_COMMON_IMPLEMENTATION
#include <ak_common.h>
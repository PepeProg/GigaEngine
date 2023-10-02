#include <GL/gl.h>
#include <GL/glx.h>
#ifndef PLATFORM_LINUX
#    define PLATFORM_LINUX 1
#endif

#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include "Common/interface/RefCntAutoPtr.hpp"

using namespace Diligent;

#ifndef GLX_CONTEXT_MAJOR_VERSION_ARB
#    define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif

#ifndef GLX_CONTEXT_MINOR_VERSION_ARB
#    define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#ifndef GLX_CONTEXT_FLAGS_ARB
#    define GLX_CONTEXT_FLAGS_ARB 0x2094
#endif

#ifndef GLX_CONTEXT_DEBUG_BIT_ARB
#    define GLX_CONTEXT_DEBUG_BIT_ARB 0x0001
#endif

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, int, const int*);

static const char* VSSource = R"(
struct PSInput
{
    float4 Pos   : SV_POSITION;
    float3 Color : COLOR;
};

void main(in  uint    VertId : SV_VertexID,
          out PSInput PSIn)
{
    float4 Pos[3];
    Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
    Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
    Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

    float3 Col[3];
    Col[0] = float3(1.0, 0.0, 0.0); // red
    Col[1] = float3(0.0, 1.0, 0.0); // green
    Col[2] = float3(0.0, 0.0, 1.0); // blue

    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Col[VertId];
}
)";

// Pixel shader will simply output interpolated vertex color
static const char* PSSource = R"(
struct PSInput
{
    float4 Pos   : SV_POSITION;
    float3 Color : COLOR;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)";


class RMEApp
{
public:
    RMEApp()
    {
    }

    ~RMEApp()
    {
        m_pImmediateContext->Flush();
    }

    bool OnGLContextCreated(Display* display, Window NativeWindowHandle)
    {
        SwapChainDesc SCDesc;
        // Declare function pointer
        auto* pFactoryOpenGL = GetEngineFactoryOpenGL();

        EngineGLCreateInfo EngineCI;
        EngineCI.Window.WindowId = NativeWindowHandle;
        EngineCI.Window.pDisplay = display;

        pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);

        return true;
    }

    void CreateResources()
    {
        // Pipeline state object encompasses configuration of all GPU stages

        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc&              PSODesc = PSOCreateInfo.PSODesc;

        // Pipeline state name is used by the engine to report issues
        // It is always a good idea to give objects descriptive names
        PSODesc.Name = "Simple triangle PSO";

        // This is a graphics pipeline
        PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        // This tutorial will render to a single render target
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        // Set render target format which is the format of the swap chain's color buffer
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
        // This tutorial will not use depth buffer
        PSOCreateInfo.GraphicsPipeline.DSVFormat = TEX_FORMAT_D32_FLOAT;
        // Primitive topology defines what kind of primitives will be rendered by this pipeline state
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        // No back face culling for this tutorial
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        // Disable depth testing
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

        ShaderCreateInfo ShaderCI;
        // Tell the system that the shader source code is in HLSL.
        // For OpenGL, the engine will convert this into GLSL behind the scene
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
        ShaderCI.Desc.UseCombinedTextureSamplers = true;
        // Create vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
            ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
            ShaderCI.EntryPoint      = "main";
            ShaderCI.Desc.Name       = "Triangle vertex shader";
            ShaderCI.Source          = VSSource;
            m_pDevice->CreateShader(ShaderCI, &pVS);
        }

        // Create pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
            ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
            ShaderCI.EntryPoint      = "main";
            ShaderCI.Desc.Name       = "Triangle pixel shader";
            ShaderCI.Source          = PSSource;
            m_pDevice->CreateShader(ShaderCI, &pPS);
        }

        // Finally, create the pipeline state
        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;
        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
    }

    void Render()
    {
        // Set render targets before issuing any draw command.
        // Note that Present() unbinds the back buffer if it is set as render target.
        auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
        m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Clear the back buffer
        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        // Let the engine perform required state transitions
        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Set pipeline state in the immediate context
        m_pImmediateContext->SetPipelineState(m_pPSO);

        // Typically we should now call CommitShaderResources(), however shaders in this example don't
        // use any resources.

        DrawAttribs drawAttrs;
        drawAttrs.NumVertices = 3; // We will render 3 vertices
        m_pImmediateContext->Draw(drawAttrs);
    }

    void Present()
    {
        m_pSwapChain->Present();
    }

    void WindowResize(Uint32 Width, Uint32 Height)
    {
        if (m_pSwapChain)
            m_pSwapChain->Resize(Width, Height);
    }

    RENDER_DEVICE_TYPE GetDeviceType() const { return m_DeviceType; }

private:
    RefCntAutoPtr<IRenderDevice>  m_pDevice;
    RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
    RefCntAutoPtr<ISwapChain>     m_pSwapChain;
    RefCntAutoPtr<IPipelineState> m_pPSO;
    RENDER_DEVICE_TYPE            m_DeviceType = RENDER_DEVICE_TYPE_GL;
};

using namespace Diligent;

#if GL_SUPPORTED
int x_main()
{
    std::unique_ptr<RMEApp> TheApp(new RMEApp);

    Display* display = XOpenDisplay(0);

    // clang-format off
    static int visual_attribs[] =
            {
                    GLX_RENDER_TYPE,    GLX_RGBA_BIT,
                    GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
                    GLX_DOUBLEBUFFER,   true,

                    // The largest available total RGBA color buffer size (sum of GLX_RED_SIZE,
                    // GLX_GREEN_SIZE, GLX_BLUE_SIZE, and GLX_ALPHA_SIZE) of at least the minimum
                    // size specified for each color component is preferred.
                    GLX_RED_SIZE,       8,
                    GLX_GREEN_SIZE,     8,
                    GLX_BLUE_SIZE,      8,
                    GLX_ALPHA_SIZE,     8,

                    // The largest available depth buffer of at least GLX_DEPTH_SIZE size is preferred
                    GLX_DEPTH_SIZE,     24,

                    //GLX_SAMPLE_BUFFERS, 1,
                    GLX_SAMPLES, 1,
                    None
            };
    // clang-format on

    int          fbcount = 0;
    GLXFBConfig* fbc     = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    if (!fbc)
    {
        std::cout << "Failed to retrieve a framebuffer config\n";
        return -1;
    }

    XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[0]);

    XSetWindowAttributes swa;
    swa.colormap     = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
    swa.border_pixel = 0;
    swa.event_mask =
            StructureNotifyMask |
            ExposureMask |
            KeyPressMask |
            KeyReleaseMask |
            ButtonPressMask |
            ButtonReleaseMask |
            PointerMotionMask;

    Window win = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0, 1024, 768, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
    if (!win)
    {
        std::cout << "Failed to create window.\n";
        return -1;
    }

    {
        auto SizeHints        = XAllocSizeHints();
        SizeHints->flags      = PMinSize;
        SizeHints->min_width  = 320;
        SizeHints->min_height = 240;
        XSetWMNormalHints(display, win, SizeHints);
        XFree(SizeHints);
    }

    XMapWindow(display, win);

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
    {
        // Create an oldstyle context first, to get the correct function pointer for glXCreateContextAttribsARB
        GLXContext ctx_old         = glXCreateContext(display, vi, 0, GL_TRUE);
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
        glXMakeCurrent(display, None, NULL);
        glXDestroyContext(display, ctx_old);
    }

    if (glXCreateContextAttribsARB == nullptr)
    {
        std::cout << "glXCreateContextAttribsARB entry point not found. Aborting.\n";
        return -1;
    }

    int Flags = GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
#    ifdef _DEBUG
    Flags |= GLX_CONTEXT_DEBUG_BIT_ARB;
#    endif

    int major_version = 4;
    int minor_version = 3;
    // clang-format off
    static int context_attribs[] =
            {
                    GLX_CONTEXT_MAJOR_VERSION_ARB, major_version,
                    GLX_CONTEXT_MINOR_VERSION_ARB, minor_version,
                    GLX_CONTEXT_FLAGS_ARB, Flags,
                    None
            };
    // clang-format on

    constexpr int True = 1;
    GLXContext    ctx  = glXCreateContextAttribsARB(display, fbc[0], NULL, True, context_attribs);
    if (!ctx)
    {
        std::cout << "Failed to create GL context.\n";
        return -1;
    }
    XFree(fbc);


    glXMakeCurrent(display, win, ctx);
    TheApp->OnGLContextCreated(display, win);
    TheApp->CreateResources();
    XStoreName(display, win, "RME");
    while (true)
    {
        bool   EscPressed = false;
        XEvent xev;
        // Handle all events in the queue
        while (XCheckMaskEvent(display, 0xFFFFFFFF, &xev))
        {
            switch (xev.type)
            {
                case KeyPress:
                {
                    KeySym keysym;
                    char   buffer[80];
                    int    num_char = XLookupString((XKeyEvent*)&xev, buffer, _countof(buffer), &keysym, 0);
                    (void)num_char;
                    EscPressed = (keysym == XK_Escape);
                }

                case ConfigureNotify:
                {
                    XConfigureEvent& xce = reinterpret_cast<XConfigureEvent&>(xev);
                    if (xce.width != 0 && xce.height != 0)
                        TheApp->WindowResize(xce.width, xce.height);
                    break;
                }
            }
        }

        if (EscPressed)
            break;

        TheApp->Render();

        TheApp->Present();
    }

    TheApp.reset();

    ctx = glXGetCurrentContext();
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, ctx);
    XDestroyWindow(display, win);
    XCloseDisplay(display);

    return 0;
}
#endif

int main(int argc, char** argv)
{
    std::cout << " 1";

    RENDER_DEVICE_TYPE DevType = RENDER_DEVICE_TYPE_UNDEFINED;

    DevType = RENDER_DEVICE_TYPE_GL;

    std::cout << " 2";

    int arg = 0;
    while (arg < argc && strcmp(argv[arg], "--mode") != 0 && strcmp(argv[arg], "-m") != 0)
        ++arg;
    if (arg + 1 < argc)
    {
        const auto* mode = argv[arg + 1];
        if (strcasecmp(mode, "GL") == 0)
        {
            DevType = RENDER_DEVICE_TYPE_GL;
        }
        else if (strcasecmp(mode, "VK") == 0)
        {
        }
        else
        {
            std::cout << mode << " is not a valid device type. Only the following types are supported: GL, VK";
            return -1;
        }
    }

    std::cout << " 4";

    if (DevType == RENDER_DEVICE_TYPE_GL)
    {
        return x_main();
    }
    std::cout << " 5";

    return -1;
}
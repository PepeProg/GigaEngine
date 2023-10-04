#include <cstdio>
#include <cstring>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"
#include "string"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "iostream"
#include <fstream>
#include <bx/commandline.h>
#include <bx/endian.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <bx/string.h>
#include <entry.h>

SDL_Window* window = NULL;

//static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
//{
//    if (bx::open(_reader, _filePath) )
//    {
//        uint32_t size = (uint32_t)bx::getSize(_reader);
//        const bgfx::Memory* mem = bgfx::alloc(size+1);
//        bx::read(_reader, mem->data, size, bx::ErrorAssert{});
//        bx::close(_reader);
//        mem->data[mem->size-1] = '\0';
//        return mem;
//    }
//
//    return NULL;
//}

//static bgfx::ShaderHandle loadShader(bx::FileReaderI* _reader, const char* _name)
//{
//    char filePath[512];
//
//    const char* shaderPath = "???";
//
//    switch (bgfx::getRendererType() )
//    {
//        case bgfx::RendererType::Noop:
//        case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
//        case bgfx::RendererType::Direct3D11:
//        case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
//        case bgfx::RendererType::Agc:
//        case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
//        case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
//        case bgfx::RendererType::Nvn:        shaderPath = "shaders/nvn/";   break;
//        case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
//        case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
//        case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;
//        case bgfx::RendererType::WebGPU:     shaderPath = "shaders/spirv/"; break;
//
//        case bgfx::RendererType::Count:
//            BX_ASSERT(false, "You should not be here!");
//            break;
//    }
//
//    bx::strCopy(filePath, BX_COUNTOF(filePath), shaderPath);
//    bx::strCat(filePath, BX_COUNTOF(filePath), _name);
//    bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");
//
//    bgfx::ShaderHandle handle = bgfx::createShader(loadMem(_reader, filePath) );
//    bgfx::setName(handle, _name);
//
//    return handle;
//}

//bgfx::ShaderHandle loadShader(const char* _name)
//{
//    return loadShader(entry::getFileReader(), _name);
//}

bgfx::ShaderHandle loadShader(const char* _name) {
    char* data = new char[2048];
    std::ifstream file;
    size_t fileSize;
    file.open(_name);
    if(file.is_open()) {
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        file.read(data, fileSize);
        file.close();
    }
    const bgfx::Memory* mem = bgfx::copy(data,fileSize+1);
    mem->data[mem->size-1] = '\0';
    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, _name);
    return handle;
}

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
        {  0.5f,  0.5f, 0.0f, 0xff0000ff },
        {  0.5f, -0.5f, 0.0f, 0xff0000ff },
        { -0.5f, -0.5f, 0.0f, 0xff00ff00 },
        { -0.5f,  0.5f, 0.0f, 0xff00ff00 }
};


static const uint16_t s_cubeTriList[] =
{
        0,1,3,
        1,2,3
};

bgfx::VertexBufferHandle m_vbh;
bgfx::IndexBufferHandle m_ibh;
bgfx::ProgramHandle m_program;

int main(int argc, char* argv[])
{
    // INIT
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf("SDL not init, SDL_Error: %s\n",
               SDL_GetError());
    }
    else {
        // CREATE WINDOW
        window = SDL_CreateWindow("RME",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  1600, 800,
                                  SDL_WINDOW_SHOWN);
        if(window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n",
                   SDL_GetError());
        }
    }

    // Collect information about the window from SDL
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        return 1;
    }

    bgfx::Init init;
    init.platformData.nwh  = (void*)(uintptr_t)wmi.info.x11.window;
    init.platformData.ndt  = wmi.info.x11.display;
    init.resolution.width  = 1600;
    init.resolution.height = 800;

    // Tell bgfx about the platform and window
    //    bgfx::setPlatformData(pd);

    // Render an empty frame
    bgfx::renderFrame();
    std::cout << " 00000000000000000000000000000000000000  \n";

    // Initialize bgfx
    bgfx::init(init);

    std::cout << " 11111111111111111111111111111111111111  \n";
    PosColorVertex::init();

    std::cout << " 22222222222222222222222222222222222222  \n";

    m_vbh = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
            PosColorVertex::ms_layout
    );

    m_ibh = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );
    std::cout << " 3 \n";

    bgfx::ShaderHandle vsh = loadShader("vs_shader.bin");
    bgfx::ShaderHandle fsh = loadShader("fs_shader.bin");
    std::cout << " 4 \n";

    m_program = bgfx::createProgram(vsh,fsh,  true);
    std::cout << " 5 \n";

    // Reset window
    bgfx::reset(1600, 800, BGFX_RESET_VSYNC);
    std::cout << " 6 \n";

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);
    std::cout << " 7 \n";

    // Set view rectangle for 0th view
    bgfx::setViewRect(0, 0, 0, uint16_t(1600), uint16_t(800));
    std::cout << " 8 \n";

    // Clear the view rect
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x443355FF, 1.0f, 0);
    std::cout << " 9 \n";

    // Set empty primitive on screen
    bgfx::touch(0);
    std::cout << " 10 \n";

    // OPEN TILL CLOSE
    bool quit = false;
    SDL_Event currentEvent;
    while(!quit) {
        while(SDL_PollEvent(&currentEvent) != 0) {
            if(currentEvent.type == SDL_QUIT) {
                quit = true;
            }

            const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
            const bx::Vec3 eye = { 0.0f, 0.0f, 10.0f };

            // Set view and projection matrix for view 0.
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj,
                        60.0f,
                        float(1600)/float(800),
                        0.1f, 100.0f,
                        bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(0, view, proj);

            bgfx::setViewRect(0, 0, 0,
                              1600,
                              800);

            bgfx::touch(0);

            float mtx[16];
            bx::mtxRotateY(mtx, 0.0f);

            // position x,y,z
            mtx[12] = 0.0f;
            mtx[13] = 0.0f;
            mtx[14] = 0.0f;

            // Set model matrix for rendering.
            bgfx::setTransform(mtx);

            // Set vertex and index buffer.
            bgfx::setVertexBuffer(0, m_vbh);
            bgfx::setIndexBuffer(m_ibh);

            // Set render states.
            bgfx::setState(BGFX_STATE_DEFAULT);
            // Submit primitive for rendering to view 0.
            bgfx::submit(0, m_program);

            bgfx::frame();
        }
    }

    // OFF SYSTEMS
    SDL_DestroyWindow(window);
    bgfx::shutdown();
    SDL_Quit();

    return 0;
}
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

#ifdef __APPLE__
#include <vs_shader.sc.mtl.bin.h>
#include <fs_shader.sc.mtl.bin.h>
#define vertexPivko vs_shader_mtl
#define fragmentPivko fs_shader_mtl
#endif
#ifdef __linux__
#include <vs_shader.sc.glsl.bin.h>
#include <fs_shader.sc.glsl.bin.h>
#define vertexPivko vs_shader_glsl
#define fragmentPivko fs_shader_glsl
#endif

SDL_Window* window = NULL;

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

#ifdef __APPLE__
    init.platformData.nwh  = (void*)(uintptr_t)wmi.info.cocoa.window;
    init.platformData.ndt  = wmi.info.cocoa.window;
#endif
#ifdef __linux__
    init.platformData.nwh  = (void*)(uintptr_t)wmi.info.x11.window;
    init.platformData.ndt  = wmi.info.x11.display;
#endif

    init.resolution.width  = 1600;
    init.resolution.height = 800;

    // Render an empty frame
    bgfx::renderFrame();

    // Initialize bgfx
    bgfx::init(init);

    PosColorVertex::init();

    m_vbh = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
            PosColorVertex::ms_layout
    );

    m_ibh = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );

    const bgfx::Memory* memVertex = bgfx::copy(vertexPivko, sizeof(vertexPivko));
    bgfx::ShaderHandle handleVertex = bgfx::createShader(memVertex);

    const bgfx::Memory* memFragment = bgfx::copy(fragmentPivko, sizeof(fragmentPivko));
    bgfx::ShaderHandle handleFragment = bgfx::createShader(memFragment);
    m_program = bgfx::createProgram(handleVertex, handleFragment,  true);

    // Reset window
    bgfx::reset(1600, 800, BGFX_RESET_VSYNC);

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

    // Set view rectangle for 0th view
    bgfx::setViewRect(0, 0, 0, uint16_t(1600), uint16_t(800));

    // Clear the view rect
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x443355FF, 1.0f, 0);

    // Set empty primitive on screen
    bgfx::touch(0);

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
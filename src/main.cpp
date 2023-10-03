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
            bgfx::frame();
            if(currentEvent.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    // OFF SYSTEMS
    SDL_DestroyWindow(window);
    bgfx::shutdown();
    SDL_Quit();

    return 0;
}
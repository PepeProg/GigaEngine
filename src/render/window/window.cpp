#include "window.h"
#include <cstring>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <iostream>
#include <ctime>

static SDL_Window* window = NULL;

void WindowSDL::init(uint32_t _width, uint32_t _height) {

    if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
        printf("SDL not init, SDL_Error: %s\n",
               SDL_GetError());
    }
    else {
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

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(window, &wmi);

    bgfx::Init init;

#ifdef __APPLE__
    init.platformData.nwh  = (void*)(uintptr_t)wmi.info.cocoa.window;
    init.platformData.ndt  = wmi.info.cocoa.window;
#endif
#ifdef __linux__
    init.platformData.nwh  = (void*)(uintptr_t)wmi.info.x11.window;
    init.platformData.ndt  = wmi.info.x11.display;
#endif
    init.resolution.width  = _width;
    init.resolution.height = _height;

    // Render an empty frame
    bgfx::renderFrame();

    // Initialize bgfx
    bgfx::init(init);

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

    Apps = std::vector<Application*>();
}

void WindowSDL::update()
{
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

            // Set render states.
            bgfx::setState(BGFX_STATE_DEFAULT);

            for (auto app : Apps) {
                if (!app->isInited) {
                    app->init();
                }
                app->update();
            }

            bgfx::frame();
        }
    }
}

void WindowSDL::setRenderObj(Application* App) {
    Apps.push_back(App);
}

void WindowSDL::shutdown()
{
    SDL_DestroyWindow(window);
    bgfx::shutdown();
    SDL_Quit();
}


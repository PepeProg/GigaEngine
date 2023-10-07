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

class WindowSDL {
public:
    void init(uint32_t _width, uint32_t _height);
    void update();
    void shutdown();
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;
    int64_t m_timeOffset;
};



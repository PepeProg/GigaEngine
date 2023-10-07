#include <bgfx/bgfx.h>

class WindowSDL {
public:
    void init(uint32_t _width, uint32_t _height);
    void update();
    void shutdown();
    void setRenderObj();

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;
    int64_t m_timeOffset;
};



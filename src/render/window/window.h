#include <bgfx/bgfx.h>
#include <vector>
#include "../application/application.h"

class WindowSDL {
public:
    void init(uint32_t _width, uint32_t _height);
    void update();
    void shutdown();
    void setRenderObj(Application* App);

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;
    int64_t m_timeOffset;

private:
    std::vector<Application*> Apps;
};



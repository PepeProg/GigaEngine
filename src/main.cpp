#include "render/window/window.h"
#include "render/cuban/cuban.h"

int main(int argc, char* argv[])
{

    WindowSDL HUI = WindowSDL();
    HUI.init(1280, 720);

    auto cuban = new Cuban();
    HUI.setRenderObj(cuban);
    HUI.update();
    HUI.shutdown();

    return 0;
}
#include "render/window/window.h"

int main(int argc, char* argv[])
{

    WindowSDL HUI = WindowSDL();
    HUI.init(1280, 720);
    HUI.update();
    HUI.shutdown();

    return 0;
}
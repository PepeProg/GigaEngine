#include <GLFW/glfw3.h>
#include <gainput/gainput.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>



int testSound() {
    ALCdevice *device;

    device = alcOpenDevice(NULL);
    if (!device) {
        std::cout << "device is not defined, testSound func stopped" << std::endl;
        return -1;
    }

    ALboolean enumeration;

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE) {
        std::cout << "enumeration not supported, testSound func stopped" << std::endl;
        return -1;
    } else {
        std::cout << "enumeration supported, proceed" << std::endl;
    }

    return 0;
}

int testInput() {

    enum Button
    {
        ButtonConfirm
    };

    gainput::InputManager manager;
    manager.SetDisplaySize(640, 480);
    const gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();
    const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
    const gainput::DeviceId padId = manager.CreateDevice<gainput::InputDevicePad>();
    const gainput::DeviceId touchId = manager.CreateDevice<gainput::InputDeviceTouch>();

    gainput::InputMap map(manager);
    map.MapBool(ButtonConfirm, keyboardId, gainput::KeyReturn);
    map.MapBool(ButtonConfirm, mouseId, gainput::MouseButtonLeft);
    map.MapBool(ButtonConfirm, padId, gainput::PadButtonA);
    map.MapBool(ButtonConfirm, touchId, gainput::Touch0Down);

    manager.Update();

    // May need some platform-specific message handling here

    if (map.GetBoolWasDown(ButtonConfirm))
    {
        // Confirmed!
    }

    return 0;
}

int main()
{
//    testInput();
//    testSound();

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);

        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);

        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

#include <GLFW/glfw3.h>
#include <gainput/gainput.h>
#include <AL/al.h>

int main()
{

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
    const ALuint source = 0;
    alSourceRewind(source);
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

        manager.Update();

        // May need some platform-specific message handling here

        if (map.GetBoolWasDown(ButtonConfirm))
        {
            // Confirmed!
        }

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

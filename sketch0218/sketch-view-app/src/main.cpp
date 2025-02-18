#define GLFW_EXPOSE_NATIVE_WIN32
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "sketch-render/adapter/sketch_render.h"
#include "sketch-render/render/window_surface.h"
#include "sketch-render/streaming/memory_streaming.h"
#include "sketch/Interface/IF_App.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/IF_Document.h"
#include "sketch/STCore.h"
#include "sketch/View/ViewManager.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stb_image_write.h>

static bool resize = false;
static int s_width = 0;
static int s_height = 0;

static void on_windows_resize_event([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    sketch::STCore::inst->onViewResize(width, height);
    sketch::STCore::inst->onViewRender();
    resize = true;
    s_width = width;
    s_height = height;
}

static void on_mouse_button_event(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    int width, height;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &width, &height);
    ypos = height - ypos;

    auto ctrl_key_pressed = mods & GLFW_MOD_CONTROL;
    auto alt_key_pressed = mods & GLFW_MOD_ALT;
    auto shift_key_pressed = mods & GLFW_MOD_SHIFT;

    // switch to sketch button
    button = (button == GLFW_MOUSE_BUTTON_LEFT) ? 0 : (button == GLFW_MOUSE_BUTTON_MIDDLE) ? 1 : ((button == GLFW_MOUSE_BUTTON_RIGHT) ? 2 : -1);
    if(action == GLFW_PRESS)
    {
        sketch::ViewManager* viewMgr = sketch::STCore::inst->viewMgr();
        viewMgr->onMouseDown(button, xpos, ypos, ctrl_key_pressed, alt_key_pressed, shift_key_pressed);
    }
    else if(action == GLFW_RELEASE)
    {
        sketch::ViewManager* viewMgr = sketch::STCore::inst->viewMgr();
        viewMgr->onMouseUp(button, xpos, ypos, ctrl_key_pressed, alt_key_pressed, shift_key_pressed);
    }
}

static void on_cursor_pos_event(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    ypos = height - ypos;

    sketch::ViewManager* viewMgr = sketch::STCore::inst->viewMgr();
    auto ctrl_key_pressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    auto alt_key_pressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
    auto shift_key_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    auto left_mouse_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    auto middle_mouse_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    auto right_mouse_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    int button = left_mouse_pressed ? 0 : (middle_mouse_pressed ? 1 : (right_mouse_pressed ? 2 : -1));

    viewMgr->onMouseMove(button, xpos, ypos, ctrl_key_pressed, alt_key_pressed, shift_key_pressed);
}

static void on_keyboard_event([[maybe_unused]] GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, int mods)
{
    auto ctrl_key_pressed = mods & GLFW_MOD_CONTROL;
    auto alt_key_pressed = mods & GLFW_MOD_ALT;
    auto shift_key_pressed = mods & GLFW_MOD_SHIFT;

    if(action == GLFW_PRESS)
    {
        sketch::STCore::inst->_onViewKeyDown(key, ctrl_key_pressed, alt_key_pressed, shift_key_pressed);
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(1280, 720, "Sketch", nullptr, nullptr);
    auto native_handle = glfwGetWin32Window(window);

    glfwSetFramebufferSizeCallback(window, on_windows_resize_event);
    glfwSetMouseButtonCallback(window, on_mouse_button_event);
    glfwSetCursorPosCallback(window, on_cursor_pos_event);
    glfwSetKeyCallback(window, on_keyboard_event);

    auto surface = std::make_unique<sketch::render::WindowSurface>(native_handle);
    auto render = std::make_unique<sketch::render::SketchRender>(std::move(surface));

    sketch::STCore::inst->init_standalone();
    sketch::STCore::inst->set_render(render.get());
    newDocument(L"D:\\test", L"");

    activateController(L"Create_Rectangle");

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        sketch::STCore::inst->tick();
    }

    glfwTerminate();

    return 0;
}

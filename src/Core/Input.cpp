#include "pch.h"
#include "Core/Input.h"

#include "Core/App.h"

bool Input::IsKeyPressed(int keyCode)
{
    GLFWwindow *window = reinterpret_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    int state = glfwGetKey(window, keyCode);

    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(int button)
{
    GLFWwindow *window = reinterpret_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    int state = glfwGetMouseButton(window, button);

    return state == GLFW_PRESS;
}

std::pair<float, float> Input::GetMousePosition()
{
    GLFWwindow *window = reinterpret_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    double xPos;
    double yPos;

    glfwGetCursorPos(window, &xPos, &yPos);

    return { static_cast<float>(xPos), static_cast<float>(yPos) };
}

float Input::GetMouseX()
{
    auto [x, y] = GetMousePosition();

    return x;
}

float Input::GetMouseY()
{
    auto [x, y] = GetMousePosition();

    return y;
}

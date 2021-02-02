#pragma once

/*
 * c++ wrappings for the c glfw library
 */

#include <memory>
#include <utils/glew.hpp>

namespace glfw {
// hack
#include <GLFW/glfw3.h>

namespace helper {
struct DestroyGLFWWin{
  void operator()(GLFWwindow *ptr) {
    glfwDestroyWindow(ptr);
  }
};
}

// smart pointers for glfw structs
typedef std::unique_ptr<GLFWwindow, helper::DestroyGLFWWin> GLFWwindow_ptr;


}
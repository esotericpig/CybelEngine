/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "gl_util.h"

#include "cybel/text/text_util.h"

namespace cybel {

void GlUtil::clear_errors() {
  // Max loop to prevent accidental infinite loop.
  // - https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml
  for(int i = 0; i < 100; ++i) {
    if(glGetError() == GL_NO_ERROR) { break; }
  }
}

std::string GlUtil::fetch_error_str() {
  return fetch_error_str(glGetError());
}

std::string GlUtil::fetch_error_str(GLenum error) {
  // - https://www.khronos.org/opengl/wiki/OpenGL_Error
  switch(error) {
    // v1.0
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";

    // v1.2
    case 0x8031: return "GL_TABLE_TOO_LARGE"; // Removed in v3.1.

    // v3.0
    case 0x0506: return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // v4.5
    case 0x0507: return "GL_CONTEXT_LOST";
  }

  return TextUtil::fmt("GL_UNKNOWN_ERROR[{}]",error);
}

std::string_view GlUtil::glew_error_str(GLenum error) {
  return reinterpret_cast<const char*>(glewGetErrorString(error));
}

} // namespace cybel

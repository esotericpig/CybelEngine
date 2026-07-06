/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_GL_UTIL_H_
#define CYBEL_GFX_GL_UTIL_H_

#include "cybel/common.h"

#include <string>

namespace cybel {

namespace GlUtil {
  void clear_errors();
  std::string fetch_error_str();
  std::string fetch_error_str(GLenum error);

  std::string_view glew_error_str(GLenum error);
}

} // namespace cybel
#endif

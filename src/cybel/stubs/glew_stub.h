/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_STUBS_GLEW_STUB_H_
#define CYBEL_STUBS_GLEW_STUB_H_

#include "cybel/common.h"

#if !defined(GLEW_VERSION)

// NOTE: Not using `constexpr` on purpose to match non-stub usage (e.g., no `if constexpr(...)`).
inline const bool GLEW_VERSION_2_1 = true;
inline const bool GLEW_VERSION_3_0 = true;

inline constexpr GLenum GLEW_OK = 0;

GLenum glewInit();
const GLubyte* glewGetErrorString(GLenum error);

#endif // GLEW_VERSION
#endif

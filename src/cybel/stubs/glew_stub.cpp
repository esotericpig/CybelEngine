/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "glew_stub.h"

#if !defined(GLEW_VERSION)

GLenum glewInit() { return GLEW_OK; }

const GLubyte* glewGetErrorString(GLenum /*error*/) { return reinterpret_cast<const GLubyte*>(""); }

#endif // GLEW_VERSION

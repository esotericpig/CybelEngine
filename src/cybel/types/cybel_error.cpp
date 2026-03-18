/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "cybel_error.h"

namespace cybel {

CybelError::CybelError(std::string_view msg)
  : msg_{msg} {}

const char* CybelError::what() const noexcept { return msg_.c_str(); }

} // namespace cybel

/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "cybel_error.h"

namespace cybel {

CybelError::CybelError()
  : std::runtime_error{"Unknown error."} {}

CybelError::CybelError(std::string_view msg)
  : std::runtime_error{std::string{msg}} {}

} // namespace cybel

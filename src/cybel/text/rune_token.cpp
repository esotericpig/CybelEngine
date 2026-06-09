/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "rune_token.h"

namespace cybel {

std::string RuneToken::to_str() const {
  return std::string{view};
}

} // namespace cybel

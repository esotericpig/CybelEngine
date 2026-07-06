/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_CYBEL_ERROR_H_
#define CYBEL_TYPES_CYBEL_ERROR_H_

#include "cybel/common.h"

#include "cybel/text/text_util.h"

#include <format>
#include <stdexcept>
#include <string_view>

namespace cybel {

class CybelError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;

  explicit CybelError();
  explicit CybelError(std::string_view msg);

  template <typename... Args>
  explicit CybelError(std::format_string<Args...> fmt_str,Args&&... args);
};

template <typename... Args>
CybelError::CybelError(std::format_string<Args...> fmt_str,Args&&... args)
  : std::runtime_error{TextUtil::fmt(fmt_str,std::forward<Args>(args)...)} {}

} // namespace cybel
#endif

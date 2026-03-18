/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_CYBEL_ERROR_H_
#define CYBEL_TYPES_CYBEL_ERROR_H_

#include "cybel/common.h"

#include "cybel/util/util.h"

namespace cybel {

class CybelError : public std::exception {
public:
  explicit CybelError() = default;
  explicit CybelError(std::string_view msg);

  template <typename... MsgArgs>
  explicit CybelError(const MsgArgs&... msg_args)
    : msg_(Util::build_str(msg_args...)) {}

  const char* what() const noexcept override;

private:
  std::string msg_{};
};

} // namespace cybel
#endif

/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_ID_CONCEPTS_H_
#define CYBEL_TYPES_ID_CONCEPTS_H_

#include "cybel/common.h"

namespace cybel {

template <typename T,typename IdT>
concept IdLike = requires(T t) {
  static_cast<IdT>(t);
};

} // namespace cybel
#endif

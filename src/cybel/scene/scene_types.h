/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_SCENE_SCENE_TYPES_H_
#define CYBEL_SCENE_SCENE_TYPES_H_

#include "cybel/common.h"

#include "cybel/types/id_concepts.h"

namespace cybel {

using scene_id_t = std::size_t;

template <typename T>
concept SceneIdLike = IdLike<T,scene_id_t>;

} // namespace cybel
#endif

/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_SCENE_SCENE_BAG_H_
#define CYBEL_SCENE_SCENE_BAG_H_

#include "cybel/common.h"

#include "cybel/scene/scene.h"
#include "cybel/scene/scene_types.h"

namespace cybel {

class SceneBag final {
public:
  static const SceneBag kEmpty;

  scene_id_t id{};
  std::shared_ptr<Scene> scene{};
  bool persist = false;

  auto operator<=>(const SceneBag& other) const = default;
  explicit operator bool() const;
  Scene* operator->() const;
  Scene& operator*() const;
};

inline const SceneBag SceneBag::kEmpty{
  .scene = std::make_shared<Scene>(), // Current scene should never be null.
  .persist = true,
};

} // namespace cybel
#endif

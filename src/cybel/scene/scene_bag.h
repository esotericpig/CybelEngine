/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_SCENE_SCENE_BAG_H_
#define CYBEL_SCENE_SCENE_BAG_H_

#include "cybel/common.h"

namespace cybel {

class Scene;

class SceneBag final {
public:
  /// Do not use this for your own scene types, else they'll be ignored.
  static constexpr int kTypeNone = 0;
  static const SceneBag kEmpty;

  int type = kTypeNone;
  std::shared_ptr<Scene> scene{};
  bool persist = false;

  explicit operator bool() const;
  Scene* operator->() const;
  Scene& operator*() const;
};

} // namespace cybel
#endif

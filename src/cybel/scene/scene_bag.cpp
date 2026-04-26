/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "scene_bag.h"

namespace cybel {

const SceneBag SceneBag::kEmpty{
  .type = Scene::kNilType,
  .scene = std::make_shared<Scene>(), // Current scene should never be null.
  .persist = true,
};

SceneBag::operator bool() const { return static_cast<bool>(scene); }

Scene* SceneBag::operator->() const { return scene.get(); }

Scene& SceneBag::operator*() const { return *scene; }

} // namespace cybel

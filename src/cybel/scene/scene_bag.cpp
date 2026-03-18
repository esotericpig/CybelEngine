/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "scene_bag.h"

namespace cybel {

SceneBag::SceneBag(int type,std::shared_ptr<Scene> scene,bool persist)
  : type(type),scene(std::move(scene)),persist(persist) {}

SceneBag::operator bool() const { return static_cast<bool>(scene); }

Scene* SceneBag::operator->() const { return scene.get(); }

Scene& SceneBag::operator*() const { return *scene; }

} // namespace cybel

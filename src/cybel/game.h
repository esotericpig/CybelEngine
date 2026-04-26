/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GAME_H_
#define CYBEL_GAME_H_

#include "cybel/common.h"

#include "cybel/scene/scene.h"
#include "cybel/scene/scene_bag.h"

namespace cybel {

class Game : public Scene {
public:
  virtual void on_game_start() {}
  virtual SceneBag build_scene([[maybe_unused]] int type) { return SceneBag{}; }
};

} // namespace cybel
#endif

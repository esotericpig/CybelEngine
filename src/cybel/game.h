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
#include "cybel/scene/scene_types.h"

namespace cybel {

class CybelEngine;

class Game : public Scene {
public:
  virtual void on_game_start(CybelEngine& engine,SceneContext& ctx);

  virtual SceneBag build_scene(scene_id_t id,SceneContext& ctx);
};

inline void Game::on_game_start(CybelEngine&,SceneContext&) {}

inline SceneBag Game::build_scene(scene_id_t,SceneContext&) {
  return SceneBag{};
}

} // namespace cybel
#endif

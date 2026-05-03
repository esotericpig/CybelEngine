/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_SCENE_SCENE_CONTEXT_H_
#define CYBEL_SCENE_SCENE_CONTEXT_H_

#include "cybel/common.h"

#include "cybel/cybel_engine.h"
#include "cybel/audio/audio_player.h"
#include "cybel/scene/scene_man.h"
#include "cybel/types/view_dimens.h"

namespace cybel {

class SceneContext final {
public:
  CybelEngine& engine;
  const ViewDimens& dimens;
  SceneMan& scene_man;
  AudioPlayer& audio_player;

  explicit SceneContext(CybelEngine& engine)
    : engine{engine},
      dimens{engine.renderer().dimens()},
      scene_man{engine.scene_man()},
      audio_player{engine.audio_player()} {}
};

} // namespace cybel
#endif

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
#include "cybel/asset/asset_man.h"
#include "cybel/audio/audio_player.h"
#include "cybel/scene/scene_man.h"
#include "cybel/types/view_dimens.h"

namespace cybel {

struct SceneContext final {
  CybelEngine& engine;
  SceneMan& scenes;
  AssetMan& assets;
  const ViewDimens& dimens;
  AudioPlayer& audio;
};

} // namespace cybel
#endif

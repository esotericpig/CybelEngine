/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_SCENE_SCENE_H_
#define CYBEL_SCENE_SCENE_H_

#include "cybel/common.h"

#include "cybel/gfx/renderer.h"
#include "cybel/input/input_man.h"
#include "cybel/input/input_types.h"
#include "cybel/types/frame_step.h"

namespace cybel {

class SceneContext;

class Scene {
public:
  explicit Scene() = default;
  virtual ~Scene() noexcept = default;

  virtual void on_scene_enter([[maybe_unused]] SceneContext& ctx) {}
  /// Called when this scene is no longer the current scene (i.e., scene changed).
  /// Should not delete objects in here (i.e., not a destructor).
  virtual void on_scene_exit([[maybe_unused]] SceneContext& ctx) {}

  /// If you know that all of your scenes will only either be in 2D or 3D, then you can call
  /// Renderer.begin_2d_scene()/begin_3d_scene() in here, instead of in the main loop with draw_scene().
  ///
  /// This also applies to: begin_*center(), begin_*scale(), begin_*offset(), etc.
  virtual void on_scene_resize([[maybe_unused]] Renderer& ren,[[maybe_unused]] SceneContext& ctx) {}

  /// On WebGL context lost, zombify textures, etc.
  virtual void on_scene_context_loss([[maybe_unused]] SceneContext& ctx) {}
  /// On WebGL context restored, reload textures, re-init WebGL states, etc.
  virtual void on_scene_context_restore([[maybe_unused]] SceneContext& ctx) {}

  virtual void on_scene_input_event([[maybe_unused]] input_id_t input_id,
                                    [[maybe_unused]] SceneContext& ctx) {}
  virtual void handle_scene_input([[maybe_unused]] const InputStates& states,
                                  [[maybe_unused]] InputMan& input,
                                  [[maybe_unused]] SceneContext& ctx) {}

  virtual void update_scene_logic([[maybe_unused]] const FrameStep& step,
                                  [[maybe_unused]] SceneContext& ctx) {}
  virtual void draw_scene([[maybe_unused]] Renderer& ren,[[maybe_unused]] SceneContext& ctx) {}

protected:
  Scene(const Scene& other) = default;
  Scene(Scene&& other) noexcept = default;

  Scene& operator=(const Scene& other) = default;
  Scene& operator=(Scene&& other) noexcept = default;
};

} // namespace cybel
#endif

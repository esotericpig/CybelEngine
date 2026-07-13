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

struct SceneContext;

class Scene {
public:
  explicit Scene() = default;
  virtual ~Scene() noexcept = default;

  virtual void on_scene_enter(SceneContext& ctx);
  /// Called when this scene is no longer the current scene (i.e., scene changed).
  /// Should not delete objects in here (i.e., not a destructor).
  virtual void on_scene_exit(SceneContext& ctx);

  /// If you know that all of your scenes will only either be in 2D or 3D, then you can call
  /// Renderer.begin_2d_scene()/begin_3d_scene() in here, instead of in the main loop with draw_scene().
  ///
  /// This also applies to: begin_*center(), begin_*scale(), begin_*offset(), etc.
  virtual void on_scene_resize(Renderer& ren,SceneContext& ctx);

  /// On WebGL context lost, zombify textures, etc.
  virtual void on_scene_gpu_context_loss(SceneContext& ctx);
  /// On WebGL context restored, reload textures, re-init WebGL states, etc.
  virtual void on_scene_gpu_context_restore(SceneContext& ctx);

  virtual void on_scene_input_event(input_id_t input_id,SceneContext& ctx);
  virtual void handle_scene_input(InputMan& input,SceneContext& ctx);

  virtual void update_scene_logic(const FrameStep& step,SceneContext& ctx);
  virtual void draw_scene(Renderer& ren,SceneContext& ctx);

protected:
  Scene(const Scene& other) = default;
  Scene& operator=(const Scene& other) = default;

  Scene(Scene&& other) noexcept = default;
  Scene& operator=(Scene&& other) noexcept = default;
};

inline void Scene::on_scene_enter(SceneContext&) {}

inline void Scene::on_scene_exit(SceneContext&) {}

inline void Scene::on_scene_resize(Renderer&,SceneContext&) {}

inline void Scene::on_scene_gpu_context_loss(SceneContext&) {}

inline void Scene::on_scene_gpu_context_restore(SceneContext&) {}

inline void Scene::on_scene_input_event(input_id_t,SceneContext&) {}

inline void Scene::handle_scene_input(InputMan&,SceneContext&) {}

inline void Scene::update_scene_logic(const FrameStep&,SceneContext&) {}

inline void Scene::draw_scene(Renderer&,SceneContext&) {}

} // namespace cybel
#endif

/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_SCENE_SCENE_MAN_H_
#define CYBEL_SCENE_SCENE_MAN_H_

#include "cybel/common.h"

#include "cybel/scene/scene_bag.h"
#include "cybel/scene/scene_types.h"

#include <functional>
#include <vector>

namespace cybel {

class CybelEngine;
class Scene;

/// Only one scene action can occur per frame and first one wins.
class SceneMan final {
public:
  using BuildScene = std::function<SceneBag(scene_id_t id)>;
  using OnSceneChange = std::function<void(Scene&)>;

  explicit SceneMan(BuildScene build_scene,OnSceneChange on_scene_enter,OnSceneChange on_scene_exit);

  template <SceneIdLike T>
  bool push_scene(T id_like);
  bool pop_scene();
  bool pop_all_scenes();
  bool restart_scene();

  Scene& curr_scene() const;
  scene_id_t curr_scene_id() const;
  const std::vector<SceneBag>& prev_scene_bags() const;

  friend class CybelEngine;

private:
  enum class Action {
    kNone,
    kPushScene,
    kPopScene,
    kPopAllScenes,
    kRestartScene,
  };

  BuildScene build_scene_{};
  OnSceneChange on_scene_enter_{};
  OnSceneChange on_scene_exit_{};

  SceneBag curr_scene_bag_ = SceneBag::kEmpty;
  std::vector<SceneBag> prev_scene_bags_{};

  Action pending_action_ = Action::kNone;
  SceneBag pending_scene_bag_{};

  void commit_pending();
  void commit_push_scene();
  void commit_pop_scene();
  void commit_pop_all_scenes();
  void commit_restart_scene();
  void cancel_pending();

  void set_scene(SceneBag scene_bag);
};

template <SceneIdLike T>
bool SceneMan::push_scene(T id_like) {
  const auto id = static_cast<scene_id_t>(id_like);

  if(pending_action_ != Action::kNone) {
    return pending_action_ == Action::kPushScene && pending_scene_bag_.id == id;
  }
  if(id == SceneBag::kIdNone) { return false; }

  pending_scene_bag_ = build_scene_(id);
  if(!pending_scene_bag_.scene) { return false; }

  pending_action_ = Action::kPushScene;
  return true;
}

} // namespace cybel
#endif

/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "scene_man.h"

#include "cybel/types/cybel_error.h"

namespace cybel {

SceneMan::SceneMan(const BuildScene& build_scene,const OnSceneChange& on_scene_enter,
                   const OnSceneChange& on_scene_exit)
  : build_scene_{build_scene},
    on_scene_enter_{on_scene_enter},
    on_scene_exit_{on_scene_exit} {
  if(!build_scene_) { throw CybelError{"BuildScene is null."}; }
  if(!on_scene_enter_) { throw CybelError{"OnSceneEnter is null."}; }
  if(!on_scene_exit_) { throw CybelError{"OnSceneExit is null."}; }
}

bool SceneMan::push_scene(int type) {
  if(action_ != Action::kNone) { return action_ == Action::kPushScene && next_scene_bag_.type == type; }
  if(type == SceneBag::kTypeNone) { return false; }

  next_scene_bag_ = build_scene_(type);
  if(!next_scene_bag_.scene) { return false; }

  action_ = Action::kPushScene;
  return true;
}

bool SceneMan::pop_scene() {
  if(action_ != Action::kNone) { return action_ == Action::kPopScene; }

  for(std::size_t i = prev_scene_bags_.size(); i-- > 0;) {
    SceneBag& prev_bag = prev_scene_bags_[i];
    if(prev_bag.type == SceneBag::kTypeNone) { continue; }

    // Not persisted? (i.e., need to recreate)
    if(!prev_bag.scene) {
      SceneBag new_bag = build_scene_(prev_bag.type);
      if(!new_bag.scene) { continue; }

      prev_bag = std::move(new_bag);
    }

    // Success.
    action_ = Action::kPopScene;
    return true;
  }

  return false;
}

bool SceneMan::pop_all_scenes() {
  if(action_ != Action::kNone) { return action_ == Action::kPopAllScenes; }
  if(prev_scene_bags_.empty()) { return true; }

  action_ = Action::kPopAllScenes;
  return true;
}

bool SceneMan::restart_scene() {
  if(action_ != Action::kNone) {
    return action_ == Action::kRestartScene && next_scene_bag_.type == curr_scene_bag_.type;
  }

  next_scene_bag_ = build_scene_(curr_scene_bag_.type);

  if(!next_scene_bag_.scene) {
    // If we can't restart the scene, then we need to pop this scene off instead.
    std::cerr << "[ERROR] Failed to restart scene [" << curr_scene_bag_.type
              << "]; popping the scene off instead." << std::endl;
    pop_scene();

    return false;
  }

  action_ = Action::kRestartScene;
  return true;
}

void SceneMan::commit() {
  switch(action_) {
    case Action::kNone: return;

    case Action::kPushScene:
      commit_push_scene();
      break;
    case Action::kPopScene:
      commit_pop_scene();
      break;
    case Action::kPopAllScenes:
      commit_pop_all_scenes();
      break;
    case Action::kRestartScene:
      commit_restart_scene();
      break;
  }

  cancel();
}

void SceneMan::commit_push_scene() {
  if(!next_scene_bag_.scene) { return; }

  SceneBag prev_bag = curr_scene_bag_;
  set_scene(std::move(next_scene_bag_)); // `next_scene_bag_` is safely reset in cancel() in commit().

  // Don't store Empty scene.
  if(prev_bag.type != SceneBag::kTypeNone) {
    if(!prev_bag.persist) { prev_bag.scene = nullptr; }
    prev_scene_bags_.push_back(std::move(prev_bag));
  }
}

void SceneMan::commit_pop_scene() {
  // Avoid setting scene to Empty scene over & over.
  if(prev_scene_bags_.empty()) { return; }

  do {
    SceneBag prev_bag = std::move(prev_scene_bags_.back());
    prev_scene_bags_.pop_back();

    if(prev_bag.type == SceneBag::kTypeNone) { continue; }

    // Not persisted? (i.e., need to recreate)
    if(!prev_bag.scene) {
      prev_bag = build_scene_(prev_bag.type);
      if(!prev_bag.scene) { continue; }
    }

    // Success.
    set_scene(std::move(prev_bag));
    return;
  } while(!prev_scene_bags_.empty());

  set_scene(SceneBag::kEmpty);
}

void SceneMan::commit_pop_all_scenes() {
  if(prev_scene_bags_.empty()) { return; }

  prev_scene_bags_.clear();
  set_scene(SceneBag::kEmpty);
}

void SceneMan::commit_restart_scene() {
  if(!next_scene_bag_.scene) { return; }

  set_scene(std::move(next_scene_bag_)); // `next_scene_bag_` is safely reset in cancel() in commit().
}

void SceneMan::cancel() {
  action_ = Action::kNone;
  next_scene_bag_ = SceneBag{};
}

void SceneMan::set_scene(SceneBag scene_bag) {
  if(!scene_bag.scene) { throw CybelError{"Scene is null."}; }

  on_scene_exit_(*curr_scene_bag_.scene);
  curr_scene_bag_ = std::move(scene_bag);
  on_scene_enter_(*curr_scene_bag_.scene);
}

Scene& SceneMan::curr_scene() const { return *curr_scene_bag_.scene; }

int SceneMan::curr_scene_type() const { return curr_scene_bag_.type; }

const std::vector<SceneBag>& SceneMan::prev_scene_bags() const { return prev_scene_bags_; }

} // namespace cybel

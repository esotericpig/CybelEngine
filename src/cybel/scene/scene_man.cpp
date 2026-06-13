/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "scene_man.h"

#include "cybel/scene/scene.h"
#include "cybel/types/cybel_error.h"

namespace cybel {

SceneBag SceneMan::make_empty_scene_bag() {
  return SceneBag{
    .scene = std::make_shared<Scene>(), // Current scene should never be null.
    .persist = true,
  };
}

SceneMan::SceneMan(BuildScene build_scene,OnSceneChange on_scene_enter,OnSceneChange on_scene_exit)
  : build_scene_{std::move(build_scene)},
    on_scene_enter_{std::move(on_scene_enter)},
    on_scene_exit_{std::move(on_scene_exit)} {
  if(!build_scene_) { throw CybelError{"`build_scene` is empty."}; }
  if(!on_scene_enter_) { throw CybelError{"`on_scene_enter` is empty."}; }
  if(!on_scene_exit_) { throw CybelError{"`on_scene_exit` is empty."}; }
}

bool SceneMan::pop_scene() {
  if(prev_scene_bags_.empty() && curr_scene_bag_ == kEmptySceneBag) { return true; }
  if(pending_action_ != Action::kNone) { return pending_action_ == Action::kPopScene; }

  for(std::size_t i = prev_scene_bags_.size(); i-- > 0;) {
    SceneBag& prev_bag = prev_scene_bags_[i];
    if(prev_bag == kEmptySceneBag) { continue; }

    // Not persisted? (i.e., need to recreate)
    if(!prev_bag.scene) {
      SceneBag new_bag = build_scene_(prev_bag.id);
      if(!new_bag.scene || new_bag == kEmptySceneBag) { continue; }

      prev_bag = std::move(new_bag);
    }

    // Success.
    pending_action_ = Action::kPopScene;
    return true;
  }

  pending_action_ = Action::kPopAllScenes;
  return true;
}

bool SceneMan::pop_all_scenes() {
  if(prev_scene_bags_.empty() && curr_scene_bag_ == kEmptySceneBag) { return true; }
  if(pending_action_ != Action::kNone) { return pending_action_ == Action::kPopAllScenes; }

  pending_action_ = Action::kPopAllScenes;
  return true;
}

bool SceneMan::restart_scene() {
  if(curr_scene_bag_ == kEmptySceneBag) { return true; }

  if(pending_action_ != Action::kNone) {
    return pending_action_ == Action::kRestartScene && pending_scene_bag_.id == curr_scene_bag_.id;
  }

  pending_scene_bag_ = build_scene_(curr_scene_bag_.id);

  if(!pending_scene_bag_.scene || pending_scene_bag_ == kEmptySceneBag) {
    // If we can't restart the scene, then we need to pop this scene off instead.
    std::cerr << "[ERROR] Failed to restart Scene `" << curr_scene_bag_.id
              << "`; popping this Scene off instead." << std::endl;
    pop_scene();

    return false;
  }

  pending_action_ = Action::kRestartScene;
  return true;
}

void SceneMan::commit_pending() {
  switch(pending_action_) {
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

  cancel_pending(); // Reset.
}

void SceneMan::commit_push_scene() {
  if(!pending_scene_bag_.scene || pending_scene_bag_ == kEmptySceneBag) { return; }

  // NOTE: Don't std::move(curr_scene_bag_) since set_curr_scene() needs to call on_scene_exit_().
  SceneBag prev_bag = curr_scene_bag_;
  set_curr_scene(std::move(pending_scene_bag_));

  if(prev_bag != kEmptySceneBag) {
    if(!prev_bag.persist) { prev_bag.scene = nullptr; }
    prev_scene_bags_.push_back(std::move(prev_bag));
  }
}

void SceneMan::commit_pop_scene() {
  // Avoid setting the current scene to Empty over & over.
  if(prev_scene_bags_.empty() && curr_scene_bag_ == kEmptySceneBag) { return; }

  while(!prev_scene_bags_.empty()) {
    SceneBag prev_bag = std::move(prev_scene_bags_.back());
    prev_scene_bags_.pop_back();

    if(prev_bag == kEmptySceneBag) { continue; }

    // Not persisted? (i.e., need to recreate)
    if(!prev_bag.scene) {
      prev_bag = build_scene_(prev_bag.id);
      if(!prev_bag.scene || prev_bag == kEmptySceneBag) { continue; }
    }

    // Success.
    set_curr_scene(std::move(prev_bag));
    return;
  }

  set_curr_scene(kEmptySceneBag);
}

void SceneMan::commit_pop_all_scenes() {
  if(prev_scene_bags_.empty() && curr_scene_bag_ == kEmptySceneBag) { return; }

  prev_scene_bags_.clear();
  set_curr_scene(kEmptySceneBag);
}

void SceneMan::commit_restart_scene() {
  if(!pending_scene_bag_.scene || pending_scene_bag_ == kEmptySceneBag) { return; }

  set_curr_scene(std::move(pending_scene_bag_));
}

void SceneMan::cancel_pending() {
  pending_action_ = Action::kNone;
  pending_scene_bag_ = SceneBag{};
}

void SceneMan::set_curr_scene(SceneBag new_scene_bag) {
  if(!new_scene_bag.scene) { throw CybelError{"`new_scene_bag.scene` is null."}; }

  on_scene_exit_(*curr_scene_bag_.scene);
  curr_scene_bag_ = std::move(new_scene_bag);
  on_scene_enter_(*curr_scene_bag_.scene);
}

Scene& SceneMan::curr_scene() const { return *curr_scene_bag_.scene; }

scene_id_t SceneMan::curr_scene_id() const { return curr_scene_bag_.id; }

bool SceneMan::has_prev_scenes() const { return !prev_scene_bags_.empty(); }

const std::vector<SceneBag>& SceneMan::prev_scene_bags() const { return prev_scene_bags_; }

} // namespace cybel

/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "scene_man.h"

#include "cybel/types/cybel_error.h"

namespace cybel {

SceneMan::SceneMan(const BuildScene& build_scene,const OnSceneChange& on_scene_change)
  : build_scene_{build_scene},
    on_scene_change_{on_scene_change} {
  if(!build_scene_) { throw CybelError{"BuildScene is null."}; }
  if(!on_scene_change_) { throw CybelError{"OnSceneChange is null."}; }
}

bool SceneMan::push_scene(int type) {
  if(type == Scene::kNilType) { return false; }

  SceneBag scene = build_scene_(type);
  if(!scene.scene) { return false; }

  SceneBag prev = curr_scene_bag_;
  set_scene(std::move(scene));

  // Don't push empty scene.
  if(prev.type != Scene::kNilType) {
    if(!prev.persist) { prev.scene = nullptr; }
    prev_scene_bags_.push_back(std::move(prev));
  }

  return true;
}

bool SceneMan::pop_scene() {
  // Avoid setting scene to empty scene over & over.
  if(prev_scene_bags_.empty()) { return false; }

  do {
    SceneBag prev = std::move(prev_scene_bags_.back());
    prev_scene_bags_.pop_back();

    if(prev.type == Scene::kNilType) { continue; }

    // Not persisted? (i.e., need to recreate)
    if(!prev.scene) {
      prev = build_scene_(prev.type);
      if(!prev.scene) { continue; }
    }

    set_scene(std::move(prev));
    return true; // Success.
  } while(!prev_scene_bags_.empty());

  set_scene(SceneBag::kEmpty);

  return false;
}

void SceneMan::pop_all_scenes() {
  if(prev_scene_bags_.empty()) { return; }

  prev_scene_bags_.clear();
  set_scene(SceneBag::kEmpty);
}

bool SceneMan::restart_scene() {
  SceneBag scene_bag = build_scene_(curr_scene_bag_.type);
  if(!scene_bag.scene) { return false; }

  set_scene(std::move(scene_bag));

  return true;
}

void SceneMan::set_scene(SceneBag scene_bag) {
  if(!scene_bag.scene) { throw CybelError{"Scene is null."}; }

  curr_scene_bag_->on_scene_exit();
  curr_scene_bag_ = std::move(scene_bag);

  on_scene_change_(*curr_scene_bag_.scene);
}

Scene& SceneMan::curr_scene() const { return *curr_scene_bag_.scene; }

int SceneMan::curr_scene_type() const { return curr_scene_bag_.type; }

const std::vector<SceneBag>& SceneMan::prev_scene_bags() const { return prev_scene_bags_; }

} // namespace cybel

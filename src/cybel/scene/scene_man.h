/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_SCENE_SCENE_MAN_H_
#define CYBEL_SCENE_SCENE_MAN_H_

#include "cybel/common.h"

#include "cybel/scene/scene.h"
#include "cybel/scene/scene_bag.h"

#include <functional>
#include <vector>

namespace cybel {

class SceneMan {
public:
  using BuildScene = std::function<SceneBag(int type)>;
  using OnSceneChange = std::function<void(Scene&)>;

  explicit SceneMan(const BuildScene& build_scene,const OnSceneChange& on_scene_change);

  bool push_scene(int type);
  bool pop_scene();
  void pop_all_scenes();
  bool restart_scene();

  Scene& curr_scene() const;
  int curr_scene_type() const;
  const std::vector<SceneBag>& prev_scene_bags() const;

private:
  BuildScene build_scene_{};
  OnSceneChange on_scene_change_{};

  SceneBag curr_scene_bag_ = SceneBag::kEmpty;
  std::vector<SceneBag> prev_scene_bags_{};

  void set_scene(SceneBag scene_bag);
};

} // namespace cybel
#endif

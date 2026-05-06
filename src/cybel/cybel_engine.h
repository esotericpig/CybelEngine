/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_CYBEL_ENGINE_H_
#define CYBEL_CYBEL_ENGINE_H_

#include "cybel/common.h"

#include "cybel/game.h"
#include "cybel/audio/audio_player.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/renderer.h"
#include "cybel/input/input_man.h"
#include "cybel/scene/scene.h"
#include "cybel/scene/scene_man.h"
#include "cybel/types/color.h"
#include "cybel/types/duration.h"
#include "cybel/types/frame_step.h"
#include "cybel/types/size.h"
#include "cybel/types/view_dimens.h"
#include "cybel/util/timer.h"

namespace cybel {

class CybelEngine final {
  // NOTE: The variables in this section must be defined first so that their dtors are called last.

  /// This is necessary for RAII, since CybelEngine() ctor can throw an exception.
  /// I decided to do this over using `unique_ptr`s or individual wrappers.
  class Platform final {
  public:
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

    explicit Platform() noexcept = default;
    Platform(const Platform& other) = delete;
    Platform(Platform&& other) noexcept = delete;
    ~Platform() noexcept;

    Platform& operator=(const Platform& other) = delete;
    Platform& operator=(Platform&& other) noexcept = delete;
  } plat_{}; // Chris Pratt?

  std::unique_ptr<AudioPlayer> audio_player_{};

public:
  struct Config {
    std::string title{};
    float scale_factor = 0.0f;
    Size2i size{kFallbackWidth,kFallbackHeight};
    Size2i target_size{0,0};

    int fps = kFallbackFps;
    bool vsync = false;
    Color4f clear_color{0.0f,1.0f};

    input_id_t max_input_id = 0;

    /// All:
    ///   IMG_INIT_AVIF | IMG_INIT_JPG  | IMG_INIT_JXL | IMG_INIT_PNG |
    ///   IMG_INIT_TIF  | IMG_INIT_WEBP
    ///
    /// See: https://wiki.libsdl.org/SDL2_image/IMG_Init
    int image_types = IMG_INIT_PNG;

    /// All:
    ///   MIX_INIT_FLAC | MIX_INIT_MID  | MIX_INIT_MOD     | MIX_INIT_MP3 |
    ///   MIX_INIT_OGG  | MIX_INIT_OPUS | MIX_INIT_WAVPACK
    ///
    /// For MIDI on Linux, need to install:
    ///   timidity++ libtimidity-devel
    ///
    /// See: https://wiki.libsdl.org/SDL2_mixer/Mix_Init
    int music_types = MIX_INIT_OGG;
  };

  static constexpr int kFallbackWidth = 1600;
  static constexpr int kFallbackHeight = 900;
  static constexpr int kFallbackFps = 60;

  static CybelEngine& init(const Config& config);

  CybelEngine(const CybelEngine& other) = delete;
  CybelEngine(CybelEngine&& other) noexcept = delete;
  ~CybelEngine() noexcept = default;

  CybelEngine& operator=(const CybelEngine& other) = delete;
  CybelEngine& operator=(CybelEngine&& other) noexcept = delete;

  void run(std::unique_ptr<Game> game);
  void request_stop();
  void nav_back_in_web();

  void sync_size(bool force = true);
  void resize(const Size2i& size,bool force = true);

  void show_error(const std::string& error) const;
  void show_error(const std::string& title,const std::string& error) const;
  static void show_error_no_window(const std::string& title,const std::string& error);

  void set_icon(const Image& img);
  void set_title(const std::string& title);
  void reset_title();
  void set_fullscreen(bool fullscreen,bool windowed = true);
  void set_cursor_visible(bool visible);
  void set_vsync(bool enable);
  void set_logic_running(bool running);

  const std::string& title() const;
  bool is_fullscreen() const;
  bool is_cursor_visible() const;
  bool is_vsync() const;
  bool is_logic_running() const;

  Renderer& renderer();
  InputMan& input_man();
  AudioPlayer& audio_player();
  Game& game();
  SceneMan& scene_man();

  const ViewDimens& dimens() const;
  int target_fps() const;
  const Duration& target_dpf() const;
  const Duration& dpf() const;
  double delta_time() const;
  float avg_fps() const;

private:
  std::string title_{};
  int target_fps_ = 0;
  Duration target_dpf_{};
  float avg_fps_{};
  bool is_vsync_ = false;

  std::unique_ptr<Renderer> renderer_{};
  std::unique_ptr<InputMan> input_man_{};
  std::unique_ptr<SceneContext> scene_ctx_{};
  std::unique_ptr<Game> game_ = std::make_unique<Game>();
  SceneMan scene_man_{
    [this](int type) { return build_scene(type); },
    [this](Scene& scene) { on_scene_enter(scene); },
    [this](Scene& scene) { on_scene_exit(scene); },
  };

  bool is_running_ = false;
  bool is_logic_running_ = true;
  Timer frame_timer_{};
  FrameStep frame_step_{};

  explicit CybelEngine(Config config);

  void init_hints();
  void init_config(Config& config);
  static Size2i calc_scaled_view(const Size2i& view,float scale_factor,const Size2i& target_size);
  void init_gui(const Config& config);
  void init_context();
  void check_versions();

  bool run_frame();

#if defined(__EMSCRIPTEN__)
  static void run_web_frame(void* user_data);
  static bool on_webgl_context_change(int event_type,const void* reserved,void* user_data);
#endif

  void on_context_loss();
  void on_context_restore();

  SceneBag build_scene(int type);
  void on_scene_enter(Scene& scene);
  void on_scene_exit(Scene& scene);

  void start_frame_timer();
  void stop_frame_timer();
  void handle_events();
  void handle_non_context_events_only();
  void on_input_event(input_id_t input_id);
  void handle_input();

  static void show_error(const std::string& title,const std::string& error,SDL_Window* window);
};

} // namespace cybel
#endif

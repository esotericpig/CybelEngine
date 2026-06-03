/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "cybel_engine.h"

#include "cybel/gfx/gpu_context_key.h"
#include "cybel/metrics/metric_man.h"
#include "cybel/scene/scene_context.h"
#include "cybel/str/utf8/str_util.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

#if defined(CYBEL_RENDERER_GLES)
  #include "cybel/gfx/renderer_gles.h"
#else // CYBEL_RENDERER_GL
  #include "cybel/gfx/renderer_gl.h"
#endif

namespace cybel {

CybelEngine& CybelEngine::init(const Config& config) {
  static CybelEngine engine{config};

  // Not initialized?
  if(!is_init_.exchange(true)) {
#if defined(__EMSCRIPTEN__)
    emscripten_set_webglcontextlost_callback("#canvas",&engine,false,on_webgl_context_change);
    emscripten_set_webglcontextrestored_callback("#canvas",&engine,false,on_webgl_context_change);
    emscripten_cancel_main_loop();
    emscripten_set_main_loop_arg(run_web_frame,&engine,0,false);
#endif
  } else {
#if !defined(NDEBUG)
    std::cerr << "[WARN] CybelEngine::" << __func__ << "() called again." << std::endl;
#endif
  }

  return engine;
}

CybelEngine::CybelEngine(Config config)
  : title_{config.title},
    is_vsync_{config.vsync} {
  init_hints();

  // Don't use SDL_INIT_AUDIO here, since audio is optional.
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    throw CybelError{"Failed to init SDL: ",Util::get_sdl_error(),'.'};
  }
  if(IMG_Init(config.image_types) == 0) {
    throw CybelError{"Failed to init SDL_image: ",Util::get_sdl_img_error(),'.'};
  }

  init_config(config);
  init_gui(config);
  init_gpu_context();
  check_versions();

  audio_player_ = std::make_unique<AudioPlayer>(config.music_types);
  file_sys_ = std::unique_ptr<FileSys>(new FileSys{});
  asset_man_ = std::make_unique<AssetMan>(audio_player_->is_alive());

#if defined(CYBEL_RENDERER_GLES)
  renderer_ = std::make_unique<RendererGles>(config.size,config.target_size,config.clear_color);
#else // CYBEL_RENDERER_GL
  renderer_ = std::make_unique<RendererGl>(config.size,config.target_size,config.clear_color);
#endif

  input_man_ = std::make_unique<InputMan>([this](auto id) { on_input_event(id); });

  scene_ctx_ = std::unique_ptr<SceneContext>(new SceneContext{
    .engine = *this,
    .scenes = scene_man_,
    .assets = *asset_man_,
    .dimens = renderer_->dimens(),
    .audio = *audio_player_,
  });
}

void CybelEngine::init_hints() {
  //SDL_SetHint(SDL_HINT_APP_NAME,title_.c_str());
  SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME,title_.c_str());

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"nearest"); // One of: nearest, linear, best.
  //SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING,"1");

  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
  //SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS,"1");
  //SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,"0");
}

void CybelEngine::init_config(Config& config) {
  Size2i size = config.size;

  // If getting the current display mode fails, we fall back to Config.size.
  // Therefore, scale_factor needs to have priority over Config.size.
  if(config.scale_factor > 0.0f) {
    SDL_DisplayMode dm{};

    if(SDL_GetCurrentDisplayMode(0,&dm) != 0) {
      std::cerr << "[WARN] Failed to get current display mode: " << Util::get_sdl_error() << '.' << std::endl;
      // Don't fail; fall back to Config.size.
    } else if(dm.w > 0 && dm.h > 0) {
      size = calc_scaled_view(Size2i{dm.w,dm.h},config.scale_factor,config.target_size);
    }
  }

  config.size.w = (size.w > 0) ? size.w : kFallbackWidth;
  config.size.h = (size.h > 0) ? size.h : kFallbackHeight;
  config.target_size.w = (config.target_size.w > 0) ? config.target_size.w : config.size.w;
  config.target_size.h = (config.target_size.h > 0) ? config.target_size.h : config.size.h;

  // Allow 0 if the user wants to use delta time only (no delay).
  // - See: stop_frame_timer()
  target_fps_ = (config.fps >= 0) ? config.fps : kFallbackFps;
  target_dpf_.set_from_fps(target_fps_); // Avoids divide by 0 internally.
}

Size2i CybelEngine::calc_scaled_view(const Size2i& view,float scale_factor,const Size2i& target_size) {
  // Scaled size.
  float sw = static_cast<float>(view.w) * scale_factor;
  float sh = static_cast<float>(view.h) * scale_factor;

  // If target size set, preserve aspect ratio of target size.
  if(target_size.w > 0 && target_size.h > 0) {
    // Aspect ratio.
    const float ar = static_cast<float>(target_size.w) / static_cast<float>(target_size.h);
    const float ar_h = std::round(sw / ar);

    if(ar_h <= std::round(sh)) {
      sh = ar_h; // Adjust height based on width.
    } else {
      sw = sh * ar; // Adjust width based on height.
    }
  }

  return Size2i{
    static_cast<int>(std::round(sw)),
    static_cast<int>(std::round(sh))
  };
}

void CybelEngine::init_gui(const Config& config) {
  // NOTE: Must set GL attrs after SDL_Init() and before SDL_CreateWindow().
#if defined(CYBEL_RENDERER_GLES)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
#else // CYBEL_RENDERER_GL
  // Use a 2004-2008 version.
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
#endif

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

  if(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24) != 0) {
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
  }

  Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;

#if defined(__EMSCRIPTEN__)
  // NOTE: There is a bug in SDL2 where SDL_SetWindowResizable() doesn't work to enable receiving resize
  //       events from the browser. We must explicitly pass SDL_WINDOW_RESIZABLE in the flags.
  window_flags |= SDL_WINDOW_RESIZABLE;
#endif

  // With the SDL_WINDOW_ALLOW_HIGHDPI flag, the size might change after.
  // Therefore, it's important that we call sync_size() later, which we do in run().
  core_.window = SDL_CreateWindow(
    title_.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,config.size.w,config.size.h,window_flags
  );

  if(!core_.window) {
    throw CybelError{"Failed to create window: ",Util::get_sdl_error(),'.'};
  }

  // On Desktop, the SDL_WINDOW_RESIZABLE flag in SDL_CreateWindow() increases the size for some reason
  // (even w/o SDL_WINDOW_ALLOW_HIGHDPI), but this explicit call doesn't.
  SDL_SetWindowResizable(core_.window,SDL_TRUE);
}

void CybelEngine::init_gpu_context() {
  core_.gl_context = SDL_GL_CreateContext(core_.window);

  if(!core_.gl_context) {
    throw CybelError{"Failed to create OpenGL context: ",Util::get_sdl_error(),'.'};
  }

  const GLenum error = glewInit();

  if(error != GLEW_OK) {
    throw CybelError{"Failed to init OpenGL GLEW [",error,"]: ",Util::get_glew_error(error),'.'};
  }

  set_vsync(is_vsync_);
  Util::clear_gl_errors(); // Mainly for WebGL context restored.
}

void CybelEngine::check_versions() {
#if defined(__EMSCRIPTEN__)
  // Output Emscripten version because different/newer versions can break the build,
  // and Emscripten doesn't store the version anywhere in the generated files.
  std::cout << "[INFO] Emscripten version: " << __EMSCRIPTEN_major__ << '.' << __EMSCRIPTEN_minor__ << '.'
            << __EMSCRIPTEN_tiny__ << '.' << std::endl;
#endif

  const auto* gl_version_cstr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const auto* gl_version = (gl_version_cstr) ? gl_version_cstr : "Failed to get OpenGL version";

  std::cout << "[INFO] OpenGL version: " << gl_version << '.' << std::endl;

  if(!GLEW_VERSION_2_1) {
    std::cerr << "[WARN] OpenGL version is < 2.1." << std::endl;

    const std::string msg = Util::build_str(
      "Your system's OpenGL version is less than 2.1.\n",
      "This game might not function properly.\n",
      "Please consider updating your graphics drivers or installing Mesa.\n\n",
      "OpenGL version: ",gl_version,'.'
    );

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,title_.c_str(),msg.c_str(),core_.window);
  }
}

void CybelEngine::run(std::unique_ptr<Game> game) {
  if(!game) { throw CybelError{"`game` is null."}; }
  if(game_) { throw CybelError{"A Game is already set/running."}; }

  game_ = std::move(game);
  is_running_ = true;

  game_->on_game_start(*this,*scene_ctx_);
  game_->on_scene_enter(*scene_ctx_);

  // Check the size again, due to SDL_WINDOW_ALLOW_HIGHDPI,
  // and also need to call Game.on_scene_resize().
  sync_size(true);
  input_man_->shrink();

#if !defined(__EMSCRIPTEN__)
  while(run_frame()) {}

  std::cout << "[INFO] Stopping gracefully." << std::endl;
#endif
}

void CybelEngine::request_stop() {
  is_running_ = false;
}

bool CybelEngine::run_frame() {
  if(!is_running_) { return false; }

  if(!core_.gl_context) {
    // NOTE: Don't sleep or call SDL_Delay()/stop_frame_timer(), since SDL_Delay()/sleep is just a while-loop
    //       in Emscripten, and because requestAnimationFrame() is used, it won't hog the CPU unnecessarily.
    handle_core_events_only();
    start_frame_timer(); // Try to keep delta time close to 0.

    return is_running_;
  }

  // NOTE: For Web, must call stop_frame_timer() here and not at the end of this function.
  //       - I'm not sure of the exact cause, but could be because of Chrome's async rendering.
  //       - I can only recreate it on an older laptop (but with up-to-date software) with
  //         "graphics acceleration" turned off in Chrome.
  stop_frame_timer();
  start_frame_timer();

  {
    CYBEL_METRICS_PROFILE_SCOPE("Frame");

    input_man_->begin_input();
    handle_events();
    handle_input();

    // Event/Input requested to stop.
    if(!is_running_) { return false; }

    if(is_logic_running_) {
      game_->update_scene_logic(frame_step_,*scene_ctx_);
      scene_man_.curr_scene().update_scene_logic(frame_step_,*scene_ctx_);
      scene_man_.commit_pending();
    }

    {
      CYBEL_METRICS_PROFILE_SCOPE("Render");

      renderer_->clear_view();
      game_->draw_scene(*renderer_,*scene_ctx_);
      scene_man_.curr_scene().draw_scene(*renderer_,*scene_ctx_);
      SDL_GL_SwapWindow(core_.window);
    }
  }

  // NOTE: Must be at end of frame so that Counter.frame_count is accurate.
  CYBEL_METRICS_END_FRAME();

  return true;
}

#if defined(__EMSCRIPTEN__)

void CybelEngine::run_web_frame(void* user_data) {
  if(!user_data) {
    std::cerr << "[ERROR] `user_data` is null (no CybelEngine) in " << __func__ << "()." << std::endl;
    emscripten_cancel_main_loop();
    return;
  }

  auto& engine = *static_cast<CybelEngine*>(user_data);

  try {
    engine.run_frame();
  } catch(const CybelError& e) {
    engine.show_error(e.what());
    engine.request_stop();
    emscripten_cancel_main_loop();
  }
}

/// To simulate WebGL context lost & restored in JS:
///   // On itch.io, select `index.html` or inspect the canvas first.
///   let gle = Module.canvas.getContext('webgl2').getExtension('WEBGL_lose_context');
///   gle.loseContext();
///   gle.restoreContext();
///
/// See:
/// - https://www.khronos.org/webgl/wiki/HandlingContextLost
/// - https://emscripten.org/docs/api_reference/html5.h.html#id93
bool CybelEngine::on_webgl_context_change(int event_type,[[maybe_unused]] const void* reserved,
                                          void* user_data) {
  if(!user_data) {
    std::cerr << "[ERROR] `user_data` is null (no CybelEngine) in " << __func__ << "()." << std::endl;
    emscripten_cancel_main_loop();
    return false;
  }

  auto& engine = *static_cast<CybelEngine*>(user_data);

  switch(event_type) {
    case EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST:
      std::cerr << "[WARN] WebGL context lost." << std::endl;
      engine.on_gpu_context_loss();
      return true;

    case EMSCRIPTEN_EVENT_WEBGLCONTEXTRESTORED:
      std::cout << "[INFO] WebGL context restored. Attempting to restore the game." << std::endl;
      EM_ASM( Module.setStatus("<br>WebGL context restored. Attempting to restore the game...<br><br>"); );

      try {
        engine.on_gpu_context_restore();
        EM_ASM( Module.setStatus(""); );
      } catch(const CybelError& e) {
        engine.show_error(e.what());
        engine.request_stop();
        // emscripten_cancel_main_loop();
      }
      return true;
  }

  return false;
}

#endif // __EMSCRIPTEN__

void CybelEngine::on_gpu_context_loss() {
  scene_man_.curr_scene().on_scene_exit(*scene_ctx_);
  game_->on_scene_exit(*scene_ctx_);

  for(auto& bag : scene_man_.prev_scene_bags()) {
    if(bag.scene) { bag.scene->on_scene_gpu_context_loss(*scene_ctx_); }
  }

  scene_man_.curr_scene().on_scene_gpu_context_loss(*scene_ctx_);
  game_->on_scene_gpu_context_loss(*scene_ctx_);

  renderer_->on_gpu_context_loss(GpuContextKey{});
  asset_man_->on_gpu_context_loss(GpuContextKey{});

  core_.gl_context = nullptr;
}

void CybelEngine::on_gpu_context_restore() {
  init_gpu_context();

  asset_man_->on_gpu_context_restore(GpuContextKey{});
  renderer_->on_gpu_context_restore(GpuContextKey{});

  // NOTE: Must call Game first so that it can reload textures, etc.
  game_->on_scene_gpu_context_restore(*scene_ctx_);
  scene_man_.curr_scene().on_scene_gpu_context_restore(*scene_ctx_);

  for(auto& bag : scene_man_.prev_scene_bags()) {
    if(bag.scene) { bag.scene->on_scene_gpu_context_restore(*scene_ctx_); }
  }

  game_->on_scene_enter(*scene_ctx_);
  scene_man_.curr_scene().on_scene_enter(*scene_ctx_);
  sync_size(true); // Call Renderer.resize() & Game/Scene.on_scene_resize().

  start_frame_timer(); // Try to set delta time close to 0 to allow player to adjust.
}

void CybelEngine::nav_back_in_web() {
#if defined(__EMSCRIPTEN__)
  EM_ASM( window.history.back(); );
#endif
}

SceneBag CybelEngine::build_scene(scene_id_t id) {
  return game_->build_scene(id,*scene_ctx_);
}

void CybelEngine::on_scene_enter(Scene& scene) {
  scene.on_scene_enter(*scene_ctx_);
  scene.on_scene_resize(*renderer_,*scene_ctx_);
}

void CybelEngine::on_scene_exit(Scene& scene) {
  scene.on_scene_exit(*scene_ctx_);
}

void CybelEngine::sync_size(bool force) {
  Size2i size{};

  SDL_GL_GetDrawableSize(core_.window,&size.w,&size.h);
  resize(size,force);
}

void CybelEngine::resize(const Size2i& size,bool force) {
  if(!force && size == renderer_->dimens().size) {
    return; // Size didn't change.
  }

  renderer_->resize(size);
  game_->on_scene_resize(*renderer_,*scene_ctx_);
  scene_man_.curr_scene().on_scene_resize(*renderer_,*scene_ctx_);
}

void CybelEngine::start_frame_timer() {
  frame_timer_.restart();
}

void CybelEngine::stop_frame_timer() {
  frame_timer_.sleep_for_fps(target_dpf_);

  frame_step_.dpf = frame_timer_.pause();
  frame_step_.delta_time = frame_step_.dpf.secs(); // Delta time should be in fractional seconds.

  // Exponential Moving Average (EMA) to reduce the effects of hiccups,
  // instead of a typical average: avg = (avg + fps) / 2.
  avg_fps_ = MetricMan::calc_exp_mov_avg(avg_fps_,static_cast<float>(frame_step_.dpf.fps()));
}

void CybelEngine::handle_events() {
  SDL_Event event{};
  // Store in a var to prevent re-sizing a bunch of times in the loop (costly),
  // while the user is still dragging the window corner/edge.
  bool should_resize = false;

  while(SDL_PollEvent(&event) != 0) {
    switch(event.type) {
      case SDL_QUIT:
        std::cout << "[EVENT] Received Quit event." << std::endl;
        request_stop();
        return;

      case SDL_WINDOWEVENT:
        switch(event.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            should_resize = true;
            break;
        }
        break;

      default:
        input_man_->handle_event(event);
        break;
    }
  }

  if(should_resize) { sync_size(false); }
}

void CybelEngine::handle_core_events_only() {
  SDL_Event event{};

  while(SDL_PollEvent(&event) != 0) {
    switch(event.type) {
      case SDL_QUIT:
        std::cout << "[EVENT] Received Quit event." << std::endl;
        request_stop();
        return;
    }
  }
}

void CybelEngine::on_input_event(input_id_t input_id) {
  game_->on_scene_input_event(input_id,*scene_ctx_);
  scene_man_.curr_scene().on_scene_input_event(input_id,*scene_ctx_);
}

void CybelEngine::handle_input() {
  game_->handle_scene_input(*input_man_,*scene_ctx_);
  scene_man_.curr_scene().handle_scene_input(*input_man_,*scene_ctx_);
}

void CybelEngine::show_error(const std::string& error) const {
  show_error(title_,error);
}

void CybelEngine::show_error(const std::string& title,const std::string& error) const {
  show_error(title,error,core_.window);
}

void CybelEngine::show_error_no_window(const std::string& title,const std::string& error) {
  show_error(title,error,nullptr);
}

void CybelEngine::show_error(const std::string& title,const std::string& error,SDL_Window* window) {
  std::cerr << "[ERROR] " << error << std::endl;

  constexpr std::size_t max_len = 80;

  // Avoid copy if possible.
  if(error.length() <= max_len) {
    // SDL_ShowSimpleMessageBox() can be called before/after SDL_Init()/SDL_Quit().
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),error.c_str(),window);
  } else {
    const auto wrapped_error = utf8::StrUtil::wrap_words(error,max_len);

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title.c_str(),wrapped_error.c_str(),window);
  }
}

void CybelEngine::set_icon(const Image& img) { SDL_SetWindowIcon(core_.window,img.handle()); }

void CybelEngine::set_title(const std::string& title) { SDL_SetWindowTitle(core_.window,title.c_str()); }

void CybelEngine::reset_title() { set_title(title_); }

void CybelEngine::set_fullscreen(bool fullscreen,bool windowed) {
  const auto result = SDL_SetWindowFullscreen(
    core_.window,
    fullscreen ? (windowed ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN) : 0
  );

  if(result != 0) {
    const char* desc = fullscreen ? (windowed ? "windowed fullscreen" : "fullscreen") : "windowed";
    std::cerr << "[WARN] Failed to set window to [" << desc << "] with error [" << result << "]: "
              << Util::get_sdl_error() << '.' << std::endl;
  }
}

void CybelEngine::set_cursor_visible(bool visible) {
  SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}

void CybelEngine::set_vsync(bool enable) {
  if(enable) {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC,"1");

    // First, try with adaptive vsync.
    if(SDL_GL_SetSwapInterval(-1) != 0) {
      SDL_GL_SetSwapInterval(1);
    }
  } else {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC,"0");
    SDL_GL_SetSwapInterval(0);
  }

  is_vsync_ = (SDL_GL_GetSwapInterval() != 0);
}

void CybelEngine::set_logic_running(bool running) { is_logic_running_ = running; }

const std::string& CybelEngine::title() const { return title_; }

bool CybelEngine::is_fullscreen() const {
  // This also covers SDL_WINDOW_FULLSCREEN_DESKTOP.
  return (SDL_GetWindowFlags(core_.window) & SDL_WINDOW_FULLSCREEN);
}

bool CybelEngine::is_cursor_visible() const { return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE; }

bool CybelEngine::is_vsync() const { return is_vsync_; }

bool CybelEngine::is_logic_running() const { return is_logic_running_; }

FileSys& CybelEngine::file_sys() { return *file_sys_; }

const FileSys& CybelEngine::file_sys() const { return *file_sys_; }

Game& CybelEngine::game() { return *game_; }

const Game& CybelEngine::game() const { return *game_; }

SceneMan& CybelEngine::scenes() { return scene_man_; }

const SceneMan& CybelEngine::scenes() const { return scene_man_; }

InputMan& CybelEngine::input() { return *input_man_; }

const InputMan& CybelEngine::input() const { return *input_man_; }

AssetMan& CybelEngine::assets() { return *asset_man_; }

const AssetMan& CybelEngine::assets() const { return *asset_man_; }

Renderer& CybelEngine::renderer() { return *renderer_; }

const Renderer& CybelEngine::renderer() const { return *renderer_; }

AudioPlayer& CybelEngine::audio() { return *audio_player_; }

const AudioPlayer& CybelEngine::audio() const { return *audio_player_; }

const ViewDimens& CybelEngine::dimens() const { return renderer_->dimens(); }

int CybelEngine::target_fps() const { return target_fps_; }

const Duration& CybelEngine::target_dpf() const { return target_dpf_; }

const Duration& CybelEngine::dpf() const { return frame_step_.dpf; }

double CybelEngine::delta_time() const { return frame_step_.delta_time; }

float CybelEngine::avg_fps() const { return avg_fps_; }

CybelEngine::EngineCore::~EngineCore() noexcept {
  if(gl_context) {
    SDL_GL_DeleteContext(gl_context);
    gl_context = nullptr;
  }
  if(window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  IMG_Quit();
  SDL_Quit();
}

} // namespace cybel

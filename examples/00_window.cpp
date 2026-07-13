
#include <cybel/cybel_engine.h>
#include <cybel/game.h>

#include <cybel/gfx/renderer.h>
#include <cybel/scene/scene_context.h>
#include <cybel/text/text_util.h>
#include <cybel/types/color.h>
#include <cybel/types/cybel_error.h>
#include <cybel/types/duration.h>
#include <cybel/types/frame_step.h>
#include <cybel/types/size.h>
#include <cybel/util/ticker.h>

#include <iostream>
#include <memory>

using namespace cybel;

class MyGame final : public Game {
public:
  void on_game_start([[maybe_unused]] CybelEngine& engine,SceneContext& ctx) override {
    ctx.assets.shrink_assets(); // No assets.

    // Set to fullscreen if want.
    //engine.set_fullscreen(true);
    //engine.set_cursor_visible(false);

    std::cout << "> Start Game!" << std::endl;
  }

  void on_scene_resize([[maybe_unused]] Renderer& ren,SceneContext& ctx) override {
    print_stats(ctx);
  }

  void update_scene_logic(const FrameStep& step,SceneContext& ctx) override {
    if(print_stats_ticker.tick(step)) {
      print_stats(ctx);
    }
  }

private:
  // Every 1s, print stats.
  ChronoTicker print_stats_ticker{Duration::from_secs(1.0f),TickerFlag::kLoop | TickerFlag::kStart};

  void print_stats(const SceneContext& ctx) {
    std::cout << TextUtil::fmt(
      "\r> Window Size: {:11} | FPS: {:8.2f}\t",
      TextUtil::fmt("{}x{}",ctx.dimens.size.w,ctx.dimens.size.h),
      ctx.engine.avg_fps()
    );

    // `\r` doesn't work in Web output.
#if defined(__EMSCRIPTEN__)
    std::cout << std::endl;
#else
    std::cout << std::flush;
#endif
  }
};

int main() {
  try {
    CybelEngine& engine = CybelEngine::init(CybelEngine::Config{
      .title = "00 Window | Cybel Engine",

      // Make the window 50% of screen size, instead of using `size`.
      .scale_factor = 0.50f,            // default: (not set)
      //.size = Size2i{800,600},        // default: 1600x900 (if `scale_factor` not set)

      // For 2D scaling only.
      .target_size = Size2i{800,600},   // default: (copies `size`/`scale_factor`)

      .fps = 110,                       // default: 60
      .vsync = false,                   // default: true
      .clear_color = Color4f::kCopper,  // default: Black
    });

    engine.run(std::make_unique<MyGame>());
  } catch(const CybelError& e) {
    CybelEngine::show_error(e.what());
    return 1;
  }
}


#ifndef CYBEL_EXAMPLES_COMMON_H_
#define CYBEL_EXAMPLES_COMMON_H_

#include <cybel/cybel_engine.h>

#include <filesystem>
#include <initializer_list>
#include <string_view>

namespace cybel {

auto example_config(std::string_view title) -> CybelEngine::Config;
auto example_asset_dirs() -> std::initializer_list<std::filesystem::path>;

inline auto example_config(std::string_view title) -> CybelEngine::Config {
  return CybelEngine::Config{
    .title = std::string{title} + " | Cybel Engine",

    .scale_factor = 0.50f,
    //.size = Size2i{800,600},
    .target_size = Size2i{800,600},

    .fps = 60,
    .vsync = true,
    .clear_color = Color4f::hex(0x112233),
  };
}

// TODO: Return all possible asset dirs for examples.
inline auto example_asset_dirs() -> std::initializer_list<std::filesystem::path> {
  return {
  };
}

} // namespace cybel
#endif

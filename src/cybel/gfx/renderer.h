/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_GFX_RENDERER_H_
#define CYBEL_GFX_RENDERER_H_

#include "cybel/common.h"

#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/gpu_context_key.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/color.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"
#include "cybel/types/view_dimens.h"

#include <functional>
#include <unordered_map>

namespace cybel {

/// The begin_/end_*() functions do not preserve the previous states when nesting.
/// This is by design.
///
/// Alternatively, the wrap_*() functions do preserve the previous states when nesting.
class Renderer {
public:
  class TextureWrapper {
  public:
    Renderer& ren;
    const Texture& tex;
    Pos4f src{};

    explicit TextureWrapper(Renderer& ren,const Texture& tex,const Pos4f& src);

    TextureWrapper& draw_quad(const Pos3i& pos);
    TextureWrapper& draw_quad(const Pos3i& pos,const Size2i& size);
  };

  class SpriteWrapper {
  public:
    Renderer& ren;
    const Sprite& sprite;

    explicit SpriteWrapper(Renderer& ren,const Sprite& sprite);

    SpriteWrapper& draw_quad(const Pos3i& pos);
    SpriteWrapper& draw_quad(const Pos3i& pos,const Size2i& size);
  };

  class SpriteAtlasWrapper {
  public:
    Renderer& ren;
    const SpriteAtlas& atlas;

    explicit SpriteAtlasWrapper(Renderer& ren,const SpriteAtlas& atlas);

    SpriteAtlasWrapper& draw_quad(std::size_t index,const Pos3i& pos);
    SpriteAtlasWrapper& draw_quad(std::size_t index,const Pos3i& pos,const Size2i& size);
    SpriteAtlasWrapper& draw_quad(const Pos2i& cell,const Pos3i& pos);
    SpriteAtlasWrapper& draw_quad(const Pos2i& cell,const Pos3i& pos,const Size2i& size);
  };

  class FontAtlasWrapper {
  public:
    Renderer& ren;
    const FontAtlas& font;
    Pos3i init_pos{};
    Pos3i pos{};
    Size2i rune_size{};
    Size2i spacing{};

    explicit FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                              const Size2i& spacing);

    FontAtlasWrapper& draw_bg(const Color4f& color,const Size2i& str_size);

    FontAtlasWrapper& print();
    FontAtlasWrapper& print(char32_t rune);
    FontAtlasWrapper& print(std::string_view str);
    FontAtlasWrapper& print_blanks(int count);

    /// Prints formatted text:
    /// - Replaces `{}` with the next arg from `args`, if there is one.
    /// - Colors `{<color> text }` with the `color`.
    ///   - NOTE: You must use a space at the end before the curly brace: ` }`.
    ///   - If <color> starts with `0x` (zero) or `0X`, then it's parsed as an RGB or RGBA hex value:
    ///     - `{0x112233 text } and {0x11223344 text }`
    ///   - Else, <color> is fetched from Renderer.font_color(), if it exists:
    ///     - `{green text }`
    ///
    /// Notes:
    /// - Escape `{` with 2 `{{` or `}` with 2 `}}`:
    ///   - `This {{is }} escaped: {{}}.`
    /// - Shouldn't fail on invalid text, number of args, colors, etc.
    FontAtlasWrapper& print_fmt(std::string_view fmt,std::initializer_list<std::string_view> args = {});

    FontAtlasWrapper& puts();
    FontAtlasWrapper& puts(char32_t rune);
    FontAtlasWrapper& puts(std::string_view str);
    FontAtlasWrapper& puts_blanks(int count);

    /// @copydoc print_fmt(std::string_view,std::initializer_list<std::string_view>)
    FontAtlasWrapper& puts_fmt(std::string_view fmt,std::initializer_list<std::string_view> args = {});

    Size2i calc_total_size(const Size2i& str_size) const;

    void set_bg_padding(const Size2i& padding);

    const Size2i& bg_padding() const;

  private:
    Size2i bg_padding_{};
  };

  struct QuadBufferData {
    GLuint tex_handle = 0;
    Pos3f normal{};
    Pos3f vertices[4] = {Pos3f{},Pos3f{},Pos3f{},Pos3f{}};
  };

  using WrapCallback = std::function<void()>;
  using WrapTextureCallback = std::function<void(TextureWrapper&)>;
  using WrapSpriteCallback = std::function<void(SpriteWrapper&)>;
  using WrapSpriteAtlasCallback = std::function<void(SpriteAtlasWrapper&)>;
  using WrapFontAtlasCallback = std::function<void(FontAtlasWrapper&)>;

  static constexpr Pos4f kDefaultSrc{0.0f,0.0f,1.0f,1.0f};

  explicit Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  Renderer(const Renderer& other) = delete;
  Renderer(Renderer&& other) noexcept = delete;
  virtual ~Renderer() noexcept = default;

  Renderer& operator=(const Renderer& other) = delete;
  Renderer& operator=(Renderer&& other) noexcept = delete;

  virtual void on_gpu_context_loss(GpuContextKey);
  virtual void on_gpu_context_restore(GpuContextKey);

  virtual void resize(const Size2i& size);
  void clear_view();

  virtual Renderer& begin_2d_scene() = 0;
  virtual Renderer& begin_3d_scene() = 0;

  Renderer& begin_auto_center_scale();
  Renderer& begin_auto_anchor_scale(const Pos2f& anchor);
  Renderer& begin_auto_scale();
  Renderer& end_scale();

  virtual Renderer& begin_color(const Color4f& color) = 0;
  virtual Renderer& end_color();

  Renderer& begin_add_blend();
  Renderer& end_blend();

  Renderer& begin_tex(const Texture& tex);
  virtual Renderer& end_tex() = 0;

  Renderer& wrap_color(const Color4f& color,const WrapCallback& callback);
  Renderer& wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback);
  Renderer& wrap_add_blend(const WrapCallback& callback);

  Renderer& wrap_tex(const Texture& tex,const WrapTextureCallback& callback);
  Renderer& wrap_tex(const Texture& tex,const Pos4f& src,const WrapTextureCallback& callback);

  Renderer& wrap_sprite(const Sprite& sprite,const WrapSpriteCallback& callback);
  Renderer& wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback);

  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                            const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                            const Size2i& spacing,const WrapFontAtlasCallback& callback);

  virtual Renderer& draw_quad(const Pos3i& pos,const Size2i& size) = 0;
  virtual Renderer& draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) = 0;

  Pos5f build_dest_pos5f(const Pos3i& pos,const Size2i& size) const;

  virtual void translate_model_matrix(const Pos3f& pos) = 0;
  virtual void rotate_model_matrix(float angle,const Pos3f& axis) = 0;
  virtual void update_model_matrix() = 0;
  virtual void push_model_matrix() = 0;
  virtual void pop_model_matrix() = 0;

  virtual GLuint gen_quad_buffers(int count) = 0;
  virtual void delete_quad_buffers(GLuint id,int count) = 0;
  virtual void compile_quad_buffer(GLuint id,int index,const QuadBufferData& data) = 0;
  virtual void draw_quad_buffer(GLuint id,int index) = 0;

  void set_font_color(const std::string& name,const Color4f& color);

  const ViewDimens& dimens() const;
  const Color4f& clear_color() const;
  Color4f* font_color(const std::string& name);

protected:
  ViewDimens dimens_{};
  int depth_bits_ = 0;
  Pos2f scale_{1.0f,1.0f};
  float aspect_scale_ = 1.0f;
  Pos2f offset_{0.0f,0.0f};
  Color4f clear_color_{};

  Color4f curr_color_{1.0f};
  GLuint curr_tex_handle_ = 0;

  virtual Renderer& begin_tex(GLuint handle) = 0;

private:
  struct BlendMode {
    GLenum src_factor{};
    GLenum dst_factor{};
  };

  static constexpr BlendMode kDefaultBlendMode{GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA};
  static constexpr BlendMode kAddBlendMode{GL_ONE,GL_ONE};

  BlendMode curr_blend_mode_ = kDefaultBlendMode;
  std::unordered_map<std::string,Color4f> font_colors_{};

  void init_gpu_context();

  Renderer& begin_blend(const BlendMode& mode);

  Renderer& wrap_tex(GLuint handle,const WrapCallback& callback);
};

} // namespace cybel
#endif

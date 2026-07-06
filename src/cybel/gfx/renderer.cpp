/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "renderer.h"

#include "cybel/gfx/gl_util.h"
#include "cybel/text/rune_view.h"
#include "cybel/types/cybel_error.h"

#include <stack>

namespace cybel {

Renderer::Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color)
  : clear_color_(clear_color) {
  // Ensure 1+ to avoid divides by 0.
  dimens_.init_size = Size2i{std::max(size.w,1),std::max(size.h,1)};
  dimens_.size = dimens_.init_size;
  dimens_.target_size = Size2i{std::max(target_size.w,1),std::max(target_size.h,1)};

  // Add all standard colors.
  font_colors_["black"] = Color4f::kBlack;
  font_colors_["blue"] = Color4f::kBlue;
  font_colors_["brown"] = Color4f::kBrown;
  font_colors_["copper"] = Color4f::kCopper;
  font_colors_["cyan"] = Color4f::kCyan;
  font_colors_["green"] = Color4f::kGreen;
  font_colors_["hotpink"] = Color4f::kHotPink;
  font_colors_["pink"] = Color4f::kPink;
  font_colors_["purple"] = Color4f::kPurple;
  font_colors_["red"] = Color4f::kRed;
  font_colors_["white"] = Color4f::kWhite;
  font_colors_["yellow"] = Color4f::kYellow;

  init_gpu_context();
}

void Renderer::init_gpu_context() {
  SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE,&depth_bits_);
  std::cout << "[INFO] OpenGL depth bits: " << depth_bits_ << '.' << std::endl;

  glClearColor(clear_color_.r,clear_color_.g,clear_color_.b,clear_color_.a);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_BLEND);
  begin_blend(curr_blend_mode_);

  const GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{"Failed to init OpenGL: {}.",GlUtil::fetch_error_str(error)};
  }
}

void Renderer::on_gpu_context_loss(GpuContextKey) {
  curr_tex_handle_ = 0;
}

void Renderer::on_gpu_context_restore(GpuContextKey) {
  curr_tex_handle_ = 0;

  GlUtil::clear_errors();
  init_gpu_context();
}

void Renderer::resize(const Size2i& size) {
  // Allow resize even if the width & height haven't changed.
  // - If decide to change this logic, need to allow force resize so can resize on init.

  // Avoid divides by 0 [e.g., in begin_3d_scene()].
  dimens_.size.w = std::max(size.w,1);
  dimens_.size.h = std::max(size.h,1);
  // `target_size.w/h` should never be 0 (checked in ctor).
  dimens_.scale.x = static_cast<float>(dimens_.size.w) / static_cast<float>(dimens_.target_size.w);
  dimens_.scale.y = static_cast<float>(dimens_.size.h) / static_cast<float>(dimens_.target_size.h);
  dimens_.aspect_scale = std::min(dimens_.scale.x,dimens_.scale.y);

  glViewport(0,0,dimens_.size.w,dimens_.size.h);
}

void Renderer::clear_view() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

Renderer& Renderer::begin_auto_center_scale() {
  return begin_auto_anchor_scale(Pos2f{0.5f,0.5f});
}

Renderer& Renderer::begin_auto_anchor_scale(const Pos2f& anchor) {
  const auto w = static_cast<float>(dimens_.size.w);
  const auto h = static_cast<float>(dimens_.size.h);
  const auto tw = static_cast<float>(dimens_.target_size.w);
  const auto th = static_cast<float>(dimens_.target_size.h);

  scale_.x = dimens_.aspect_scale;
  scale_.y = dimens_.aspect_scale;
  aspect_scale_ = dimens_.aspect_scale;
  offset_.x = (w - (tw * scale_.x)) * anchor.x;
  offset_.y = (h - (th * scale_.y)) * anchor.y;

  return *this;
}

Renderer& Renderer::begin_auto_scale() {
  scale_.x = dimens_.scale.x;
  scale_.y = dimens_.scale.y;
  aspect_scale_ = dimens_.aspect_scale;

  return *this;
}

Renderer& Renderer::end_scale() {
  scale_.x = 1.0f;
  scale_.y = 1.0f;
  aspect_scale_ = 1.0f;
  offset_.x = 0.0f;
  offset_.y = 0.0f;

  return *this;
}

Renderer& Renderer::end_color() { return begin_color(Color4f{1.0f}); }

Renderer& Renderer::begin_blend(const BlendMode& mode) {
  glBlendFunc(mode.src_factor,mode.dst_factor);

  return *this;
}

Renderer& Renderer::begin_add_blend() {
  return begin_blend(kAddBlendMode);
}

Renderer& Renderer::end_blend() {
  return begin_blend(kDefaultBlendMode);
}

Renderer& Renderer::begin_tex(const Texture& tex) {
  return begin_tex(tex.handle());
}

Renderer& Renderer::wrap_color(const Color4f& color,const WrapCallback& callback) {
  const auto prev_color = curr_color_;

  begin_color(color);
  curr_color_ = color;

  callback();

  begin_color(prev_color);
  curr_color_ = prev_color;

  return *this;
}

Renderer& Renderer::wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback) {
  const Pos3f scaled_pos{
    offset_.x + (static_cast<float>(pos.x) * scale_.x),
    offset_.y + (static_cast<float>(pos.y) * scale_.y),
    static_cast<float>(pos.z)
  };

  push_model_matrix();
    translate_model_matrix(scaled_pos);
    rotate_model_matrix(angle,Pos3f{0.0f,0.0f,1.0f});
    translate_model_matrix(-scaled_pos);
    update_model_matrix();
    callback();
  pop_model_matrix();

  return *this;
}

Renderer& Renderer::wrap_add_blend(const WrapCallback& callback) {
  const auto prev_mode = curr_blend_mode_;

  begin_blend(kAddBlendMode);
  curr_blend_mode_ = kAddBlendMode;

  callback();

  begin_blend(prev_mode);
  curr_blend_mode_ = prev_mode;

  return *this;
}

Renderer& Renderer::wrap_tex(const Texture& tex,const WrapTextureCallback& callback) {
  return wrap_tex(tex,kDefaultSrc,callback);
}

Renderer& Renderer::wrap_tex(const Texture& tex,const Pos4f& src,const WrapTextureCallback& callback) {
  TextureWrapper wrapper{*this,tex,src};

  return wrap_tex(tex.handle(),[&] { callback(wrapper); });
}

Renderer& Renderer::wrap_tex(GLuint handle,const WrapCallback& callback) {
  const auto prev_tex = curr_tex_handle_;

  begin_tex(handle);
  curr_tex_handle_ = handle;

  callback();

  if(prev_tex != 0) {
    begin_tex(prev_tex);
  } else {
    end_tex();
  }
  curr_tex_handle_ = prev_tex;

  return *this;
}

Renderer& Renderer::wrap_sprite(const Sprite& sprite,const WrapSpriteCallback& callback) {
  SpriteWrapper wrapper{*this,sprite};

  return wrap_tex(sprite.handle(),[&] { callback(wrapper); });
}

Renderer& Renderer::wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback) {
  SpriteAtlasWrapper wrapper{*this,atlas};

  return wrap_tex(atlas.handle(),[&] { callback(wrapper); });
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,
                                    const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,pos,font.cell_size(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                                    const WrapFontAtlasCallback& callback) {
  return wrap_font_atlas(font,pos,rune_size,font.spacing(),callback);
}

Renderer& Renderer::wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                                    const Size2i& spacing,const WrapFontAtlasCallback& callback) {
  FontAtlasWrapper wrapper{*this,font,pos,rune_size,spacing};

  return wrap_tex(font.handle(),[&] { callback(wrapper); });
}

Pos5f Renderer::build_dest_pos5f(const Pos3i& pos,const Size2i& size) const {
  const auto x1 = offset_.x + (static_cast<float>(pos.x) * scale_.x);
  const auto y1 = offset_.y + (static_cast<float>(pos.y) * scale_.y);
  const auto x2 = x1 + (static_cast<float>(size.w) * aspect_scale_);
  const auto y2 = y1 + (static_cast<float>(size.h) * aspect_scale_);
  const auto z = static_cast<float>(pos.z);

  return Pos5f{x1,y1,x2,y2,z};
}

void Renderer::set_font_color(const std::string& name,const Color4f& color) {
  font_colors_[name] = color;
}

const ViewDimens& Renderer::dimens() const { return dimens_; }

const Color4f& Renderer::clear_color() const { return clear_color_; }

Color4f* Renderer::font_color(const std::string& name) {
  const auto it = font_colors_.find(name);

  return (it == font_colors_.end()) ? nullptr : &it->second;
}

Renderer::TextureWrapper::TextureWrapper(Renderer& ren,const Texture& tex,const Pos4f& src)
  : ren(ren),tex(tex),src(src) {}

Renderer::TextureWrapper& Renderer::TextureWrapper::draw_quad(const Pos3i& pos) {
  return draw_quad(pos,tex.size());
}

Renderer::TextureWrapper& Renderer::TextureWrapper::draw_quad(const Pos3i& pos,const Size2i& size) {
  ren.draw_quad(src,pos,size);

  return *this;
}

Renderer::SpriteWrapper::SpriteWrapper(Renderer& ren,const Sprite& sprite)
  : ren(ren),sprite(sprite) {}

Renderer::SpriteWrapper& Renderer::SpriteWrapper::draw_quad(const Pos3i& pos) {
  return draw_quad(pos,sprite.size());
}

Renderer::SpriteWrapper& Renderer::SpriteWrapper::draw_quad(const Pos3i& pos,const Size2i& size) {
  ren.draw_quad(sprite.src(),pos,size);

  return *this;
}

Renderer::SpriteAtlasWrapper::SpriteAtlasWrapper(Renderer& ren,const SpriteAtlas& atlas)
  : ren(ren),atlas(atlas) {}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(std::size_t index,const Pos3i& pos) {
  return draw_quad(index,pos,atlas.cell_size());
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(std::size_t index,const Pos3i& pos,
                                                                      const Size2i& size) {
  const Pos4f* src = atlas.src(index);

  if(src != nullptr) { ren.draw_quad(*src,pos,size); }

  return *this;
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(const Pos2i& cell,const Pos3i& pos) {
  return draw_quad(cell,pos,atlas.cell_size());
}

Renderer::SpriteAtlasWrapper& Renderer::SpriteAtlasWrapper::draw_quad(const Pos2i& cell,const Pos3i& pos,
                                                                      const Size2i& size) {
  const Pos4f* src = atlas.src(cell);

  if(src != nullptr) { ren.draw_quad(*src,pos,size); }

  return *this;
}

Renderer::FontAtlasWrapper::FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos3i& pos,
                                             const Size2i& rune_size,const Size2i& spacing)
  : ren(ren),font(font),init_pos(pos),pos(pos),rune_size(rune_size),spacing(spacing) {}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::draw_bg(const Color4f& color,const Size2i& str_size) {
  ren.end_tex(); // Temporarily unbind the font texture.
  ren.wrap_color(color,[&] {
    ren.draw_quad(Pos3i{pos.x - bg_padding_.w,pos.y - bg_padding_.h,pos.z},calc_total_size(str_size));
  });
  ren.begin_tex(font.handle()); // Bind back the font texture.

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print() { return print_blanks(1); }

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(char32_t rune) {
  const Pos4f* src = font.src(font.rune_index(rune));

  if(src != nullptr) { ren.draw_quad(*src,pos,rune_size); }

  return print();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print(std::string_view str) {
  if(str.empty()) { return *this; }

  for(const auto& r : RuneView{str}) {
    if(r.rune == '\n') {
      puts();
    } else {
      print(r.rune);
    }
  }

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print_blanks(int count) {
  pos.x += ((rune_size.w + spacing.w) * count);

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::print_fmt(
  std::string_view fmt,std::initializer_list<std::string_view> args
) {
  if(fmt.empty()) { return *this; }

  const auto fmt_it_end = RuneIterator::end(fmt);
  const auto args_it_end = args.end();

  auto args_it = args.begin();
  std::stack<Color4f> color_stack{};

  for(auto fmt_it = RuneIterator::begin(fmt); fmt_it != fmt_it_end; ++fmt_it) {
    const auto rune1 = fmt_it->rune;

    switch(rune1) {
      // Handles: `{{`, `{}`, `{<color> `
      case '{': {
        if((++fmt_it) == fmt_it_end) {
          print(rune1);
          goto break_main_loop;
        }

        const auto rune2 = fmt_it->rune;

        switch(rune2) {
          // Escaped: `{{`
          case '{':
            print(rune1);
            break;

          // Arg: `{}`
          // - Note that this doesn't account for erroneously "escaped" `{}}`, which should be `{{}}`.
          case '}': {
            if(args_it != args_it_end) {
              // Instead, this could call print_fmt() or use its own stack.
              print(*args_it);
              ++args_it;
            } else {
              // Just print `{}`.
              print(rune1);
              print(rune2);
            }
          } break;

          // Styled text: `{<color> `
          default: {
            const std::size_t first_i = fmt_it->index;

            do {
              if(fmt_it->rune == ' ') { break; }
            } while((++fmt_it) != fmt_it_end);

            // No more runes to print with color.
            if(fmt_it == fmt_it_end) {
              goto break_main_loop;
            }

            const auto color_str = fmt.substr(first_i,fmt_it->index - first_i);
            Color4f color = Color4f::kWhite; // Fallback color.

            // RGB `0x112233` or RGBA `0x11223344`.
            if(color_str.length() >= 8 && color_str[0] == '0' &&
               (color_str[1] == 'x' || color_str[1] == 'X')) {
              color = Color4f::hex(color_str,color);
            } else {
              const auto* color_ptr = ren.font_color(std::string{color_str});
              if(color_ptr) { color = *color_ptr; }
            }

            color_stack.push(color);
            ren.begin_color(color);
          } break;
        }
      } break;

      // Handles: ` `, ` }}`, color ` }`
      case ' ': {
        const auto peek_fmt_it2 = fmt_it + 1;

        if(peek_fmt_it2 == fmt_it_end) {
          print(rune1);
          goto break_main_loop;
        }

        const auto rune2 = peek_fmt_it2->rune;

        // Just a normal space.
        if(rune2 != '}') {
          print(rune1);
          break; // Process 2nd rune on next iteration.
        }

        const auto peek_fmt_it3 = peek_fmt_it2 + 1;
        const auto rune3 = (peek_fmt_it3 != fmt_it_end) ? peek_fmt_it3->rune : 0;

        // Escaped: ` }}`
        if(rune3 == '}') {
          print(rune1);
          print(rune2);

          fmt_it = peek_fmt_it3;
        }
        // Styled text: ` }`
        else if(!color_stack.empty()) {
          color_stack.pop();
          ren.begin_color(!color_stack.empty() ? color_stack.top() : ren.curr_color_);

          fmt_it = peek_fmt_it2;
        }
      } break;

      // Handles: non-color `}`, `}}`
      case '}': {
        print(rune1);

        const auto peek_fmt_it2 = fmt_it + 1;

        if(peek_fmt_it2 == fmt_it_end) {
          goto break_main_loop;
        }

        // Escaped: `}}`.
        if(peek_fmt_it2->rune == '}') {
          fmt_it = peek_fmt_it2;
        }
      } break;

      case '\n':
        puts();
        break;

      default:
        print(rune1);
        break;
    }
  }

break_main_loop:
  ren.begin_color(ren.curr_color_);

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts() { return puts_blanks(1); }

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(char32_t rune) {
  return print(rune).puts();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts(std::string_view str) {
  return print(str).puts();
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts_blanks(int count) {
  pos.x = init_pos.x;
  pos.y += ((rune_size.h + spacing.h) * count);

  return *this;
}

Renderer::FontAtlasWrapper& Renderer::FontAtlasWrapper::puts_fmt(
  std::string_view fmt,std::initializer_list<std::string_view> args
) {
  return print_fmt(fmt,args).puts();
}

Size2i Renderer::FontAtlasWrapper::calc_total_size(const Size2i& str_size) const {
  return Size2i{
    (str_size.w * rune_size.w) + ((str_size.w - 1) * spacing.w) + (bg_padding_.w << 1),
    (str_size.h * rune_size.h) + ((str_size.h - 1) * spacing.h) + (bg_padding_.h << 1)
  };
}

void Renderer::FontAtlasWrapper::set_bg_padding(const Size2i& padding) {
  // Remove the previous padding (if any) and add the new padding.
  init_pos.x = (init_pos.x - bg_padding_.w) + padding.w;
  init_pos.y = (init_pos.y - bg_padding_.h) + padding.h;
  pos.x = (pos.x - bg_padding_.w) + padding.w;
  pos.y = (pos.y - bg_padding_.h) + padding.h;

  bg_padding_ = padding;
}

const Size2i& Renderer::FontAtlasWrapper::bg_padding() const { return bg_padding_; }

} // namespace cybel

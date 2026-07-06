/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "texture.h"

#include "cybel/gfx/gl_util.h"
#include "cybel/types/cybel_error.h"

#include <algorithm>

namespace cybel {

Texture::Texture(AssetManKey,const Image& image)
  : size_{image.size()} {
  GlUtil::clear_errors();

  const auto bypp = image.bytes_per_pixel();
  GLenum image_fmt = GL_RGBA;
  constexpr GLenum image_type = GL_UNSIGNED_BYTE;

  switch(bypp) {
    case 4:
      image_fmt = image.is_red_first() ? GL_RGBA : GL_BGRA;
      break;

    case 3:
      image_fmt = image.is_red_first() ? GL_RGB : GL_BGR;
      break;

    default:
      throw CybelError{"Unsupported Bytes Per Pixel `{}` for Image `{}`.",bypp,image.id()};
  }

  glGenTextures(1,&handle_);
  glBindTexture(GL_TEXTURE_2D,handle_);

  // I didn't have any problems without this, but could be needed.
  // - https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
  if(bypp <= 3) {
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  } else {
    glPixelStorei(GL_UNPACK_ALIGNMENT,4); // Should be the default.
  }

  if(GLEW_VERSION_3_0) {
    // See: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
  }

  // See: https://open.gl/textures
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

  {
    const auto pixels_guard = image.lock_pixels();

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,size_.w,size_.h,0,image_fmt,image_type,pixels_guard.pixels());
  }

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  const GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    // Just eat the errors, so a blank texture is shown instead of crashing.
    std::cerr << "[WARN] Failed to gen/bind Texture for Image `" << image.id()
              << "`; error `" << error << "`: " << GlUtil::fetch_error_str(error) << '.' << std::endl;
    GlUtil::clear_errors();
  }
}

Texture::Texture(AssetManKey key,const Color4f& color)
  : Texture{key,Size2i{2,2},color} {}

Texture::Texture(AssetManKey,const Size2i& size,const Color4f& color)
  : size_{std::max(size.w,1),std::max(size.h,1)} {
  GlUtil::clear_errors();

  const auto r = color.byte_r();
  const auto g = color.byte_g();
  const auto b = color.byte_b();
  const auto a = color.byte_a();

  constexpr std::uint8_t bypp = 4;
  const auto pixel_count = static_cast<std::size_t>(size_.area()) * bypp;
  const auto pixels = std::make_unique<GLubyte[]>(pixel_count);
  const auto* pixels_end = pixels.get() + pixel_count;

  for(auto* p = pixels.get(); p < pixels_end; p += bypp) {
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = a;
  }

  glGenTextures(1,&handle_);
  glBindTexture(GL_TEXTURE_2D,handle_);

  if(GLEW_VERSION_3_0) {
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
  }

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,size_.w,size_.h,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels.get());

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  const GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    // Just eat the errors, so a blank texture is shown instead of crashing.
    std::cerr << "[WARN] Failed to gen/bind Texture for Color " << color.to_byte_str()
              << "; error `" << error << "`: " << GlUtil::fetch_error_str(error) << '.' << std::endl;
    GlUtil::clear_errors();
  }
}

Texture::Texture(Texture&& other) noexcept {
  move_from(std::move(other));
}

void Texture::move_from(Texture&& other) noexcept {
  destroy();

  handle_ = std::exchange(other.handle_,0);
  size_ = std::exchange(other.size_,Size2i{});
}

Texture::~Texture() noexcept {
  destroy();
}

void Texture::destroy() noexcept {
  if(handle_ != 0) {
    glDeleteTextures(1,&handle_);
    handle_ = 0;
  }
}

Texture& Texture::operator=(Texture&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }
  return *this;
}

void Texture::zombify(AssetManKey) noexcept {
  handle_ = 0;
}

const Size2i& Texture::size() const { return size_; }

GLuint Texture::handle() const { return handle_; }

} // namespace cybel

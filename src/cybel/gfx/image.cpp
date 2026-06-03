/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "image.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Image::Image(AssetManKey,const std::filesystem::path& file)
  : id_{file} {
  const auto file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  handle_ = IMG_Load(file_cstr);

  if(!handle_) {
    throw CybelError{"Failed to load Image file `",file_cstr,"`: ",Util::get_sdl_img_error(),'.'};
  }

  size_ = Size2i{std::max(handle_->w,0),std::max(handle_->h,0)};
}

Image::Image(const Size2i& size,const Color4f& color)
  : id_{"Color4f://" + color.to_byte_str()},
    size_{size} {
  handle_ = SDL_CreateRGBSurfaceWithFormat(0,size_.w,size_.h,32,SDL_PIXELFORMAT_RGBA32);

  if(!handle_) {
    throw CybelError{"Failed to create Image from ",id_,": ",Util::get_sdl_img_error(),'.'};
  }

  const auto pixel = SDL_MapRGBA(
    handle_->format,
    color.byte_r(),
    color.byte_g(),
    color.byte_b(),
    color.byte_a()
  );

  // Null rect to fill entire surface.
  SDL_FillRect(handle_,nullptr,pixel);
}

Image::Image(Image&& other) noexcept {
  move_from(std::move(other));
}

void Image::move_from(Image&& other) noexcept {
  destroy();

  id_ = std::exchange(other.id_,"");
  handle_ = std::exchange(other.handle_,nullptr);
  size_ = std::exchange(other.size_,Size2i{});
  is_locked_ = std::exchange(other.is_locked_,false);
}

Image::~Image() noexcept {
  destroy();
}

void Image::destroy() noexcept {
  if(handle_) {
    unlock();
    SDL_FreeSurface(handle_);
    handle_ = nullptr;
  }
}

Image& Image::operator=(Image&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }
  return *this;
}

Image Image::dup() const {
  Image image{};
  image.id_ = id_;
  image.handle_ = SDL_DuplicateSurface(handle_);
  image.size_ = size_;

  if(!image.handle_) {
    throw CybelError{"Failed to duplicate Image `",id_,"`: ",Util::get_sdl_img_error(),'.'};
  }

  return image;
}

Image& Image::make_weird() {
  edit_pixels([](Color4f& c) {
    std::swap(c.r,c.b);
  });

  return *this;
}

Image& Image::colorize(const Color4f& to_color) {
  // NOTE: Don't change the alpha values of the image.
  edit_pixels([&](Color4f& c) {
    c.r = std::clamp(c.r * (to_color.r / 0.5f),0.0f,1.0f);
    c.g = std::clamp(c.g * (to_color.g / 0.5f),0.0f,1.0f);
    c.b = std::clamp(c.b * (to_color.b / 0.5f),0.0f,1.0f);
  });

  return *this;
}

Image& Image::edit_pixels(const EditPixel& edit_pixel) {
  if(SDL_PIXELTYPE(handle_->format->format) != SDL_PIXELTYPE_PACKED32) {
    // Convert to a surface we can work with.
    SDL_Surface* new_handle = SDL_ConvertSurfaceFormat(handle_,SDL_PIXELFORMAT_RGBA32,0);

    if(!new_handle) {
      std::cerr << "[WARN] Failed to convert Image `" << id_ << "` for editing pixels: "
                << Util::get_sdl_error() << '.' << std::endl;
      return *this;
    }

    destroy();
    handle_ = new_handle;
    size_ = Size2i{std::max(handle_->w,0),std::max(handle_->h,0)};
  }

  try {
    lock();
  } catch(const CybelError& e) {
    std::cerr << "[WARN] Failed to edit pixels: " << e.what() << std::endl;
    return *this;
  }

  const int area = size_.area();
  auto* pixels = static_cast<Uint32*>(handle_->pixels);

  for(int i = 0; i < area; ++i) {
    const auto pixel = pixels[i];
    Uint8 r{},g{},b{},a{};

    SDL_GetRGBA(pixel,handle_->format,&r,&g,&b,&a);
    auto color = Color4f::bytes(r,g,b,a);

    edit_pixel(color);

    pixels[i] = SDL_MapRGBA(
      handle_->format,
      color.byte_r(),
      color.byte_g(),
      color.byte_b(),
      color.byte_a()
    );
  }

  unlock();
  return *this;
}

void Image::lock() const {
  if(is_locked_ || !SDL_MUSTLOCK(handle_)) { return; }

  if(SDL_LockSurface(handle_) != 0) {
    throw CybelError{"Failed to lock Image `",id_,"`: ",Util::get_sdl_error(),'.'};
  }

  is_locked_ = true;
}

void Image::unlock() const noexcept {
  if(is_locked_) {
    SDL_UnlockSurface(handle_);
    is_locked_ = false;
  }
}

const std::string& Image::id() const { return id_; }

const Size2i& Image::size() const { return size_; }

std::uint8_t Image::bytes_per_pixel() const { return handle_->format->BytesPerPixel; }

bool Image::is_red_first() const { return handle_->format->Rmask == 0x000000ff; }

Image::PixelsGuard Image::lock_pixels() const { return PixelsGuard{*this}; }

const void* Image::pixels() const { return handle_->pixels; }

SDL_Surface* Image::handle() const { return handle_; }

Image::PixelsGuard::PixelsGuard(const Image& image)
  : image_{image} {
  image_.get().lock();
}

Image::PixelsGuard::~PixelsGuard() noexcept {
  image_.get().unlock();
}

const void* Image::PixelsGuard::pixels() const { return image_.get().pixels(); }

} // namespace cybel

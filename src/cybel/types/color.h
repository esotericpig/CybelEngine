/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_COLOR_H_
#define CYBEL_TYPES_COLOR_H_

#include "cybel/common.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace cybel {

/// Color of floats in range [0.0,1.0].
class Color4f final {
public:
  static const Color4f kNone;
  static const Color4f kBlack;
  static const Color4f kBlue;
  static const Color4f kBrown;
  static const Color4f kCopper;
  static const Color4f kCyan;
  static const Color4f kGreen;
  static const Color4f kHotPink;
  static const Color4f kMagenta;
  static const Color4f kPink;
  static const Color4f kPurple;
  static const Color4f kRed;
  static const Color4f kWhite;
  static const Color4f kYellow;

  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  float a = 1.0f;

  static constexpr Color4f hex(std::uint32_t rgb,std::uint8_t a = 255) noexcept;
  static Color4f hex(std::string_view str,const Color4f& fallback = kWhite) noexcept;
  static constexpr Color4f bytes(std::uint8_t rgb,std::uint8_t a = 255) noexcept;
  static constexpr Color4f bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255) noexcept;

  explicit constexpr Color4f() noexcept = default;
  explicit constexpr Color4f(float rgb,float a = 1.0f) noexcept;
  explicit constexpr Color4f(float r,float g,float b,float a = 1.0f) noexcept;

  constexpr auto operator<=>(const Color4f& other) const = default;

  constexpr Color4f& set(float rgb);
  constexpr Color4f& set(float rgb,float a);
  constexpr Color4f& set(float r,float g,float b);
  constexpr Color4f& set(float r,float g,float b,float a);
  constexpr Color4f& set_hex(std::uint32_t rgb);
  constexpr Color4f& set_hex(std::uint32_t rgb,std::uint8_t a);
  constexpr Color4f& set_bytes(std::uint8_t rgb);
  constexpr Color4f& set_bytes(std::uint8_t rgb,std::uint8_t a);
  constexpr Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b);
  constexpr Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a);

  constexpr Color4f with_r(float r) const;
  constexpr Color4f with_g(float g) const;
  constexpr Color4f with_b(float b) const;
  constexpr Color4f with_a(float a) const;
  constexpr Color4f with_byte_r(std::uint8_t r) const;
  constexpr Color4f with_byte_g(std::uint8_t g) const;
  constexpr Color4f with_byte_b(std::uint8_t b) const;
  constexpr Color4f with_byte_a(std::uint8_t a) const;

  std::uint8_t byte_r() const;
  std::uint8_t byte_g() const;
  std::uint8_t byte_b() const;
  std::uint8_t byte_a() const;

  std::string to_str() const;
  std::string to_hex_str() const;
  std::string to_byte_str() const;
};

constexpr Color4f Color4f::hex(std::uint32_t rgb,std::uint8_t a) noexcept {
  return Color4f{}.set_hex(rgb,a);
}

constexpr Color4f Color4f::bytes(std::uint8_t rgb,std::uint8_t a) noexcept {
  return Color4f{}.set_bytes(rgb,a);
}

constexpr Color4f Color4f::bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) noexcept {
  return Color4f{}.set_bytes(r,g,b,a);
}

constexpr Color4f::Color4f(float rgb,float a) noexcept {
  set(rgb,a);
}

constexpr Color4f::Color4f(float r,float g,float b,float a) noexcept {
  set(r,g,b,a);
}

constexpr Color4f& Color4f::set(float rgb) { return set(rgb,a); }

constexpr Color4f& Color4f::set(float rgb,float a) { return set(rgb,rgb,rgb,a); }

constexpr Color4f& Color4f::set(float r,float g,float b) { return set(r,g,b,a); }

constexpr Color4f& Color4f::set(float r,float g,float b,float a) {
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;

  return *this;
}

constexpr Color4f& Color4f::set_hex(std::uint32_t rgb) {
  return set_bytes(
    static_cast<std::uint8_t>((rgb >> 16) & 0xff),
    static_cast<std::uint8_t>((rgb >>  8) & 0xff),
    static_cast<std::uint8_t>((rgb      ) & 0xff)
  );
}

constexpr Color4f& Color4f::set_hex(std::uint32_t rgb,std::uint8_t a) {
  return set_bytes(
    static_cast<std::uint8_t>((rgb >> 16) & 0xff),
    static_cast<std::uint8_t>((rgb >>  8) & 0xff),
    static_cast<std::uint8_t>((rgb      ) & 0xff),
    a
  );
}

constexpr Color4f& Color4f::set_bytes(std::uint8_t rgb) { return set_bytes(rgb,rgb,rgb); }

constexpr Color4f& Color4f::set_bytes(std::uint8_t rgb,std::uint8_t a) { return set_bytes(rgb,rgb,rgb,a); }

constexpr Color4f& Color4f::set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b) {
  return set(
    static_cast<float>(r) / 255.0f,
    static_cast<float>(g) / 255.0f,
    static_cast<float>(b) / 255.0f
  );
}

constexpr Color4f& Color4f::set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) {
  return set(
    static_cast<float>(r) / 255.0f,
    static_cast<float>(g) / 255.0f,
    static_cast<float>(b) / 255.0f,
    static_cast<float>(a) / 255.0f
  );
}

constexpr Color4f Color4f::with_r(float r) const { return Color4f{r,g,b,a}; }

constexpr Color4f Color4f::with_g(float g) const { return Color4f{r,g,b,a}; }

constexpr Color4f Color4f::with_b(float b) const { return Color4f{r,g,b,a}; }

constexpr Color4f Color4f::with_a(float a) const { return Color4f{r,g,b,a}; }

constexpr Color4f Color4f::with_byte_r(std::uint8_t r) const {
  return Color4f{static_cast<float>(r) / 255.0f,g,b,a};
}

constexpr Color4f Color4f::with_byte_g(std::uint8_t g) const {
  return Color4f{r,static_cast<float>(g) / 255.0f,b,a};
}

constexpr Color4f Color4f::with_byte_b(std::uint8_t b) const {
  return Color4f{r,g,static_cast<float>(b) / 255.0f,a};
}

constexpr Color4f Color4f::with_byte_a(std::uint8_t a) const {
  return Color4f{r,g,b,static_cast<float>(a) / 255.0f};
}

constexpr Color4f Color4f::kNone{0.0f,0.0f};
constexpr Color4f Color4f::kBlack = hex(0x000000);
constexpr Color4f Color4f::kBlue = hex(0x0000ff);
constexpr Color4f Color4f::kBrown = hex(0x964b00);
constexpr Color4f Color4f::kCopper = hex(0xb87333);
constexpr Color4f Color4f::kCyan = hex(0x00ffff);
constexpr Color4f Color4f::kGreen = hex(0x00ff00);
constexpr Color4f Color4f::kHotPink = hex(0xff69b4);
constexpr Color4f Color4f::kMagenta = hex(0xff00ff);
constexpr Color4f Color4f::kPink = hex(0xffc0cb);
constexpr Color4f Color4f::kPurple = hex(0xff00ff);
constexpr Color4f Color4f::kRed = hex(0xff0000);
constexpr Color4f Color4f::kWhite = hex(0xffffff);
constexpr Color4f Color4f::kYellow = hex(0xffff00);

} // namespace cybel
#endif

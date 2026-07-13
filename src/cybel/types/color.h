/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_COLOR_H_
#define CYBEL_TYPES_COLOR_H_

#include "cybel/common.h"

namespace cybel {

/// Clamped floats to [0,1].
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

  static Color4f hex(std::uint32_t rgb,std::uint8_t a = 255) noexcept;
  static Color4f hex(std::string_view str,const Color4f& fallback = kWhite);
  static Color4f bytes(std::uint8_t rgb,std::uint8_t a = 255) noexcept;
  static Color4f bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255) noexcept;

  explicit Color4f() noexcept = default;
  explicit Color4f(float rgb,float a = 1.0f) noexcept;
  explicit Color4f(float r,float g,float b,float a = 1.0f) noexcept;

  auto operator<=>(const Color4f& other) const = default;

  Color4f& set(float rgb);
  Color4f& set(float rgb,float a);
  Color4f& set(float r,float g,float b);
  Color4f& set(float r,float g,float b,float a);
  Color4f& set_hex(std::uint32_t rgb);
  Color4f& set_hex(std::uint32_t rgb,std::uint8_t a);
  Color4f& set_bytes(std::uint8_t rgb);
  Color4f& set_bytes(std::uint8_t rgb,std::uint8_t a);
  Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b);
  Color4f& set_bytes(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a);

  Color4f with_r(float r) const;
  Color4f with_g(float g) const;
  Color4f with_b(float b) const;
  Color4f with_a(float a) const;
  Color4f with_byte_r(std::uint8_t r) const;
  Color4f with_byte_g(std::uint8_t g) const;
  Color4f with_byte_b(std::uint8_t b) const;
  Color4f with_byte_a(std::uint8_t a) const;

  std::uint8_t byte_r() const;
  std::uint8_t byte_g() const;
  std::uint8_t byte_b() const;
  std::uint8_t byte_a() const;

  std::string to_str() const;
  std::string to_hex_str() const;
  std::string to_byte_str() const;
};

inline const Color4f Color4f::kNone{0.0f,0.0f};
inline const Color4f Color4f::kBlack = hex(0x000000);
inline const Color4f Color4f::kBlue = hex(0x0000ff);
inline const Color4f Color4f::kBrown = hex(0x964b00);
inline const Color4f Color4f::kCopper = hex(0xb87333);
inline const Color4f Color4f::kCyan = hex(0x00ffff);
inline const Color4f Color4f::kGreen = hex(0x00ff00);
inline const Color4f Color4f::kHotPink = hex(0xff69b4);
inline const Color4f Color4f::kMagenta = hex(0xff00ff);
inline const Color4f Color4f::kPink = hex(0xffc0cb);
inline const Color4f Color4f::kPurple = hex(0xff00ff);
inline const Color4f Color4f::kRed = hex(0xff0000);
inline const Color4f Color4f::kWhite = hex(0xffffff);
inline const Color4f Color4f::kYellow = hex(0xffff00);

} // namespace cybel
#endif

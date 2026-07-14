/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "color.h"

#include "cybel/text/text_util.h"

#include <charconv>
#include <cmath>
#include <system_error>

namespace cybel {

Color4f Color4f::hex(std::string_view str,const Color4f& fallback) noexcept {
  if(str.length() >= 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
    str.remove_prefix(2);
  }
  if(str.empty()) { return fallback; }

  // RGBA: 11223344
  if(str.length() >= 8) {
    std::uint64_t rgba = 0;
    const auto ec = std::from_chars(str.data(),str.data() + str.size(),rgba,16).ec;

    if(ec == std::errc{}) {
      const auto rgb = static_cast<std::uint32_t>((rgba >> 8) & 0xffffff);
      const auto a = static_cast<std::uint8_t>(rgba & 0xff);

      return hex(rgb,a);
    }
  }
  // RGB: 112233
  else {
    std::uint32_t rgb = 0;
    const auto ec = std::from_chars(str.data(),str.data() + str.size(),rgb,16).ec;

    if(ec == std::errc{}) {
      return hex(rgb & 0xffffff);
    }
  }

  return fallback;
}

std::uint8_t Color4f::byte_r() const { return static_cast<std::uint8_t>(std::round(r * 255.0f)); }

std::uint8_t Color4f::byte_g() const { return static_cast<std::uint8_t>(std::round(g * 255.0f)); }

std::uint8_t Color4f::byte_b() const { return static_cast<std::uint8_t>(std::round(b * 255.0f)); }

std::uint8_t Color4f::byte_a() const { return static_cast<std::uint8_t>(std::round(a * 255.0f)); }

std::string Color4f::to_str() const {
  return TextUtil::fmt("({:.2f},{:.2f},{:.2f},{:.2f})",r,g,b,a);
}

std::string Color4f::to_hex_str() const {
  return TextUtil::fmt("0x{:02x}{:02x}{:02x}{:02x}",byte_r(),byte_g(),byte_b(),byte_a());
}

std::string Color4f::to_byte_str() const {
  return TextUtil::fmt("({},{},{},{})",byte_r(),byte_g(),byte_b(),byte_a());
}

} // namespace cybel

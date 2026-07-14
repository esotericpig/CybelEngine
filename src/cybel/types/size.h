/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_SIZE_H_
#define CYBEL_TYPES_SIZE_H_

#include "cybel/common.h"

#include "cybel/types/pos.h"

#include <ostream>
#include <type_traits>

namespace cybel {

template <typename T>
class Size2 {
public:
  T w{};
  T h{};

  constexpr auto operator<=>(const Size2& other) const = default;

  constexpr bool in_bounds(const Pos2<T>& pos,const Size2& size) const;

  constexpr Size2& set(T w,T h);

  constexpr T area() const;
  constexpr float aspect_ratio() const;

  template <typename T2>
  constexpr Size2<T2> to_size2() const;
};

using Size2f = Size2<float>;
using Size2i = Size2<int>;

static_assert(std::is_aggregate_v<Size2f>);
static_assert(std::is_aggregate_v<Size2i>);

template <typename T>
std::ostream& operator<<(std::ostream& out,const Size2<T>& size) {
  return out << size.w << 'x' << size.h;
}

template <typename T>
constexpr bool Size2<T>::in_bounds(const Pos2<T>& pos,const Size2& size) const {
  return ((pos.x + size.w) >= 0 && pos.x < w) &&
         ((pos.y + size.h) >= 0 && pos.y < h);
}

template <typename T>
constexpr Size2<T>& Size2<T>::set(T w,T h) {
  this->w = w;
  this->h = h;

  return *this;
}

template <typename T>
constexpr T Size2<T>::area() const { return w * h; }

template <typename T>
constexpr float Size2<T>::aspect_ratio() const {
  if(h == 0) [[unlikely]] { return 0.0f; }

  return static_cast<float>(w) / static_cast<float>(h);
}

template <typename T>
template <typename T2>
constexpr Size2<T2> Size2<T>::to_size2() const {
  return Size2<T2>{static_cast<T2>(w),static_cast<T2>(h)};
}

} // namespace cybel
#endif

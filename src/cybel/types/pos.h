/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_POS_H_
#define CYBEL_TYPES_POS_H_

#include "cybel/common.h"

#include <ostream>
#include <type_traits>

namespace cybel {

template <typename T>
class Pos3;

template <typename T>
class Pos2 {
public:
  T x{};
  T y{};

  constexpr auto operator<=>(const Pos2& other) const = default;

  constexpr Pos2 operator-() const;

  constexpr Pos2& set(T x,T y);

  template <typename T2>
  constexpr Pos2<T2> to_pos2() const;
  template <typename T2>
  constexpr Pos3<T2> to_pos3() const;
};

using Pos2f = Pos2<float>;
using Pos2i = Pos2<int>;

static_assert(std::is_aggregate_v<Pos2f>);
static_assert(std::is_aggregate_v<Pos2i>);

template <typename T>
class Pos3 {
public:
  T x{};
  T y{};
  T z{};

  constexpr auto operator<=>(const Pos3& other) const = default;

  constexpr Pos3 operator-() const;

  constexpr Pos3& set(T x,T y);
  constexpr Pos3& set(T x,T y,T z);

  template <typename T2>
  constexpr Pos2<T2> to_pos2() const;
  template <typename T2>
  constexpr Pos3<T2> to_pos3() const;
};

using Pos3f = Pos3<float>;
using Pos3i = Pos3<int>;

static_assert(std::is_aggregate_v<Pos3f>);
static_assert(std::is_aggregate_v<Pos3i>);

struct Pos4f {
  float x1{};
  float y1{};
  float x2{};
  float y2{};
};

struct Pos5f {
  float x1{};
  float y1{};
  float x2{};
  float y2{};
  float z{};
};

template <typename T>
std::ostream& operator<<(std::ostream& out,const Pos2<T>& pos) {
  return out << '(' << pos.x << ',' << pos.y << ')';
}

template <typename T>
std::ostream& operator<<(std::ostream& out,const Pos3<T>& pos) {
  return out << '(' << pos.x << ',' << pos.y << ',' << pos.z << ')';
}

template <typename T>
constexpr Pos2<T> Pos2<T>::operator-() const {
  return Pos2{-x,-y};
}

template <typename T>
constexpr Pos2<T>& Pos2<T>::set(T x,T y) {
  this->x = x;
  this->y = y;

  return *this;
}

template <typename T>
template <typename T2>
constexpr Pos2<T2> Pos2<T>::to_pos2() const {
  return Pos2<T2>{static_cast<T2>(x),static_cast<T2>(y)};
}

template <typename T>
template <typename T2>
constexpr Pos3<T2> Pos2<T>::to_pos3() const {
  return Pos3<T2>{static_cast<T2>(x),static_cast<T2>(y)};
}

template <typename T>
constexpr Pos3<T> Pos3<T>::operator-() const {
  return Pos3{-x,-y,-z};
}

template <typename T>
constexpr Pos3<T>& Pos3<T>::set(T x,T y) {
  this->x = x;
  this->y = y;

  return *this;
}

template <typename T>
constexpr Pos3<T>& Pos3<T>::set(T x,T y,T z) {
  this->x = x;
  this->y = y;
  this->z = z;

  return *this;
}

template <typename T>
template <typename T2>
constexpr Pos2<T2> Pos3<T>::to_pos2() const {
  return Pos2<T2>{static_cast<T2>(x),static_cast<T2>(y)};
}

template <typename T>
template <typename T2>
constexpr Pos3<T2> Pos3<T>::to_pos3() const {
  return Pos3<T2>{static_cast<T2>(x),static_cast<T2>(y),static_cast<T2>(z)};
}

} // namespace cybel
#endif

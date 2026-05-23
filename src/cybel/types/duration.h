/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TYPES_DURATION_H_
#define CYBEL_TYPES_DURATION_H_

#include "cybel/common.h"

#include <chrono>

namespace cybel {

class Timer;

class Duration final {
public:
  static const Duration kZero;

  static constexpr Duration from_millis(double millis) noexcept;
  static constexpr Duration from_secs(double secs) noexcept;
  static constexpr Duration from_fps(double fps) noexcept;

  explicit constexpr Duration() noexcept = default;

  constexpr auto operator<=>(const Duration& other) const = default;

  constexpr Duration operator+(const Duration& other) const;
  constexpr Duration& operator+=(const Duration& other);
  constexpr Duration operator-(const Duration& other) const;
  constexpr Duration& operator-=(const Duration& other);
  constexpr Duration operator*(double scalar) const;
  constexpr Duration& operator*=(double scalar);
  constexpr Duration operator/(double scalar) const;
  constexpr Duration& operator/=(double scalar);
  constexpr Duration operator%(double scalar) const;
  constexpr Duration& operator%=(double scalar);

  constexpr Duration& set_to_zero() noexcept;
  constexpr Duration& set_from_millis(double millis) noexcept;
  constexpr Duration& set_from_secs(double secs) noexcept;
  constexpr Duration& set_from_fps(double fps) noexcept;

  constexpr double millis() const;
  constexpr double secs() const;
  constexpr double fps() const;

  constexpr std::uint32_t round_millis() const;
  constexpr std::uint32_t round_secs() const;
  constexpr std::uint32_t round_fps() const;

  constexpr std::uint32_t whole_millis() const;
  constexpr std::uint32_t whole_secs() const;
  constexpr std::uint32_t whole_fps() const;

  friend class Timer;

private:
  using millis_t = std::chrono::duration<double,std::milli>;
  using secs_t = std::chrono::duration<double>;

  using value_t = millis_t;

  value_t value_{};

  explicit constexpr Duration(value_t value) noexcept;
};

constexpr Duration Duration::kZero{};

constexpr Duration Duration::from_millis(double millis) noexcept {
  return Duration{}.set_from_millis(millis);
}

constexpr Duration Duration::from_secs(double secs) noexcept {
  return Duration{}.set_from_secs(secs);
}

constexpr Duration Duration::from_fps(double fps) noexcept {
  return Duration{}.set_from_fps(fps);
}

constexpr Duration::Duration(value_t value) noexcept
  : value_{value} {}

constexpr Duration Duration::operator+(const Duration& other) const {
  return Duration{value_ + other.value_};
}

constexpr Duration& Duration::operator+=(const Duration& other) {
  value_ += other.value_;
  return *this;
}

constexpr Duration Duration::operator-(const Duration& other) const {
  return Duration{value_ - other.value_};
}

constexpr Duration& Duration::operator-=(const Duration& other) {
  value_ -= other.value_;
  return *this;
}

constexpr Duration Duration::operator*(double scalar) const {
  return Duration{value_ * scalar};
}

constexpr Duration& Duration::operator*=(double scalar) {
  value_ *= scalar;
  return *this;
}

constexpr Duration Duration::operator/(double scalar) const {
  return Duration{value_ / scalar};
}

constexpr Duration& Duration::operator/=(double scalar) {
  value_ /= scalar;
  return *this;
}

constexpr Duration Duration::operator%(double scalar) const {
  return Duration{value_t{std::fmod(value_.count(),scalar)}};
}

constexpr Duration& Duration::operator%=(double scalar) {
  value_ = value_t{std::fmod(value_.count(),scalar)};
  return *this;
}

constexpr Duration& Duration::set_to_zero() noexcept {
  value_ = value_t::zero();
  return *this;
}

constexpr Duration& Duration::set_from_millis(double millis) noexcept {
  value_ = millis_t{millis};
  return *this;
}

constexpr Duration& Duration::set_from_secs(double secs) noexcept {
  value_ = secs_t{secs};
  return *this;
}

constexpr Duration& Duration::set_from_fps(double fps) noexcept {
  return set_from_secs((fps > 0.0) ? (1.0 / fps) : 0.0);
}

constexpr double Duration::millis() const { return millis_t{value_}.count(); }

constexpr double Duration::secs() const { return secs_t{value_}.count(); }

constexpr double Duration::fps() const {
  const auto s = secs();
  return (s > 0.0) ? (1.0 / s) : 0.0;
}

constexpr std::uint32_t Duration::round_millis() const {
  return static_cast<std::uint32_t>(std::round(millis()));
}

constexpr std::uint32_t Duration::round_secs() const {
  return static_cast<std::uint32_t>(std::round(secs()));
}

constexpr std::uint32_t Duration::round_fps() const {
  return static_cast<std::uint32_t>(std::round(fps()));
}

constexpr std::uint32_t Duration::whole_millis() const {
  return static_cast<std::uint32_t>(millis());
}

constexpr std::uint32_t Duration::whole_secs() const {
  return static_cast<std::uint32_t>(secs());
}

constexpr std::uint32_t Duration::whole_fps() const {
  return static_cast<std::uint32_t>(fps());
}

} // namespace cybel
#endif

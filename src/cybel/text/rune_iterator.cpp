/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "rune_iterator.h"

#include "cybel/text/rune_util.h"

namespace cybel {

RuneIterator RuneIterator::begin(std::string_view str) {
  RuneIterator it{str};
  it.next_rune();

  return it;
}

RuneIterator RuneIterator::begin(std::string_view str,difference_type rune_offset) {
  RuneIterator it = (rune_offset >= 0) ? begin(str) : end(str);
  it += rune_offset;

  return it;
}

RuneIterator RuneIterator::end(std::string_view str) {
  RuneIterator it{str};
  it.token_.index = it.str_.size();
  it.token_.view = it.str_.substr(it.token_.index,0);

  return it;
}

RuneIterator RuneIterator::end(std::string_view str,difference_type rune_offset) {
  RuneIterator it = (rune_offset <= 0) ? end(str) : begin(str);
  it += rune_offset; // Same as begin(), on purpose.

  return it;
}

RuneIterator::RuneIterator(std::string_view str)
  : str_{str} {}

bool RuneIterator::operator==(const RuneIterator& other) const {
  return str_.data() == other.str_.data() &&
         str_.size() == other.str_.size() &&
         token_.index == other.token_.index;
}

std::strong_ordering RuneIterator::operator<=>(const RuneIterator& other) const {
  using order_t = std::strong_ordering;

  order_t order = (str_.data() <=> other.str_.data());
  if(order != order_t::equal) { return order; }

  order = (str_.size() <=> other.str_.size());
  if(order != order_t::equal) { return order; }

  return token_.index <=> other.token_.index;
}

RuneIterator::pointer RuneIterator::operator->() const {
  return &token_;
}

RuneIterator::reference RuneIterator::operator*() const {
  return token_;
}

RuneIterator& RuneIterator::operator++() {
  next_rune();

  return *this;
}

RuneIterator RuneIterator::operator++(int) {
  const auto old_it = *this;
  next_rune();

  return old_it;
}

RuneIterator RuneIterator::operator+(difference_type rune_count) const {
  auto it = *this;
  it += rune_count;

  return it;
}

RuneIterator& RuneIterator::operator+=(difference_type rune_count) {
  // Positive count.
  for(; rune_count > 0 && token_.index < str_.size(); --rune_count) {
    next_rune();
  }
  // Negative count.
  for(; rune_count < 0 && token_.index > 0; ++rune_count) {
    prev_rune();
  }
  // Zero count.

  return *this;
}

RuneIterator& RuneIterator::operator--() {
  prev_rune();

  return *this;
}

RuneIterator RuneIterator::operator--(int) {
  const auto old_it = *this;
  prev_rune();

  return old_it;
}

RuneIterator RuneIterator::operator-(difference_type rune_count) const {
  auto it = *this;
  it -= rune_count;

  return it;
}

RuneIterator& RuneIterator::operator-=(difference_type rune_count) {
  // Positive count.
  for(; rune_count > 0 && token_.index > 0; --rune_count) {
    prev_rune();
  }
  // Negative count.
  for(; rune_count < 0 && token_.index < str_.size(); ++rune_count) {
    next_rune();
  }
  // Zero count.

  return *this;
}

void RuneIterator::next_rune() {
  token_.index += token_.byte_count;

  if(token_.index < str_.size()) [[likely]] {
    token_.rune = RuneUtil::next_rune(str_,token_.index,token_.byte_count);
    token_.view = str_.substr(token_.index,token_.byte_count);
  } else [[unlikely]] {
    token_.index = str_.size();
    token_.rune = RuneUtil::kInvalidRune;
    token_.view = str_.substr(token_.index,0);
  }
}

void RuneIterator::prev_rune() {
  if(token_.index == 0) [[unlikely]] {
    token_.rune = RuneUtil::kInvalidRune;
    token_.view = str_.substr(token_.index,0);
    return;
  }

  --token_.index;
  token_.rune = RuneUtil::prev_rune(str_,token_.index,token_.byte_count);
  token_.view = str_.substr(token_.index,token_.byte_count);

  if(token_.byte_count <= token_.index) [[likely]] {
    // +1 since we decrement the index before calling RuneUtil::prev_rune().
    token_.index = token_.index - token_.byte_count + 1;
  } else [[unlikely]] {
    token_.index = 0;
  }
}

std::string_view RuneIterator::str() const { return str_; }

} // namespace cybel

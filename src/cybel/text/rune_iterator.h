/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TEXT_RUNE_ITERATOR_H_
#define CYBEL_TEXT_RUNE_ITERATOR_H_

#include "cybel/common.h"

#include "cybel/text/rune_token.h"

#include <cstddef>
#include <iterator>
#include <string_view>

namespace cybel {

class RuneIterator final {
public:
  using iterator_concept = std::bidirectional_iterator_tag;
  using iterator_category = std::bidirectional_iterator_tag;

  using value_type = RuneToken;
  using difference_type = std::ptrdiff_t;
  using pointer = const RuneToken*;
  using reference = const RuneToken&;

  static RuneIterator begin(std::string_view str);
  static RuneIterator begin(std::string_view str,difference_type rune_offset);
  static RuneIterator end(std::string_view str);
  /// For `rune_offset`, use a negative number to start from the end, just like begin().
  static RuneIterator end(std::string_view str,difference_type rune_offset);

  explicit RuneIterator() = default;

  bool operator==(const RuneIterator& other) const;
  std::strong_ordering operator<=>(const RuneIterator& other) const;

  pointer operator->() const;
  reference operator*() const;

  RuneIterator& operator++();
  RuneIterator operator++(int);
  RuneIterator operator+(difference_type rune_count) const;
  RuneIterator& operator+=(difference_type rune_count);
  RuneIterator& operator--();
  RuneIterator operator--(int);
  RuneIterator operator-(difference_type rune_count) const;
  RuneIterator& operator-=(difference_type rune_count);

  std::string_view str() const;

private:
  std::string_view str_{};
  value_type token_{};

  explicit RuneIterator(std::string_view str);

  void next_rune();
  void prev_rune();
};

static_assert(std::bidirectional_iterator<RuneIterator>);

} // namespace cybel
#endif

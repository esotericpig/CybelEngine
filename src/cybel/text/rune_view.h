/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_TEXT_RUNE_VIEW_H_
#define CYBEL_TEXT_RUNE_VIEW_H_

#include "cybel/common.h"

#include "cybel/text/rune_iterator.h"

#include <ranges>

namespace cybel {

class RuneView final : public std::ranges::view_interface<RuneView> {
public:
  using value_type = RuneIterator::value_type;
  using difference_type = RuneIterator::difference_type;
  using pointer = RuneIterator::pointer;
  using reference = RuneIterator::reference;

  using iterator = RuneIterator;
  using const_iterator = RuneIterator;

  /// For both `begin_rune_range` & `end_rune_range`, a positive number starts from the beginning,
  /// and a negative number starts from the end.
  explicit RuneView(std::string_view str,difference_type begin_rune_range = 0,
                    difference_type end_rune_range = 0);

  explicit RuneView() = default;

  iterator begin() const;
  iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  bool empty() const;

private:
  std::string_view str_{};
  difference_type begin_rune_range_ = 0;
  difference_type end_rune_range_ = 0;
};

static_assert(std::ranges::bidirectional_range<RuneView>);

} // namespace cybel
#endif

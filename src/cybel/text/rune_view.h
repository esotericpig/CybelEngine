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
  using const_iterator = RuneIterator;
  using const_reverse_iterator = RuneIterator::reverse_iterator;

  explicit RuneView() = default;
  explicit RuneView(std::string_view str,std::size_t begin_rune_count = 0,std::size_t end_rune_count = 0);

  RuneIterator begin() const;
  RuneIterator end() const;
  RuneIterator cbegin() const;
  RuneIterator cend() const;

  RuneIterator::reverse_iterator rbegin() const;
  RuneIterator::reverse_iterator rend() const;
  RuneIterator::reverse_iterator crbegin() const;
  RuneIterator::reverse_iterator crend() const;

private:
  std::string_view str_{};
  std::size_t begin_rune_count_ = 0;
  std::size_t end_rune_count_ = 0;
};

} // namespace cybel
#endif

/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "rune_view.h"

namespace cybel {

RuneView::RuneView(std::string_view str,std::size_t begin_rune_count,std::size_t end_rune_count)
  : str_(str),begin_rune_count_(begin_rune_count),end_rune_count_(end_rune_count) {}

RuneIterator RuneView::begin() const {
  return RuneIterator::begin(str_,begin_rune_count_);
}

RuneIterator RuneView::end() const {
  return RuneIterator::end(str_,end_rune_count_);
}

RuneIterator RuneView::cbegin() const {
  return begin();
}

RuneIterator RuneView::cend() const {
  return end();
}

RuneIterator::reverse_iterator RuneView::rbegin() const {
  return RuneIterator::rbegin(str_,begin_rune_count_);
}

RuneIterator::reverse_iterator RuneView::rend() const {
  return RuneIterator::rend(str_,end_rune_count_);
}

RuneIterator::reverse_iterator RuneView::crbegin() const {
  return rbegin();
}

RuneIterator::reverse_iterator RuneView::crend() const {
  return rend();
}

} // namespace cybel

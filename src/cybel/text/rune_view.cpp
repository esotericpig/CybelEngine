/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "rune_view.h"

namespace cybel {

RuneView::RuneView(std::string_view str,difference_type begin_rune_range,difference_type end_rune_range)
  : str_{str},
    begin_rune_range_{begin_rune_range},
    end_rune_range_{end_rune_range} {}

RuneView::iterator RuneView::begin() const {
  return iterator::begin(str_,begin_rune_range_);
}

RuneView::iterator RuneView::end() const {
  return iterator::end(str_,end_rune_range_);
}

RuneView::const_iterator RuneView::cbegin() const {
  return begin();
}

RuneView::const_iterator RuneView::cend() const {
  return end();
}

bool RuneView::empty() const { return str_.empty(); }

} // namespace cybel

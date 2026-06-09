/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "text_util.h"

#include "cybel/text/rune_iterator.h"
#include "cybel/text/rune_util.h"

#include <cctype>

namespace cybel {

int TextUtil::casecmp_ascii(std::string_view str1,std::string_view str2) {
  const std::size_t size1 = str1.size();
  const std::size_t size2 = str2.size();
  const std::size_t min_size = std::min(size1,size2);

  for(std::size_t i = 0; i < min_size; ++i) {
    // `int` & `unsigned char` for std::tolower().
    int c1 = static_cast<unsigned char>(str1[i]);
    int c2 = static_cast<unsigned char>(str2[i]);

    if(c1 <= RuneUtil::kMaxAsciiOctet) { c1 = std::tolower(c1); }
    if(c2 <= RuneUtil::kMaxAsciiOctet) { c2 = std::tolower(c2); }

    if(c1 < c2) { return -1; }
    if(c1 > c2) { return 1; }
  }

  if(size1 < size2) { return -1; }
  if(size1 > size2) { return 1; }
  return 0;
}

std::size_t TextUtil::count_runes(std::string_view str) {
  if(str.empty()) { return 0; }

  std::size_t rune_count = 0;
  std::uint8_t byte_count = 0;

  for(std::size_t i = 0; i < str.size(); i += byte_count) {
    RuneUtil::next_rune(str,i,byte_count);
    ++rune_count;
  }

  return rune_count;
}

std::string TextUtil::ellipsize(std::string_view str,std::size_t max_len) {
  if(max_len == 0 || str.empty()) { return ""; }
  if(str.size() <= max_len) { return std::string{str}; }

  std::size_t len = 0;
  std::size_t index = 0;
  std::uint8_t byte_count = 0;

  do {
    RuneUtil::next_rune(str,index,byte_count);

    if((++len) > max_len) { break; }

    index += byte_count;
  } while(index < str.size());

  // No ellipsis necessary or only 1 rune?
  if(len <= max_len || max_len <= 1) {
    return std::string{str.substr(0,index)};
  }

  // Chop off last rune for ellipsis.
  return std::string{str.substr(0,index - byte_count)} + "…";
}

std::string TextUtil::ljust(std::string_view str,std::size_t new_len) {
  if(new_len == 0 || (new_len == 1 && !str.empty())) {
    return std::string{str};
  }

  auto len = count_runes(str);
  if(len >= new_len) { return std::string{str}; }

  len = new_len - len;

  std::string result{};
  result.reserve(str.size() + len);

  result += str;
  result += std::string(len,' ');

  return result;
}

std::string_view TextUtil::strip(std::string_view str) {
  if(str.empty()) { return ""; }

  std::size_t first_index = 0;
  std::size_t last_index = str.size();
  std::uint8_t byte_count = 0;

  // lstrip().
  for(; first_index < last_index; first_index += byte_count) {
    const auto rune = RuneUtil::next_rune(str,first_index,byte_count);

    if(!RuneUtil::is_whitespace(rune)) { break; }
  }

  // rstrip().
  for(byte_count = 1; last_index > first_index;) {
    last_index -= byte_count;
    const auto rune = RuneUtil::prev_rune(str,last_index,byte_count);

    if(!RuneUtil::is_whitespace(rune)) { break; }
  }

  // Safe to use a larger count in substr().
  return str.substr(first_index,last_index - first_index + 1);
}

std::string TextUtil::wrap_words(std::string_view str,std::size_t max_len) {
  if(max_len == 0) { max_len = 1; }
  if(str.size() <= max_len) { return std::string{str}; }

  std::string result{};
  const auto it_end = RuneIterator::end(str);

  bool is_line_start = true;
  std::size_t line_len = 0;
  std::size_t word_i = 0;
  std::size_t word_len = 0;
  std::string_view word{};

  // Bunch of funky logic here, just to avoid calling next_rune() more than necessary.
  for(auto it = RuneIterator::begin(str); it != it_end;) {
    // First, read/eat all spaces.
    do {
      if(it->rune == '\n' || it->rune == '\r') { goto handle_newlines; }
      if(!RuneUtil::is_whitespace(it->rune)) { break; }

      if(is_line_start) {
        result += it->view;
        ++line_len;
      }
    } while((++it) != it_end);

    if(it == it_end) { break; }

    // Second, gather word.
    word_i = it->index;
    word_len = 1;

    for(++it; it != it_end; ++it) {
      if(it->rune == '\n' || it->rune == '\r') { break; } // Append word.
      if(RuneUtil::is_whitespace(it->rune)) { break; }

      ++word_len;
    }

    word = str.substr(word_i,it->index - word_i);

    // Third, append word appropriately.
    if(is_line_start) {
      result += word;
      is_line_start = false;
      line_len += word_len;
    } else {
      line_len += 1 + word_len; // +1 for space between words.

      if(line_len <= max_len) {
        result += ' '; // Space between words, if not first word.
      } else {
        result += '\n';
        line_len = word_len;
      }

      result += word;
    }

    if(it == it_end) { break; }

    // Fourth, check for newlines.
  handle_newlines:
    while(it->rune == '\n' || it->rune == '\r') {
      // Check for CRLF.
      if(it->rune == '\r') {
        const auto peek_it = it + 1;

        if(peek_it != it_end && peek_it->rune == '\n') {
          it = peek_it;
        }
      }

      result += '\n';
      is_line_start = true;
      line_len = 0;

      if((++it) == it_end) { break; }
    }
  }

  return result;
}

} // namespace cybel

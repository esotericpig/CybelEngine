/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_IO_TEXT_READER_BUF_H_
#define CYBEL_IO_TEXT_READER_BUF_H_

#include "cybel/common.h"

#include <filesystem>
#include <streambuf>
#include <vector>

namespace cybel {

/// Implemented:
/// - Reading.              - underflow()
/// - peek()                - underflow()
///
/// Not implemented:
/// - Seeking.              - seekoff(), seekpos()
/// - Writing.              - overflow()
/// - unget(), putback()    - pbackfail()
///   - This is kind of implemented until the buffer is empty, but not recommended to be used.
class TextReaderBuf final : public std::streambuf {
public:
  explicit TextReaderBuf(const std::filesystem::path& file,std::size_t buffer_size = 4 * 1024);
  ~TextReaderBuf() noexcept override;

  TextReaderBuf(const TextReaderBuf& other) = delete;
  TextReaderBuf& operator=(const TextReaderBuf& other) = delete;

  TextReaderBuf(TextReaderBuf&& other) noexcept;
  TextReaderBuf& operator=(TextReaderBuf&& other) noexcept;

  /// Mainly used for testing purposes.
  ///
  /// This can return false, yet there is still data in the buffer to read.
  bool is_open() const;

  SDL_RWops* handle() const;

protected:
  int_type underflow() override;

  /// Not implemented, since it could require seeking, which might not be supported by SDL_RWseek().
  /// However, you can still do unget()/putback() until the buffer is empty.
  /// After the buffer is empty, I believe it returns EOF (or the behavior is undefined).
  ///
  /// Always returns EOF.
  int_type pbackfail(int_type c) override;

  /// Not implemented, since this class is for reading only.
  ///
  /// Always returns EOF.
  int_type overflow(int_type c) override;

private:
  SDL_RWops* handle_ = nullptr;
  std::vector<char_type> buffer_{};

  void close() noexcept;
  void move_from(TextReaderBuf&& other) noexcept;
};

} // namespace cybel
#endif

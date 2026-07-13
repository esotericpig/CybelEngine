/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "text_reader_buf.h"

#include "cybel/types/cybel_error.h"

namespace cybel {

TextReaderBuf::TextReaderBuf(const std::filesystem::path& file,std::size_t buffer_size)
  : buffer_(std::max<std::size_t>(buffer_size,1),0) {
  const std::u8string file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  handle_ = SDL_RWFromFile(file_cstr,"r");

  if(!handle_) {
    throw CybelError{"Failed to open file `{}` for reading: {}.",file_cstr,SDL_GetError()};
  }

  // Init to needing data to read (begin=end).
  setg(buffer_.data(),buffer_.data(),buffer_.data()); // (begin,current,end)
}

TextReaderBuf::~TextReaderBuf() noexcept {
  close();
}

void TextReaderBuf::close() noexcept {
  if(handle_) {
    SDL_RWclose(handle_);
    handle_ = nullptr;
  }
}

TextReaderBuf::TextReaderBuf(TextReaderBuf&& other) noexcept
  : std::streambuf(std::move(other)) {
  move_from(std::move(other));
}

TextReaderBuf& TextReaderBuf::operator=(TextReaderBuf&& other) noexcept {
  if(this != &other) {
    close();
    std::streambuf::operator=(std::move(other));
    move_from(std::move(other));
  }

  return *this;
}

void TextReaderBuf::move_from(TextReaderBuf&& other) noexcept {
  handle_ = std::exchange(other.handle_,nullptr);
  buffer_ = std::move(other.buffer_);
}

TextReaderBuf::int_type TextReaderBuf::underflow() {
  if(gptr() < egptr()) { return traits_type::to_int_type(*gptr()); }

  // On failure/EOF, gptr() must equal egptr().
  setg(buffer_.data(),buffer_.data(),buffer_.data());

  if(!handle_) { return traits_type::eof(); }

  std::size_t read_count = buffer_.size();

  // Unfortunately, SDL2's SDL_RWread() returns 0 for either EOF or error,
  // with no info about how many bytes were read before EOF.
  //
  // Because of this, we have to rely on the last value in `buffer_` that is not 0.
  // Terrible design.
  std::ranges::fill(buffer_,0);
  SDL_ClearError();

  if(SDL_RWread(handle_,buffer_.data(),sizeof(char_type) * read_count,1) == 0) {
    close();

    const std::string_view error = SDL_GetError();

    if(!error.empty()) {
      std::cerr << "[WARN] Failed to read data: " << error << '.' << std::endl;
      // Don't fail; maybe it still read some data.
    }

    // Find the last non-zero value.
    for(; read_count > 0; --read_count) { // Unsigned loop.
      if(buffer_.at(read_count - 1) != 0) { break; }
    }

    if(read_count == 0) { return traits_type::eof(); }
  }

  setg(buffer_.data(),buffer_.data(),buffer_.data() + read_count);

  return traits_type::to_int_type(*gptr());
}

TextReaderBuf::int_type TextReaderBuf::pbackfail(int_type) {
  return traits_type::eof();
}

TextReaderBuf::int_type TextReaderBuf::overflow(int_type) {
  return traits_type::eof();
}

bool TextReaderBuf::is_open() const { return handle_; }

SDL_RWops* TextReaderBuf::handle() const { return handle_; }

} // namespace cybel

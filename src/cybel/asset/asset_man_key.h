/*
 * This file is part of Cybel Engine.
 * Copyright (c) 2026 Bradley Whited
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef CYBEL_ASSET_ASSET_MAN_KEY_H_
#define CYBEL_ASSET_ASSET_MAN_KEY_H_

#include "cybel/common.h"

namespace cybel {

class AssetMan;

/// Only AssetMan can create/zombify assets.
class AssetManKey final {
public:
  friend class AssetMan;

private:
  explicit constexpr AssetManKey() noexcept = default;
};

} // namespace cybel
#endif

#pragma once

struct nonmovable {
  nonmovable() = default;
  nonmovable(nonmovable&&) = delete;
  nonmovable& operator=(nonmovable&&) = delete;
};
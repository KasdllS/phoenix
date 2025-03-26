#pragma once

struct noncopyable {
  noncopyable() = default;
  noncopyable(const noncopyable&) = delete;
  const noncopyable& operator=(const noncopyable&) = delete;
};
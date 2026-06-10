
#ifndef HASH_LAB_COMMON_UTILS_HPP
#define HASH_LAB_COMMON_UTILS_HPP
#include <algorithm>
#include <bit>
#include <concepts>
#include <cstring>
#include <ranges>
#include <stdexcept>

namespace hash_lab {

template <std::integral T>
constexpr T byte_swap(T value) noexcept {
  static_assert(std::has_unique_object_representations_v<T>,
                "T may not have padding bits");
  auto value_representation =
    std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
  std::ranges::reverse(value_representation);
  return std::bit_cast<T>(value_representation);
}

template <std::integral T>
void WriteBE(std::span<std::byte> dest, T value) {
  static_assert(std::has_unique_object_representations_v<T>,
                "T may not have padding bits");
  if (sizeof(T) > dest.size()) [[unlikely]] {
    throw std::length_error("[WriteBE] destination it too small");
  }
  if constexpr (std::endian::native == std::endian::little) {
    std::ranges::reverse_copy(
      std::bit_cast<std::array<std::byte, sizeof(T)>>(value), dest.begin());
    return;
  }
  std::ranges::copy(std::bit_cast<std::array<std::byte, sizeof(T)>>(value),
                    dest.begin());
}

template <std::integral T>
T ReadBE(const std::span<const std::byte> src) {
  static_assert(std::has_unique_object_representations_v<T>,
                "T may not have padding bits");
  if (sizeof(T) > src.size()) [[unlikely]] {
    throw std::length_error("[ReadBE] src is too small");
  }
  std::array<std::byte, sizeof(T)> val_raw;
  std::ranges::copy_n(src.begin(), sizeof(T), val_raw.begin());
  if constexpr (std::endian::native == std::endian::little) {
    std::ranges::reverse(val_raw);
  }
  return std::bit_cast<T>(val_raw);
}

template <std::integral T>
T ReadAs(const std::span<const std::byte> src) {
  if (sizeof(T) > src.size()) [[unlikely]] {
    throw std::length_error("[ReadBE] src is too small");
  }
  T result;
  std::memcpy(&result, src.data(), sizeof(T));
  return result;
}

}  // namespace hash_lab

#endif  // HASH_LAB_COMMON_UTILS_HPP

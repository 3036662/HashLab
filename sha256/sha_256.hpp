
#ifndef HASH_LAB_SHA_256_HPP
#define HASH_LAB_SHA_256_HPP
#include <array>
#include <cstdint>
#include <string_view>

#include "common/data_formatter.hpp"

namespace hash_lab::sha256 {

class Sha256 {
 public:
  explicit Sha256(const std::span<const std::byte> raw_data)
    : fmt_(raw_data) {};

  explicit Sha256(const std::string_view data)
    : Sha256(std::span(reinterpret_cast<const std::byte *>(data.data()),
                       data.size())) {};

  // delete constructor from rvalue vector
  template <typename T>
    requires(!std::is_same_v<std::span<const std::byte>, T> &&
             !std::is_lvalue_reference_v<T>)
  explicit Sha256(T &&raw_data) = delete;

  [[nodiscard]] std::array<std::byte, 32> Calculate() const;

 private:
  DataFormatter fmt_;
};

}  // namespace hash_lab::sha256

#endif  // HASH_LAB_SHA_256_HPP

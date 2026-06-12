
#ifndef HASH_LAB_SHA_256_HPP
#define HASH_LAB_SHA_256_HPP
#include <array>
#include <cstdint>
#include <string_view>

#include "common/data_formatter.hpp"

namespace hash_lab::sha224_256 {

template <int SHA_TYPE>
class Sha224_256 {
  static_assert(SHA_TYPE == 224 || SHA_TYPE == 256, "Unsupported SHA_TYPE");
  using Ret = std::array<std::byte, SHA_TYPE / 8>;

 public:
  explicit Sha224_256(const std::span<const std::byte> raw_data)
    : fmt_(raw_data) {};

  explicit Sha224_256(const std::string_view data)
    : Sha224_256(std::span(reinterpret_cast<const std::byte *>(data.data()),
                           data.size())) {};

  // delete constructor from rvalue vector
  template <typename T>
    requires(!std::is_same_v<std::span<const std::byte>, T> &&
             !std::is_lvalue_reference_v<T>)
  explicit Sha224_256(T &&raw_data) = delete;

  [[nodiscard]] Ret Calculate() const;

 private:
  DataFormatter fmt_;
};

using Sha256 = Sha224_256<256>;
using Sha224 = Sha224_256<224>;

}  // namespace hash_lab::sha224_256

#endif  // HASH_LAB_SHA_256_HPP

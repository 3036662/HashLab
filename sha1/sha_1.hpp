#ifndef HASH_LAB_SHA_1_HPP
#define HASH_LAB_SHA_1_HPP
#include <array>

#include "common/common_utils.hpp"
#include "common/data_formatter.hpp"

namespace hash_lab::sha1 {

using Buffer5Words = std::array<std::uint32_t, 5>;
using Block80Words = std::array<std::uint32_t, 80>;

class Sha1 {
 public:
  explicit Sha1(const std::span<const std::byte> raw_data) : fmt_(raw_data) {};

  explicit Sha1(const std::string_view data)
    : Sha1(std::span(reinterpret_cast<const std::byte *>(data.data()),
                     data.size())) {};

  // delete constructor from rvalue vector
  template <typename T>
    requires(!std::is_same_v<std::span<const std::byte>, T> &&
             !std::is_lvalue_reference_v<T>)
  explicit Sha1(T &&raw_data) = delete;

  [[nodiscard]] std::array<std::byte, 20> Calculate() const;

 private:
  DataFormatter fmt_;
};

Block80Words CreateBlock80Words(DataBlock data_block);

Block80Words CreateBlock80WordsEx(const DataBlock data_block);

[[nodiscard]] uint32_t MagicFunc(uint32_t step, uint32_t b, uint32_t c,
                                 uint32_t d);

[[nodiscard]] uint32_t CircularLeftShift(uint32_t word, uint32_t shift);

[[nodiscard]] uint32_t GetK(uint32_t step);

}  // namespace hash_lab::sha1

#endif  // HASH_LAB_SHA_1_HPP


#ifndef HASH_LAB_DATA_FORMATTER_HPP
#define HASH_LAB_DATA_FORMATTER_HPP

#include <array>
#include <span>
#include <vector>

namespace hash_lab {

// Data block (512 bits)
constexpr size_t kBlockSize = 64;
constexpr size_t kLengthSize = 8;
constexpr std::byte kPaddingByte{0b10000000};
using DataBlock = std::span<const std::byte, kBlockSize>;
using DataBlockOwning = std::array<std::byte, kBlockSize>;
using VecBlocks = std::vector<DataBlock>;

/// @brief DataFormatter will prepare a data for hashing
/// @details does not own the data
class DataFormatter {
 public:
  explicit DataFormatter(std::span<const std::byte> raw_data);

  // delete constructor from rvalue vector
  template <typename T>
    requires(!std::is_same_v<std::span<const std::byte>, T> &&
             !std::is_lvalue_reference_v<T>)
  explicit DataFormatter(T &&raw_data) = delete;

  [[nodiscard]] const VecBlocks &blocks() const & { return blocks_; }

 private:
  void AddData(std::span<const std::byte> raw_data);
  void CreateLastBlocks(std::span<const std::byte> data_tail,
                        size_t msg_total_size);
  void AppendLastBlocks();

  std::vector<DataBlock> blocks_;
  std::vector<DataBlockOwning> last_blocks_;
};

}  // namespace hash_lab

#endif  // HASH_LAB_DATA_FORMATTER_HPP

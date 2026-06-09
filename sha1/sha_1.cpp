
#include "sha_1.hpp"

namespace hash_lab::sha1 {

DataFormatter::DataFormatter(std::span<const std::byte> raw_data) {
  AddData(std::move(raw_data));
}

void DataFormatter::AddData(std::span<const std::byte> raw_data) {
  last_blocks_.reserve(2);
  last_blocks_.resize(1);

  if (raw_data.size() > std::numeric_limits<uint64_t>::max() - 1) [[unlikely]] {
    throw std::length_error("[DataFormatter] data size too large");
  }

  // create one data block for an empty message
  if (raw_data.empty()) {
    last_blocks_[0][0] = kPaddingByte;
    AppendLastBlocks();
    return;
  }

  // create non owning data blocks
  const size_t data_size = raw_data.size();
  const size_t block_count = data_size / kBlockSize;
  const size_t rest_data_size = data_size % kBlockSize;
  blocks_.reserve(block_count + 2);

  // create non-owning view data blocks
  for (size_t block_index = 0; block_index < block_count; ++block_index) {
    blocks_.emplace_back(
      DataBlock{raw_data.begin() + block_index * kBlockSize, kBlockSize});
  }

  CreateLastBlocks(raw_data.subspan(block_count * kBlockSize, rest_data_size),
                   data_size);
  AppendLastBlocks();
}

void DataFormatter::CreateLastBlocks(std::span<const std::byte> data_tail,
                                     size_t msg_total_size) {
  const size_t data_size = data_tail.size();

  // raw_data.size() < 440 bit - create one data block
  if (data_size + 1 + kLengthSize <= kBlockSize) {
    std::ranges::copy(data_tail, last_blocks_[0].begin());
    last_blocks_[0][data_size] = kPaddingByte;
    // write the message length
    WriteBE(std::span<std::byte>(
              last_blocks_[0].begin() + kBlockSize - kLengthSize, kLengthSize),
            msg_total_size);
    return;
  }

  // if size > 440bit < size < 504bit
  // put the padding to first block
  // create a second block with length
  if (data_size + 1 < kBlockSize) {
    std::ranges::copy(data_tail, last_blocks_[0].begin());
    last_blocks_[0][data_size] = kPaddingByte;
    last_blocks_.resize(2);
    WriteBE(std::span(last_blocks_[1].begin() + kBlockSize - kLengthSize,
                      kLengthSize),
            msg_total_size);
    return;
  }
}

void DataFormatter::AppendLastBlocks() {
  // add last owning blocks to blocks_;
  std::ranges::transform(last_blocks_, std::back_inserter(blocks_),
                         [](const DataBlockOwning &block) {
                           return DataBlock{block.begin(), block.end()};
                         });
}

}  //  namespace hash_lab::sha1
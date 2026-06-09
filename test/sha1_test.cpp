#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "sha1/sha_1.hpp"

TEST_CASE("First test", "[smoke]") { REQUIRE(1 + 1 == 2); }

namespace sha1 = hash_lab::sha1;

TEST_CASE("DataFormatter") {
  SECTION("Empty") {
    const std::vector<std::byte> raw_data;
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 1);
    REQUIRE(blocks.at(0)[0] == sha1::kPaddingByte);
    REQUIRE(std::ranges::all_of(
      blocks.at(0).begin() + 1, blocks.at(0).end(),
      [](const std::byte val) { return val == std::byte{0}; }));
  }

  SECTION("55bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector<std::byte> raw_data(55, val);
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 1);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == sha1::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));
    REQUIRE(first_block[raw_data.size()] == sha1::kPaddingByte);

    REQUIRE(hash_lab::ReadBE<uint64_t>(first_block.subspan(
              sha1::kBlockSize - sha1::kLengthSize)) == raw_data.size());
  }

  SECTION("56bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector<std::byte> raw_data(56, val);
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 2);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == sha1::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));
    REQUIRE(first_block[raw_data.size()] == sha1::kPaddingByte);
    // bytes 56 - 64 must be 0
    const auto rest_bytes_of_first_block =
      std::span(first_block.begin() + raw_data.size() + 1, first_block.end());
    REQUIRE(std::ranges::all_of(
      rest_bytes_of_first_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    // length must be 56 in second block
    const auto &second_block = blocks.at(1);
    REQUIRE(hash_lab::ReadBE<uint64_t>(second_block.subspan(
              sha1::kBlockSize - sha1::kLengthSize)) == raw_data.size());
  }

  SECTION("57bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector<std::byte> raw_data(57, val);
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 2);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == sha1::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));
    REQUIRE(first_block[raw_data.size()] == sha1::kPaddingByte);
    // bytes 56 - 64 must be 0
    const auto rest_bytes_of_first_block =
      std::span(first_block.begin() + raw_data.size() + 1, first_block.end());
    REQUIRE(std::ranges::all_of(
      rest_bytes_of_first_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    // length must be 57 in second block
    const auto &second_block = blocks.at(1);
    REQUIRE(hash_lab::ReadBE<uint64_t>(second_block.subspan(
              sha1::kBlockSize - sha1::kLengthSize)) == raw_data.size());
  }

  SECTION("64bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector<std::byte> raw_data(64, val);
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 2);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == sha1::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));

    // length must be 57 in second block
    const auto &second_block = blocks.at(1);
    REQUIRE(second_block[0] == sha1::kPaddingByte);
    const auto null_bytes_of_second_block =
      std::span(second_block.begin() + 1,
                second_block.begin() + sha1::kBlockSize - sha1::kLengthSize);
    REQUIRE(std::ranges::all_of(
      null_bytes_of_second_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    REQUIRE(hash_lab::ReadBE<uint64_t>(second_block.subspan(
              sha1::kBlockSize - sha1::kLengthSize)) == raw_data.size());
  }

  SECTION("128bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector<std::byte> raw_data(128, val);
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 3);
    const auto &first_block = blocks.at(0);
    const auto &second_block = blocks.at(1);
    const auto &third_block = blocks.at(2);
    REQUIRE(first_block.size() == sha1::kBlockSize);
    REQUIRE(std::ranges::all_of(
      first_block, [](const std::byte elem) { return elem == val; }));
    REQUIRE(std::ranges::all_of(
      second_block, [](const std::byte elem) { return elem == val; }));

    REQUIRE(third_block[0] == sha1::kPaddingByte);
    const auto null_bytes_of_third_block =
      std::span(third_block.begin() + 1,
                third_block.begin() + sha1::kBlockSize - sha1::kLengthSize);
    REQUIRE(std::ranges::all_of(
      null_bytes_of_third_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    REQUIRE(hash_lab::ReadBE<uint64_t>(third_block.subspan(
              sha1::kBlockSize - sha1::kLengthSize)) == raw_data.size());
  }

  SECTION("200bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector<std::byte> raw_data(200, val);
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 4);
    const auto &block1 = blocks.at(0);
    const auto &block2 = blocks.at(1);
    const auto &block3 = blocks.at(2);
    const auto &block4 = blocks.at(3);

    REQUIRE(std::ranges::all_of(
      block1, [](const std::byte elem) { return elem == val; }));
    REQUIRE(std::ranges::all_of(
      block2, [](const std::byte elem) { return elem == val; }));
    REQUIRE(std::ranges::all_of(
      block3, [](const std::byte elem) { return elem == val; }));

    // 8 data bytes in block 4
    const auto data_bytes_block4 = block4.subspan(0, 8);
    REQUIRE(std::ranges::all_of(
      data_bytes_block4, [](const std::byte elem) { return elem == val; }));

    REQUIRE(block4[8] == sha1::kPaddingByte);

    // null bytes in block4
    const auto null_bytes_block4 =
      std::span(block4.begin() + 9,
                block4.begin() + sha1::kBlockSize - sha1::kLengthSize);
    REQUIRE(std::ranges::all_of(null_bytes_block4, [](const std::byte elem) {
      return elem == std::byte{0};
    }));
    // length
    REQUIRE(hash_lab::ReadBE<uint64_t>(block4.subspan(
              sha1::kBlockSize - sha1::kLengthSize)) == raw_data.size());
  }

  SECTION("248bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector<std::byte> raw_data(248, val);
    const sha1::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 5);
    const auto &block1 = blocks.at(0);
    const auto &block2 = blocks.at(1);
    const auto &block3 = blocks.at(2);
    const auto &block4 = blocks.at(3);
    const auto &block5 = blocks.at(4);

    // 192 bytes data
    REQUIRE(std::ranges::all_of(
      block1, [](const std::byte elem) { return elem == val; }));
    REQUIRE(std::ranges::all_of(
      block2, [](const std::byte elem) { return elem == val; }));
    REQUIRE(std::ranges::all_of(
      block3, [](const std::byte elem) { return elem == val; }));
    // 56 bytes data res
    const auto data_bytes_block4 = block4.subspan(0, 56);
    REQUIRE(std::ranges::all_of(
      data_bytes_block4, [](const std::byte elem) { return elem == val; }));
    // padding byte block4
    REQUIRE(block4[56] == sha1::kPaddingByte);
    // null bytes in block4
    const auto null_bytes_of_block4 =
      block4.subspan(57, sha1::kBlockSize - 57 - sha1::kLengthSize);
    REQUIRE(std::ranges::all_of(null_bytes_of_block4, [](const std::byte elem) {
      return elem == std::byte{0};
    }));

    // null bytes in block5
    const auto null_bytes_of_block5 =
      block5.subspan(0, sha1::kBlockSize - sha1::kLengthSize);
    REQUIRE(std::ranges::all_of(null_bytes_of_block5, [](const std::byte elem) {
      return elem == std::byte{0};
    }));

    // length
    REQUIRE(hash_lab::ReadBE<uint64_t>(block5.subspan(
              sha1::kBlockSize - sha1::kLengthSize)) == raw_data.size());
  }
}
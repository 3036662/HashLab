#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <iomanip>
#include <iostream>

#include "common/data_formatter.hpp"
#include "sha1/sha_1.hpp"

TEST_CASE("First test", "[smoke]") { REQUIRE(1 + 1 == 2); }

namespace sha1 = hash_lab::sha1;

TEST_CASE("DataFormatter") {
  SECTION("Empty") {
    const std::vector<std::byte> raw_data;
    const hash_lab::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 1);
    REQUIRE(blocks.at(0)[0] == hash_lab::kPaddingByte);
    REQUIRE(std::ranges::all_of(
      blocks.at(0).begin() + 1, blocks.at(0).end(),
      [](const std::byte val) { return val == std::byte{0}; }));
  }

  SECTION("55bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector raw_data(55, val);
    const hash_lab::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 1);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == hash_lab::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));
    REQUIRE(first_block[raw_data.size()] == hash_lab::kPaddingByte);

    REQUIRE(hash_lab::ReadBE<uint64_t>(first_block.subspan(
              hash_lab::kBlockSize - hash_lab::kLengthSize)) ==
            raw_data.size() * 8);
  }

  SECTION("56bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector raw_data(56, val);
    const hash_lab::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 2);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == hash_lab::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));
    REQUIRE(first_block[raw_data.size()] == hash_lab::kPaddingByte);
    // bytes 56 - 64 must be 0
    const auto rest_bytes_of_first_block =
      std::span(first_block.begin() + raw_data.size() + 1, first_block.end());
    REQUIRE(std::ranges::all_of(
      rest_bytes_of_first_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    // length must be 56 in second block
    const auto &second_block = blocks.at(1);
    REQUIRE(hash_lab::ReadBE<uint64_t>(second_block.subspan(
              hash_lab::kBlockSize - hash_lab::kLengthSize)) ==
            raw_data.size() * 8);
  }

  SECTION("57bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector raw_data(57, val);
    const hash_lab::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 2);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == hash_lab::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));
    REQUIRE(first_block[raw_data.size()] == hash_lab::kPaddingByte);
    // bytes 56 - 64 must be 0
    const auto rest_bytes_of_first_block =
      std::span(first_block.begin() + raw_data.size() + 1, first_block.end());
    REQUIRE(std::ranges::all_of(
      rest_bytes_of_first_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    // length must be 57 in second block
    const auto &second_block = blocks.at(1);
    REQUIRE(hash_lab::ReadBE<uint64_t>(second_block.subspan(
              hash_lab::kBlockSize - hash_lab::kLengthSize)) ==
            raw_data.size() * 8);
  }

  SECTION("64bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector raw_data(64, val);
    const hash_lab::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 2);
    const auto &first_block = blocks.at(0);
    REQUIRE(first_block.size() == hash_lab::kBlockSize);
    REQUIRE(std::all_of(first_block.begin(),
                        first_block.begin() + raw_data.size(),
                        [](const std::byte elem) { return elem == val; }));

    // length must be 57 in second block
    const auto &second_block = blocks.at(1);
    REQUIRE(second_block[0] == hash_lab::kPaddingByte);
    const auto null_bytes_of_second_block = std::span(
      second_block.begin() + 1,
      second_block.begin() + hash_lab::kBlockSize - hash_lab::kLengthSize);
    REQUIRE(std::ranges::all_of(
      null_bytes_of_second_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    REQUIRE(hash_lab::ReadBE<uint64_t>(second_block.subspan(
              hash_lab::kBlockSize - hash_lab::kLengthSize)) ==
            raw_data.size() * 8);
  }

  SECTION("128bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector raw_data(128, val);
    const hash_lab::DataFormatter fmt(raw_data);
    const auto &blocks = fmt.blocks();

    REQUIRE(blocks.size() == 3);
    const auto &first_block = blocks.at(0);
    const auto &second_block = blocks.at(1);
    const auto &third_block = blocks.at(2);
    REQUIRE(first_block.size() == hash_lab::kBlockSize);
    REQUIRE(std::ranges::all_of(
      first_block, [](const std::byte elem) { return elem == val; }));
    REQUIRE(std::ranges::all_of(
      second_block, [](const std::byte elem) { return elem == val; }));

    REQUIRE(third_block[0] == hash_lab::kPaddingByte);
    const auto null_bytes_of_third_block = std::span(
      third_block.begin() + 1,
      third_block.begin() + hash_lab::kBlockSize - hash_lab::kLengthSize);
    REQUIRE(std::ranges::all_of(
      null_bytes_of_third_block,
      [](const std::byte elem) { return elem == std::byte{0}; }));
    REQUIRE(hash_lab::ReadBE<uint64_t>(third_block.subspan(
              hash_lab::kBlockSize - hash_lab::kLengthSize)) ==
            raw_data.size() * 8);
  }

  SECTION("200bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector raw_data(200, val);
    const hash_lab::DataFormatter fmt(raw_data);
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

    REQUIRE(block4[8] == hash_lab::kPaddingByte);

    // null bytes in block4
    const auto null_bytes_block4 =
      std::span(block4.begin() + 9,
                block4.begin() + hash_lab::kBlockSize - hash_lab::kLengthSize);
    REQUIRE(std::ranges::all_of(null_bytes_block4, [](const std::byte elem) {
      return elem == std::byte{0};
    }));
    // length
    REQUIRE(hash_lab::ReadBE<uint64_t>(
              block4.subspan(hash_lab::kBlockSize - hash_lab::kLengthSize)) ==
            raw_data.size() * 8);
  }

  SECTION("248bytes") {
    constexpr auto val = std::byte{0xf0};
    const std::vector raw_data(248, val);
    const hash_lab::DataFormatter fmt(raw_data);
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
    REQUIRE(block4[56] == hash_lab::kPaddingByte);
    // null bytes in block4
    const auto null_bytes_of_block4 =
      block4.subspan(57, hash_lab::kBlockSize - 57 - hash_lab::kLengthSize);
    REQUIRE(std::ranges::all_of(null_bytes_of_block4, [](const std::byte elem) {
      return elem == std::byte{0};
    }));

    // null bytes in block5
    const auto null_bytes_of_block5 =
      block5.subspan(0, hash_lab::kBlockSize - hash_lab::kLengthSize);
    REQUIRE(std::ranges::all_of(null_bytes_of_block5, [](const std::byte elem) {
      return elem == std::byte{0};
    }));

    // length
    REQUIRE(hash_lab::ReadBE<uint64_t>(
              block5.subspan(hash_lab::kBlockSize - hash_lab::kLengthSize)) ==
            raw_data.size() * 8);
  }
}

TEST_CASE("ReadBE") {
  std::array<std::byte, 4> data{std::byte{0x01}, std::byte{0x23},
                                std::byte{0x45}, std::byte{0x67}};
  const auto value = hash_lab::ReadBE<uint32_t>(data);
  REQUIRE(value == 0x01234567);
}

TEST_CASE("ConvertDataBlock") {
  std::vector raw_data(hash_lab::kBlockSize, std::byte{0xfa});
  auto block_words = sha1::CreateBlock80Words(hash_lab::DataBlock(raw_data));
  auto first_16 = std::span(block_words.begin(), 16);
  REQUIRE(std::ranges::all_of(
    first_16, [](const uint32_t word) { return word == 0xfafafafa; }));
}

TEST_CASE("CreateBlock80Words for abc") {
  std::array<std::byte, 64> block{};
  block[0] = std::byte{'a'};   // 0x61
  block[1] = std::byte{'b'};   // 0x62
  block[2] = std::byte{'c'};   // 0x63
  block[3] = std::byte{0x80};  // padding

  block[60] = std::byte{0x00};
  block[61] = std::byte{0x00};
  block[62] = std::byte{0x00};
  block[63] = std::byte{0x18};  // 24 = 0x18

  auto W = sha1::CreateBlock80Words(block);

  REQUIRE(W[0] == 0x61626380);

  REQUIRE(W[15] == 0x00000018);
}

TEST_CASE("MagicFunc") {
  constexpr uint32_t b = 0xEFCDAB89;
  constexpr uint32_t c = 0x98BADCFE;
  constexpr uint32_t d = 0x10325476;

  const uint32_t f20 = sha1::MagicFunc(20, b, c, d);
  REQUIRE(f20 == (b ^ c ^ d));

  const uint32_t f40 = sha1::MagicFunc(40, b, c, d);
  REQUIRE(f40 == ((b & c) | (b & d) | (c & d)));
}

TEST_CASE("CircularLeftShift") {
  constexpr uint32_t word = 0x80000000;  // 1000...0000
  const uint32_t shifted = sha1::CircularLeftShift(word, 1);
  REQUIRE(shifted == 0x00000001);  // 0000...0001
}

TEST_CASE("GetK") {
  REQUIRE(sha1::GetK(0) == 0x5A827999UL);
  REQUIRE(sha1::GetK(19) == 0x5A827999UL);
  REQUIRE(sha1::GetK(20) == 0x6ED9EBA1UL);
  REQUIRE(sha1::GetK(39) == 0x6ED9EBA1UL);
  REQUIRE(sha1::GetK(40) == 0x8F1BBCDCUL);
  REQUIRE(sha1::GetK(59) == 0x8F1BBCDCUL);
  REQUIRE(sha1::GetK(60) == 0xCA62C1D6UL);
  REQUIRE(sha1::GetK(79) == 0xCA62C1D6UL);
}

TEST_CASE("SHA1") {
  SECTION("abc") {
    const sha1::Sha1 sha{"abc"};
    const auto result = sha.Calculate();
    hash_lab::PrintHex(result);
    constexpr std::array<uint8_t, 20> expected{
      0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
      0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D};

    REQUIRE(std::ranges::equal(std::as_bytes(std::span(expected)), result));
  }

  SECTION("empty") {
    const sha1::Sha1 sha{""};
    const auto result = sha.Calculate();
    hash_lab::PrintHex(result);
    constexpr std::array<uint8_t, 20> expected{
      0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
      0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09};

    REQUIRE(std::ranges::equal(std::as_bytes(std::span(expected)), result));
  }
}
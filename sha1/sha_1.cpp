
#include "sha_1.hpp"

namespace hash_lab::sha1 {

//  S^n(X)  =  (X << n) OR (X >> 32-n).
[[nodiscard]] uint32_t CircularLeftShift(const uint32_t word,
                                         const uint32_t shift) {
  if (shift >= 32) [[unlikely]] {
    throw std::runtime_error("[CircularLeftShift] shift too large");
  }
  return std::rotl(word, shift);
}

[[nodiscard]] uint32_t MagicFunc(const uint32_t step, const uint32_t b,
                                 const uint32_t c, const uint32_t d) {
  if (step <= 19) {
    return (b & c) | (~b & d);
  }
  if (step <= 39) {
    return b ^ c ^ d;
  }
  if (step <= 59) {
    return (b & c) | (b & d) | (c & d);
  }
  if (step <= 79) {
    return b ^ c ^ d;
  }
  throw std::runtime_error("[MagicFunc] step too large");
}

[[nodiscard]] uint32_t GetK(const uint32_t step) {
  if (step <= 19) return k0_19;
  if (step <= 39) return k20_39;
  if (step <= 59) return k40_59;
  if (step <= 79) {
    return k60_79;
  }
  throw std::runtime_error("[GetK] step too large");
}

std::array<std::byte, 20> Sha1::Calculate() const {
  // The words of the second 5-word buffer are labeled `H0, H1, H2, H3, H4`.
  Buffer5Words buff_h{kH0, kH1, kH2, kH3, kH4};
  uint32_t& H0 = buff_h[0];
  uint32_t& H1 = buff_h[1];
  uint32_t& H2 = buff_h[2];
  uint32_t& H3 = buff_h[3];
  uint32_t& H4 = buff_h[4];

  // for each data block
  for (const auto& data_block : fmt_.blocks()) {
    // The words of the first 5-word buffer are labeled `A,B,C,D,E`
    Buffer5Words buff1{H0, H1, H2, H3, H4};
    uint32_t& A = buff1[0];
    uint32_t& B = buff1[1];
    uint32_t& C = buff1[2];
    uint32_t& D = buff1[3];
    uint32_t& E = buff1[4];
    Block80Words block80_w_words = CreateBlock80WordsEx(data_block);
    // perform 80 steps
    for (size_t step = 0; step < kTotalSteps; ++step) {
      // TEMP = S^5(A) + f(t;B,C,D) + E + W(t) + K(t);
      // E = D;  D = C;  C = S^30(B);  B = A; A = TEMP;
      const uint32_t temp = CircularLeftShift(A, 5) + MagicFunc(step, B, C, D) +
                            E + block80_w_words[step] + GetK(step);
      E = D;
      D = C;
      C = CircularLeftShift(B, 30);
      B = A;
      A = temp;
    }
    // update H
    H0 += A;
    H1 += B;
    H2 += C;
    H3 += D;
    H4 += E;
  }

  std::array<std::byte, 20> res{};
  for (size_t word_index = 0; word_index < 5; ++word_index) {
    WriteBE(std::span(res.begin() + word_index * sizeof(uint32_t), 4),
            buff_h[word_index]);
  }
  return res;
}

Block80Words CreateBlock80Words(const DataBlock data_block) {
  Block80Words res{};
  // 16 words
  for (size_t word_index = 0; word_index < 16; ++word_index) {
    constexpr size_t word_size = sizeof(uint32_t);
    res[word_index] =
      ReadBE<uint32_t>(data_block.subspan(word_index * word_size, word_size));
  }
  // the rest 64 words
  for (size_t word_index = 16; word_index < 80; ++word_index) {
    res[word_index] =
      CircularLeftShift(res[word_index - 3] ^ res[word_index - 8] ^
                          res[word_index - 14] ^ res[word_index - 16],
                        1);
  }
  return res;
}

// optimized version
Block80Words CreateBlock80WordsEx(const DataBlock data_block) {
  Block80Words res;
  // copy 16 words
  std::memcpy(res.data(), data_block.data(), 64);
  // 16 words reverse
  if constexpr (std::endian::native == std::endian::little) {
    auto* bytes = reinterpret_cast<uint8_t*>(res.data());
    for (size_t i = 0; i < 16; ++i) {
      std::reverse(bytes + i * 4, bytes + i * 4 + 4);
    }
  }
  // the rest 64 words
  for (size_t word_index = 16; word_index < 80; ++word_index) {
    res[word_index] = std::rotl(res[word_index - 3] ^ res[word_index - 8] ^
                                  res[word_index - 14] ^ res[word_index - 16],
                                1);
  }
  return res;
}

}  //  namespace hash_lab::sha1
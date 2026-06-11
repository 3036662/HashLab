
#include "sha_256.hpp"

#include "common/common_utils.hpp"

namespace hash_lab::sha256 {

using Block64Words = std::array<std::uint32_t, 64>;
using Buffer8Words = std::array<std::uint32_t, 8>;

constexpr size_t kTotalSteps = 64;

constexpr std::array<uint32_t, 64> kKCube{
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
  0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
  0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
  0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
  0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
  0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

constexpr std::array<uint32_t, 8> kSha256InitialH{
  0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
  0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

// SSIG0(x) = ROTR^7(x) XOR ROTR^18(x) XOR SHR^3(x)
[[nodiscard]] constexpr uint32_t SSig0(uint32_t x) {
  return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3);
}

// SSIG1(x) = ROTR^17(x) XOR ROTR^19(x) XOR SHR^10(x)
[[nodiscard]] constexpr uint32_t SSig1(uint32_t x) {
  return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10);
}

// BSIG0(x) = ROTR^2(x) XOR ROTR^13(x) XOR ROTR^22(x)
[[nodiscard]] constexpr uint32_t BSig0(uint32_t x) {
  return std::rotr(x, 2) ^ std::rotr(x, 13) ^ std::rotr(x, 22);
}

// BSIG1(x) = ROTR^6(x) XOR ROTR^11(x) XOR ROTR^25(x)
[[nodiscard]] constexpr uint32_t BSig1(uint32_t x) {
  return std::rotr(x, 6) ^ std::rotr(x, 11) ^ std::rotr(x, 25);
}

// MAJ( x, y, z) = (x AND y) XOR (x AND z) XOR (y AND z)
[[nodiscard]] constexpr uint32_t Maj(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

// CH( x, y, z) = (x AND y) XOR ( (NOT x) AND z)
[[nodiscard]] constexpr uint32_t Ch(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (~x & z);
}

Block64Words CreateBlock64Words(const DataBlock data_block) {
  Block64Words res;
  // copy 16 words
  std::memcpy(res.data(), data_block.data(), 64);
  // 16 words reverse to BE
  if constexpr (std::endian::native == std::endian::little) {
    auto* bytes = reinterpret_cast<uint8_t*>(res.data());
    for (size_t i = 0; i < 16; ++i) {
      std::reverse(bytes + i * 4, bytes + i * 4 + 4);
    }
  }
  // the rest 64 words
  // Wt = SSIG1(W(t-2)) + W(t-7) + SSIG0(w(t-15)) + W(t-16)
  for (size_t word_index = 16; word_index < 64; ++word_index) {
    res[word_index] = SSig1(res[word_index - 2]) + res[word_index - 7] +
                      SSig0(res[word_index - 15]) + res[word_index - 16];
  }
  return res;
}

std::array<std::byte, 32> Sha256::Calculate() const {
  // The words of the message schedule are labeled W0, W1, ..., W63

  // The eight working variables are labeled a, b, c, d, e, f, g, and h.

  // The words of the hash value are labeled H(i) 0, H(i) 1, ..., H(i) 7}
  // which will hold the initial hash value, H(0), replaced by each successive
  // intermediate hash value (after each message block is processed), H(i)

  // 512-bit blocks that are considered to be composed of sixteen 32-bit words
  // M(i)0, M(i)1, ..., M(i)15

  Buffer8Words buff_h{kSha256InitialH};
  uint32_t& H0 = buff_h[0];
  uint32_t& H1 = buff_h[1];
  uint32_t& H2 = buff_h[2];
  uint32_t& H3 = buff_h[3];
  uint32_t& H4 = buff_h[4];
  uint32_t& H5 = buff_h[5];
  uint32_t& H6 = buff_h[6];
  uint32_t& H7 = buff_h[7];
  for (const auto& data_block : fmt_.blocks()) {
    Buffer8Words buff1{H0, H1, H2, H3, H4, H5, H6, H7};
    uint32_t& A = buff1[0];
    uint32_t& B = buff1[1];
    uint32_t& C = buff1[2];
    uint32_t& D = buff1[3];
    uint32_t& E = buff1[4];
    uint32_t& F = buff1[5];
    uint32_t& G = buff1[6];
    uint32_t& H = buff1[7];
    const Block64Words block80_w_words = CreateBlock64Words(data_block);
    // 64 steps
    for (size_t step = 0; step < kTotalSteps; ++step) {
      // T1 = h + BSIG1(e) + CH(e,f,g) + Kt + Wt
      const uint32_t temp1 =
        H + BSig1(E) + Ch(E, F, G) + kKCube[step] + block80_w_words[step];
      // T2 = BSIG0(a) + MAJ(a,b,c)
      const uint32_t temp2 = BSig0(A) + Maj(A, B, C);
      H = G;
      G = F;
      F = E;
      E = D + temp1;
      D = C;
      C = B;
      B = A;
      A = temp1 + temp2;
    }
    H0 += A;
    H1 += B;
    H2 += C;
    H3 += D;
    H4 += E;
    H5 += F;
    H6 += G;
    H7 += H;
  }
  std::array<std::byte, 32> res;
  for (size_t word_index = 0; word_index < 8; ++word_index) {
    WriteBE(std::span(res.begin() + word_index * sizeof(uint32_t), 4),
            buff_h[word_index]);
  }
  return res;
}

}  // namespace hash_lab::sha256
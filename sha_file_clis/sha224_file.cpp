#include <iostream>
#include <mio/mmap.hpp>
#include <vector>

#include "common/common_utils.hpp"
#include "sha224_256/sha_224_256.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::error_code error;
  const mio::mmap_source mmap = mio::make_mmap_source(argv[1], error);

  if (error) {
    std::cerr << "Error mapping file: " << error.message() << std::endl;
    return 1;
  }

  auto file_data = std::span<const std::byte>(
    reinterpret_cast<const std::byte*>(mmap.data()), mmap.size());

  // Calculate SHA-1
  const hash_lab::sha224_256::Sha224 sha224{std::span(file_data)};
  const auto hash = sha224.Calculate();
  hash_lab::PrintHex(hash);

  return 0;
}

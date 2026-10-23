#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
using DEVICE = std::uint32_t;
using BLOCK = std::uint32_t;


using UID=std::uint32_t;
using GID=std::uint32_t;
using blockNumber = std::uint32_t;
constexpr std::size_t BLOCK_SIZE = 4096;


constexpr uint8_t NDIRECT=10;
constexpr uint8_t NBLOCKADDR= 13; //13 member array
constexpr std::size_t NINDIRECT=BLOCK_SIZE/sizeof(blockNumber);
constexpr uint32_t ROOTINO=1;
#endif
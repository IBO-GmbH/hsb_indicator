#include "wolf/esp3_parser.hpp"
#include "gtest/gtest.h"

using read_buffer = std::array<std::uint8_t, 128>;

static int callback_counter = 0;

void connect_signals(wolf::esp3_parser& parser) {
  callback_counter = 0;
  parser.signal_learned.connect(
      [](const wolf::types::id_esp3& id, const std::uint8_t& r_org,
         const std::uint8_t& func, const std::uint8_t& type) {
        EXPECT_EQ(id, 0x508d7b3);
        EXPECT_EQ(r_org, 0xa5);
        EXPECT_EQ(func, 0x4);
        EXPECT_EQ(type, 0x1);
        ++callback_counter;
      });
}

std::size_t make_data_valid(read_buffer& data) {
  // enocean sensor a5-4-1 paket with id 0x508d7b3
  data = {0x55, 0x0,  0xa,  0x7, 0x1, 0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,
          0x8,  0xd7, 0xb3, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee};
  return 24;
}

std::size_t make_data_invalid(read_buffer& data) {
  // part of valid paket but without beginning
  data = {0x8, 0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,
          0x0, 0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee};
  return 16;
}

std::size_t make_data_valid_long(read_buffer& data) {
  // enocean sensor a5-4-1 paket with id 0x508d7b3 with part of same paket at
  // the beginning
  data = {0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0, 0xff,
          0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa, 0x7,
          0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8, 0xd7,
          0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0, 0xee};
  return 40;
}

std::size_t make_data_valid_half1(read_buffer& data) {
  // enocean sensor a5-4-1 paket with id 0x508d7b3
  data = {0x55, 0x0, 0xa, 0x7, 0x1, 0xeb, 0xa5, 0x10, 0x8, 0xb, 0xd5, 0x5};
  return 12;
}

std::size_t make_data_valid_half2(read_buffer& data) {
  // enocean sensor a5-4-1 paket with id 0x508d7b3
  data = {0x8, 0xd7, 0xb3, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x30, 0x0, 0xee};
  return 12;
}

std::size_t make_data_valid_triple(read_buffer& data) {
  // 3 enocean sensor a5-4-1 pakets with id 0x508d7b3
  data = {0x55, 0x0,  0xa,  0x7, 0x1, 0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,
          0x8,  0xd7, 0xb3, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee,
          0x55, 0x0,  0xa,  0x7, 0x1, 0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,
          0x8,  0xd7, 0xb3, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee,
          0x55, 0x0,  0xa,  0x7, 0x1, 0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,
          0x8,  0xd7, 0xb3, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee};
  return 72;
}

std::size_t make_data_valid_long_triple(read_buffer& data) {
  // 3 enocean sensor a5-4-1 pakets with id 0x508d7b3 with part of same paket at
  // the beginning
  data = {0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff,
          0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,  0x7,  0x1,  0xeb,
          0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,
          0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,  0x7,
          0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3,
          0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,
          0xa,  0x7,  0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,
          0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee};
  return 88;
}

std::size_t make_data_valid_invalid_long(read_buffer& data) {
  // 3 enocean sensor a5-4-1 pakets with id 0x508d7b3 with part of same paket at
  // the beginning and part inbetween
  data = {0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff,
          0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,  0x7,  0x1,  0xeb,
          0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,
          0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x8,  0xb,  0xd5, 0x5,
          0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,
          0xee, 0x55, 0x0,  0xa,  0x7,  0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,
          0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff,
          0x30, 0x0,  0xee, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,
          0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,
          0x7,  0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7,
          0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee};
  return 120;
}

std::size_t make_data_valid_invalid_long_wrong_crc_header(read_buffer& data) {
  // 3 enocean sensor a5-4-1 pakets with id 0x508d7b3 with part of same paket at
  // the beginning and part inbetween and 2nd package header crc is wrong
  data = {0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff,
          0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,  0x7,  0x1,  0xeb,
          0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,
          0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x8,  0xb,  0xd5, 0x5,
          0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,
          0xee, 0x55, 0x0,  0xa,  0x7,  0x1,  0xef, 0xa5, 0x10, 0x8,  0xb,
          0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff,
          0x30, 0x0,  0xee, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,
          0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,
          0x7,  0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7,
          0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee};
  return 120;
}

std::size_t make_data_valid_invalid_long_wrong_crc_data(read_buffer& data) {
  // 3 enocean sensor a5-4-1 pakets with id 0x508d7b3 with part of same paket at
  // the beginning and part inbetween and 2nd package data crc is wrong
  data = {0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff,
          0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,  0x7,  0x1,  0xeb,
          0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,
          0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x8,  0xb,  0xd5, 0x5,
          0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,
          0xee, 0x55, 0x0,  0xa,  0x7,  0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,
          0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff,
          0x30, 0x0,  0xea, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7, 0xb3, 0x0,
          0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee, 0x55, 0x0,  0xa,
          0x7,  0x1,  0xeb, 0xa5, 0x10, 0x8,  0xb,  0xd5, 0x5,  0x8,  0xd7,
          0xb3, 0x0,  0x0,  0xff, 0xff, 0xff, 0xff, 0x30, 0x0,  0xee};
  return 120;
}

TEST(ESP3Parser, OnlyValidPackets) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid(data);
  parser.handle_data(data, size);
  parser.handle_data(data, size);
  size = make_data_valid_half1(data);
  parser.handle_data(data, size);
  size = make_data_valid_half2(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 3);
}

TEST(ESP3Parser, InitialSync) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid_long(data);
  parser.handle_data(data, size);
  size = make_data_valid(data);
  parser.handle_data(data, size);
  size = make_data_valid_half1(data);
  parser.handle_data(data, size);
  size = make_data_valid_half2(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 3);
}

TEST(ESP3Parser, Resync) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid(data);
  parser.handle_data(data, size);
  size = make_data_invalid(data);
  parser.handle_data(data, size);
  size = make_data_valid(data);
  parser.handle_data(data, size);
  size = make_data_valid_long(data);
  parser.handle_data(data, size);
  size = make_data_invalid(data);
  parser.handle_data(data, size);
  size = make_data_valid_half1(data);
  parser.handle_data(data, size);
  size = make_data_valid_half2(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 4);
}

TEST(ESP3Parser, OnlyValidPacketTriple) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid_triple(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 3);
}

TEST(ESP3Parser, InitialSyncTriple) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid_long_triple(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 3);
}

TEST(ESP3Parser, InitialSyncValidInvalidPackets) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid_invalid_long(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 3);
}

TEST(ESP3Parser, ValidPacketsAndInvalidCRCHeader) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid_invalid_long_wrong_crc_header(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 2);
}

TEST(ESP3Parser, ValidPacketsAndInvalidCRCData) {
  wolf::esp3_parser parser;
  connect_signals(parser);
  read_buffer data;
  auto size = make_data_valid_invalid_long_wrong_crc_data(data);
  parser.handle_data(data, size);
  EXPECT_EQ(callback_counter, 2);
}
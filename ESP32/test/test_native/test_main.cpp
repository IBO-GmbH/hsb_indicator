#include <gtest/gtest.h>

// fakes
#include "esp_log.h"

// libaries
#include "uart_handler.h"

// TEST(...)
// TEST_F(...)

TEST(DummyTest, ShouldPass) { EXPECT_EQ(1, 1); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // if you plan to use GMock, replace the line above with
  // ::testing::InitGoogleMock(&argc, argv);

  if (RUN_ALL_TESTS()) {
  }

  // Always return zero-code and allow PlatformIO to parse results
  return 0;
}

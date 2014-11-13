#include <limits.h>
#include "File.h"
#include "gtest/gtest.h"

//Parsing Test

TEST(ParsingTest, OriginalFile) {
  
  File of("../File/star.txt");
  of.parseText();
  EXPECT_EQ(1549, of.getTotalLines());

}

TEST(ParsingTest, RandomFiles) {
  
  File f1("../File/f1.txt");
  File f2("../File/f2.txt");
  File f3("../File/f3.txt");
  f1.parseText();
  f2.parseText();
  f3.parseText();
  EXPECT_EQ(1525, f1.getTotalLines());
  EXPECT_EQ(10, f2.getTotalLines());
  EXPECT_EQ(2, f3.getTotalLines());

}

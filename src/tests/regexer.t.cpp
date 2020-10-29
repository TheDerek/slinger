#include <gtest/gtest.h>

#include "map_maker.h"
#include "regexer.h"

TEST(Regexer, CanGetMatches) {
    Regexer regexer(R"(\s?([a-zA-Z])\s([^a-zA-Z]+\s))");

    auto matches = regexer.search("M 72.732231,91.993675 H 85.979527 L 72.732231,118.39366 Z");
    ASSERT_EQ(matches.size(), 3);
    EXPECT_EQ("M 72.732231,91.993675 ", matches.at(0).matchedString());

    ASSERT_EQ(matches.at(0).groups().size(), 2);
    EXPECT_EQ("M", matches.at(0).groups().at(0));
    EXPECT_EQ("72.732231,91.993675 ", matches.at(0).groups().at(1));

    EXPECT_TRUE(true);
}
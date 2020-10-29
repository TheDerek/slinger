#include <gtest/gtest.h>

#include "path_builder.h"
#include "regexer.h"

TEST(PathBuilder, CanBuildPath) {
    Regexer regexer(PathBuilder::COMMAND_REGEX);

    auto matches = regexer.search("M 72.732231,91.993675 H 85.979527 L 72.732231,118.39366 Z");
    ASSERT_EQ(matches.size(), 4);
    EXPECT_EQ("M 72.732231,91.993675 ", matches.at(0).matchedString());

    ASSERT_EQ(matches.at(0).groups().size(), 2);
    EXPECT_EQ("M", matches.at(0).groups().at(0));
    EXPECT_EQ("72.732231,91.993675 ", matches.at(0).groups().at(1));

    EXPECT_TRUE(true);
}

TEST(PathBuilder, CanParseExponentInCommand) {
    Regexer regexer(PathBuilder::COMMAND_REGEX);

    auto matches = regexer.search("M 72.732231e10,91.993675 H 85.979527 L 72.732231,118.39366 Z");
    ASSERT_EQ(matches.size(), 4);
    EXPECT_EQ("M 72.732231e10,91.993675 ", matches.at(0).matchedString());

    ASSERT_EQ(matches.at(0).groups().size(), 2);
    EXPECT_EQ("M", matches.at(0).groups().at(0));
    EXPECT_EQ("72.732231e10,91.993675 ", matches.at(0).groups().at(1));
}

TEST(PathBuilder, CanParseNumber) {
    Regexer regexer(PathBuilder::COORDINATE_REGEX);

    auto matches = regexer.search("72.732231,91.993675 85.979527,118.39366");
    ASSERT_EQ(matches.size(), 4);
    EXPECT_EQ("72.732231", matches.at(0).matchedString());
    EXPECT_EQ("91.993675", matches.at(1).matchedString());
    EXPECT_EQ("85.979527", matches.at(2).matchedString());
    EXPECT_EQ("118.39366", matches.at(3).matchedString());
}

TEST(PathBuilder, CanParseNumberWithExponent) {
    Regexer regexer(PathBuilder::COORDINATE_REGEX);

    auto matches = regexer.search("72.732231e10,91.993675 85.979527e-8,118.39366");
    ASSERT_EQ(matches.size(), 4);
    EXPECT_EQ("72.732231e10", matches.at(0).matchedString());
    EXPECT_EQ("91.993675", matches.at(1).matchedString());
    EXPECT_EQ("85.979527e-8", matches.at(2).matchedString());
    EXPECT_EQ("118.39366", matches.at(3).matchedString());
}
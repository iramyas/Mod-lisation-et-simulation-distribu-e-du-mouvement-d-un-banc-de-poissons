#include "gtest/gtest.h"
#include "vector2D.h"

TEST(Vector2DTest, AdditionOperator) {
    simulation::Vector2D vec1 { 1, 1 };
    simulation::Vector2D vec2 { 2, 5 };
    simulation::Vector2D vec_expected = { 3, 6 };
    EXPECT_EQ(vec1 + vec2, vec_expected);
}

TEST(Vector2DTest, AdditionCommutative) {
    simulation::Vector2D vec1 { 1, 1 };
    simulation::Vector2D vec2 { 3, 1 };
    EXPECT_EQ(vec1 + vec2, vec2 + vec1);
}

TEST(Vector2DTest, AdditionNegative) {
    simulation::Vector2D vec1 { 5, 3 };
    simulation::Vector2D vec2 { -2, -1};
    simulation::Vector2D vec_expected = { 3, 2 };
    EXPECT_EQ(vec1 + vec2, vec_expected);
}
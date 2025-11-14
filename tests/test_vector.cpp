#include "gtest/gtest.h"
#include "vector2D.h"

// ################## Tests Addition ################## //

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

// ################## Tests Soustraction ################## //

TEST(Vector2DTest, SubstractOperator) {
    simulation::Vector2D vec1 { 2, 5 };
    simulation::Vector2D vec2 { 1, 1 };
    simulation::Vector2D vec_expected = { 1, 4 };
    EXPECT_EQ(vec1 - vec2, vec_expected);
}

TEST(Vector2DTest, SubtractOneNegative) {
    simulation::Vector2D vec1 { 3, 3 };
    simulation::Vector2D vec2 { -3, -1 };
    simulation::Vector2D vec_expected = { 6, 4 };
    EXPECT_EQ(vec1 - vec2, vec_expected);
}

TEST(Vector2DTest, SubstractTwoNegative) {
    simulation::Vector2D vec1 { -3, -3 };
    simulation::Vector2D vec2 { -3, -1 };
    simulation::Vector2D vec_expected = { 0, -2 };
    EXPECT_EQ(vec1 - vec2, vec_expected);
}

// ################## Tests Multiplication Scalaire ################## //

TEST(Vector2DTest, MultiplyScalarPositive) {
    simulation::Vector2D vec1 { 2, 5 };
    simulation::Vector2D vec_expected = { 4, 10 };
    EXPECT_EQ(vec1 * 2, vec_expected);
}

TEST(Vector2DTest, MultiplyScalarNegative) {
    simulation::Vector2D vec1 { 2, 5 };
    simulation::Vector2D vec_expected = { -4, -10 };
    EXPECT_EQ(vec1 * -2, vec_expected);
}

TEST(Vector2DTest, MultiplyScalarZero) {
    simulation::Vector2D vec1 { 2, 5 };
    simulation::Vector2D vec_expected = { 0, 0 };
    EXPECT_EQ(vec1 * 0, vec_expected);
}
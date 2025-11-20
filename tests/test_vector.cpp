#include "gtest/gtest.h"
#include <cmath>
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

// ################## Tests Magnitude ################## //

TEST(Vector2DTest, MagnitudeBase) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    EXPECT_FLOAT_EQ(vec.magnitude(), 5.0f);
}

TEST(Vector2DTest, MagnitudeZeroVector) {
    simulation::Vector2D vec { 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(vec.magnitude(), 0.0f);
}

TEST(Vector2DTest, MagnitudeNegattiveValues) {
    simulation::Vector2D vec { -3.0f, -4.0f };
    EXPECT_FLOAT_EQ(vec.magnitude(), 5.0f);
}

TEST(Vector2DTest, MagnitudeUnitVector) {
    simulation::Vector2D vec { 1.0f, 0.0f };
    EXPECT_FLOAT_EQ(vec.magnitude(), 1.0f);
}

// ################## Tests Magnitude Squared ################## //

TEST(Vector2DTest, MagnitudeSquaredBase) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    EXPECT_FLOAT_EQ(vec.magnitudeSquared(), 25.0f);
}

TEST(Vector2DTest, MagnitudeSquaredZeroVector) {
    simulation::Vector2D vec { 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(vec.magnitudeSquared(), 0.0f);
}

TEST(Vector2DTest, MagnitudeSquaredConsistency) {
    simulation::Vector2D vec { 5.0f, 12.0f };
    float mag = vec.magnitude();
    EXPECT_FLOAT_EQ(vec.magnitudeSquared(), mag * mag);
}

// ################## Tests Angle ################## //

TEST(Vector2DTest, AngleAxisX) {
    simulation::Vector2D vec { 1.0f, 0.0f };
    EXPECT_NEAR(vec.angle(), 0.0, 1e-9);
}

TEST(Vector2DTest, AngleAxisY) {
    simulation::Vector2D vec { 0.0f, 1.0f};
    EXPECT_NEAR(vec.angle(), M_PI / 2.0, 1e-6);
}

TEST(Vector2DTest, AngleAxisNegativeX) {
    simulation::Vector2D vec { -1.0f, 0.0f };
    EXPECT_NEAR(vec.angle(), M_PI, 1e-6);
}

TEST(Vector2DTest, AngleAxisNegativeY) {
    simulation::Vector2D vec { 0.0f, -1.0f };
    EXPECT_NEAR(vec.angle(), -M_PI / 2.0, 1e-6);
}

TEST(Vector2DTest, Angle45Degrees) {
    simulation::Vector2D vec { 1.0f, 1.0f };
    EXPECT_NEAR(vec.angle(), M_PI / 4.0, 1e-6);
}

TEST(Vector2DTest, AngleRange) {
    simulation::Vector2D vec { -1.0f, -1.0f };
    double angle = vec.angle();
    EXPECT_GE(angle, -M_PI);
    EXPECT_LE(angle, M_PI);
}

// ################## Tests Normalized ################## //

TEST(Vector2DTest, NormalizedUnitVector) {
    simulation::Vector2D vec { 1.0f, 0.0f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_FLOAT_EQ(norm.x, 1.0f);
    EXPECT_FLOAT_EQ(norm.y, 0.0f);
    EXPECT_FLOAT_EQ(norm.magnitude(), 1.0f);
}

TEST(Vector2DTest, NormalizedMagnitudeIsOne) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_NEAR(norm.magnitude(), 1.0f, 1e-6f);
}

TEST(Vector2DTest, NormalizedDirection) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_FLOAT_EQ(norm.x, 0.6f);
    EXPECT_FLOAT_EQ(norm.y, 0.8f);
}

TEST(Vector2DTest, NormalizedZeroVector) {
    simulation::Vector2D vec { 0.0f, 0.0f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_FLOAT_EQ(norm.x, 0.0f);
    EXPECT_FLOAT_EQ(norm.y, 0.0f);
}

TEST(Vector2DTest, NormalizedVerySmallVector) {
    simulation::Vector2D vec { 1e-8f, 1e-8f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_FLOAT_EQ(norm.x, 0.0f);
    EXPECT_FLOAT_EQ(norm.y, 0.0f);
}

TEST(Vector2DTest, NormalizedNegativeVector) {
    simulation::Vector2D vec { -3.0f, -4.0f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_FLOAT_EQ(norm.x, -0.6f);
    EXPECT_FLOAT_EQ(norm.y, -0.8f);
    EXPECT_NEAR(norm.magnitude(), 1.0f, 1e-6f);
}

TEST(Vector2DTest, NormalizedDoesNotModifyOriginal) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_FLOAT_EQ(vec.x, 3.0f);
    EXPECT_FLOAT_EQ(vec.y, 4.0f);
    EXPECT_FLOAT_EQ(vec.magnitude(), 5.0f);
}

TEST(Vector2DTest, NormalizedAnglePreserved) {
    simulation::Vector2D vec { 5.0f, 5.0f };
    simulation::Vector2D norm = vec.normalized();
    EXPECT_NEAR(vec.angle(), norm.angle(), 1e-6);
}

TEST(Vector2DTest, NormalizedIdempotent) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D norm1 = vec.normalized();
    simulation::Vector2D norm2 = norm1.normalized();
    EXPECT_FLOAT_EQ(norm1.x, norm2.x);
    EXPECT_FLOAT_EQ(norm1.y, norm2.y);
}

// ################## Tests Dot Product ################## //

TEST(Vector2DTest, DotProductPerpendicular) {
    simulation::Vector2D vec1 { 1.0f, 0.0f };
    simulation::Vector2D vec2 { 0.0f, 1.0f };
    EXPECT_FLOAT_EQ(vec1.dot(vec2), 0.0f);
}

TEST(Vector2DTest, DotProductParallel) {
    simulation::Vector2D vec1 { 2.0f, 3.0f };
    simulation::Vector2D vec2 { 2.0f, 3.0f };
    EXPECT_FLOAT_EQ(vec1.dot(vec2), 13.0f);
}

TEST(Vector2DTest, DotProductOpposite) {
    simulation::Vector2D vec1 { 1.0f, 0.0f };
    simulation::Vector2D vec2 { -1.0f, 0.0f };
    EXPECT_FLOAT_EQ(vec1.dot(vec2), -1.0f);
}

TEST(Vector2DTest, DotProductCommutative) {
    simulation::Vector2D vec1 { 3.0f, 4.0f };
    simulation::Vector2D vec2 { 5.0f, 2.0f };
    EXPECT_FLOAT_EQ(vec1.dot(vec2), vec2.dot(vec1));
}

TEST(Vector2DTest, DotProductZeroVector) {
    simulation::Vector2D vec1 { 3.0f, 4.0f };
    simulation::Vector2D vec2 { 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(vec1.dot(vec2), 0.0f);
}

// ################## Tests Distance ################## //

TEST(Vector2DTest, DistanceSamePoint) {
    simulation::Vector2D vec1 { 3.0f, 4.0f };
    simulation::Vector2D vec2 { 3.0f, 4.0f };
    EXPECT_FLOAT_EQ(vec1.distance(vec2), 0.0f);
}

TEST(Vector2DTest, DistanceHorizontal) {
    simulation::Vector2D vec1 { 0.0f, 0.0f };
    simulation::Vector2D vec2 { 5.0f, 0.0f };
    EXPECT_FLOAT_EQ(vec1.distance(vec2), 5.0f);
}

TEST(Vector2DTest, DistanceVertical) {
    simulation::Vector2D vec1 { 0.0f, 0.0f };
    simulation::Vector2D vec2 { 0.0f, 12.0f };
    EXPECT_FLOAT_EQ(vec1.distance(vec2), 12.0f);
}

TEST(Vector2DTest, DistancePythagorean) {
    simulation::Vector2D vec1 { 3.0f, 0.0f };
    simulation::Vector2D vec2 { 0.0f, 4.0f };
    EXPECT_FLOAT_EQ(vec1.distance(vec2), 5.0f);
}

TEST(Vector2DTest, DistanceSymmetric) {
    simulation::Vector2D vec1 { 1.0f, 2.0f };
    simulation::Vector2D vec2 { 4.0f, 6.0f };
    EXPECT_FLOAT_EQ(vec1.distance(vec2), vec2.distance(vec1));
}

TEST(Vector2DTest, DistanceNegativeCoordinates) {
    simulation::Vector2D vec1 { -2.0f, -3.0f };
    simulation::Vector2D vec2 { 1.0f, 1.0f };
    EXPECT_FLOAT_EQ(vec1.distance(vec2), 5.0f);
}

// ################## Tests Distance Squared ################## //

TEST(Vector2DTest, DistanceSquaredSamePoint) {
    simulation::Vector2D vec1 { 3.0f, 4.0f };
    simulation::Vector2D vec2 { 3.0f, 4.0f };
    EXPECT_FLOAT_EQ(vec1.distanceSquared(vec2), 0.0f);
}

TEST(Vector2DTest, DistanceSquaredPythagorean) {
    simulation::Vector2D vec1 { 3.0f, 0.0f };
    simulation::Vector2D vec2 { 0.0f, 4.0f };
    EXPECT_FLOAT_EQ(vec1.distanceSquared(vec2), 25.0f);
}

TEST(Vector2DTest, DistanceSquaredConsistency) {
    simulation::Vector2D vec1 { 1.0f, 2.0f };
    simulation::Vector2D vec2 { 4.0f, 6.0f };
    float dist = vec1.distance(vec2);
    EXPECT_FLOAT_EQ(vec1.distanceSquared(vec2), dist * dist);
}

TEST(Vector2DTest, DistanceSquaredSymmetric) {
    simulation::Vector2D vec1 { 2.0f, 3.0f };
    simulation::Vector2D vec2 { 5.0f, 7.0f };
    EXPECT_FLOAT_EQ(vec1.distanceSquared(vec2), vec2.distanceSquared(vec1));
}

TEST(Vector2DTest, DistanceSquaredNegativeCoordinates) {
    simulation::Vector2D vec1 { 2.0f, -1.0f };
    simulation::Vector2D vec2 { -1.0f, 3.0f };
    EXPECT_FLOAT_EQ(vec1.distanceSquared(vec2), 25.0f);
}

// ################## Tests Négation ################## //

TEST(Vector2DTest, NegationBase) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D neg = -vec;
    EXPECT_FLOAT_EQ(neg.x, -3.0f);
    EXPECT_FLOAT_EQ(neg.y, -4.0f);
}

TEST(Vector2DTest, NegationZeroVector) {
    simulation::Vector2D vec { 0.0f, 0.0f };
    simulation::Vector2D neg = -vec;
    EXPECT_FLOAT_EQ(neg.x, 0.0f);
    EXPECT_FLOAT_EQ(neg.y, 0.0f);
}

TEST(Vector2DTest, NegationNegativeValues) {
    simulation::Vector2D vec { -5.0f, -2.0f };
    simulation::Vector2D neg = -vec;
    EXPECT_FLOAT_EQ(neg.x, 5.0f);
    EXPECT_FLOAT_EQ(neg.y, 2.0f);
}

TEST(Vector2DTest, NegationMixedValues) {
    simulation::Vector2D vec { 3.0f, -7.0f };
    simulation::Vector2D neg = -vec;
    EXPECT_FLOAT_EQ(neg.x, -3.0f);
    EXPECT_FLOAT_EQ(neg.y, 7.0f);
}

TEST(Vector2DTest, NegationDoesNotModifyOriginal) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D neg = -vec;
    EXPECT_FLOAT_EQ(vec.x, 3.0f);
    EXPECT_FLOAT_EQ(vec.y, 4.0f);
}

TEST(Vector2DTest, NegationDoubleNegation) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D doubleNeg = -(-vec);
    EXPECT_FLOAT_EQ(doubleNeg.x, vec.x);
    EXPECT_FLOAT_EQ(doubleNeg.y, vec.y);
}

TEST(Vector2DTest, NegationMagnitudePreserved) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D neg = -vec;
    EXPECT_FLOAT_EQ(vec.magnitude(), neg.magnitude());
}

TEST(Vector2DTest, NegationAdditionCancels) {
    simulation::Vector2D vec { 5.0f, 3.0f };
    simulation::Vector2D result = vec + (-vec);
    EXPECT_FLOAT_EQ(result.x, 0.0f);
    EXPECT_FLOAT_EQ(result.y, 0.0f);
}

// ################## Tests Division ################## //

TEST(Vector2DTest, DivideScalarPositive) {
    simulation::Vector2D vec { 10.0f, 20.0f };
    simulation::Vector2D result = vec / 2.0f;
    EXPECT_FLOAT_EQ(result.x, 5.0f);
    EXPECT_FLOAT_EQ(result.y, 10.0f);
}

TEST(Vector2DTest, DivideScalarNegative) {
    simulation::Vector2D vec { 10.0f, 20.0f };
    simulation::Vector2D result = vec / -2.0f;
    EXPECT_FLOAT_EQ(result.x, -5.0f);
    EXPECT_FLOAT_EQ(result.y, -10.0f);
}

TEST(Vector2DTest, DivideScalarOne) {
    simulation::Vector2D vec { 3.0f, 4.0f };
    simulation::Vector2D result = vec / 1.0f;
    EXPECT_FLOAT_EQ(result.x, 3.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
}

TEST(Vector2DTest, DivideScalarFraction) {
    simulation::Vector2D vec { 1.0f, 2.0f };
    simulation::Vector2D result = vec / 0.5f;
    EXPECT_FLOAT_EQ(result.x, 2.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
}

TEST(Vector2DTest, DivideNegativeVector) {
    simulation::Vector2D vec { -6.0f, -9.0f };
    simulation::Vector2D result = vec / 3.0f;
    EXPECT_FLOAT_EQ(result.x, -2.0f);
    EXPECT_FLOAT_EQ(result.y, -3.0f);
}

TEST(Vector2DTest, DivideNotModifyOriginal) {
    simulation::Vector2D vec { 10.0f, 20.0f };
    simulation::Vector2D result = vec / 2.0f;
    EXPECT_FLOAT_EQ(vec.x, 10.0f);
    EXPECT_FLOAT_EQ(vec.y, 20.0f);
}

TEST(Vector2DTest, DivideByZero) {
    simulation::Vector2D vec { 10.0f, 20.0f };
    simulation::Vector2D result = vec / 0.0f;
    EXPECT_FLOAT_EQ(result.x, 0.0f);
    EXPECT_FLOAT_EQ(result.y, 0.0f);
}
TEST(Vector2DTest, DivideByVerySmallNumber) {
    simulation::Vector2D vec { 10.0f, 20.0f };
    simulation::Vector2D result = vec / 1e-10f;
    EXPECT_FLOAT_EQ(result.x, 0.0f);
    EXPECT_FLOAT_EQ(result.y, 0.0f);
}
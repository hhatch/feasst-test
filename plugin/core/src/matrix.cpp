#include <cmath>
#include "core/include/matrix.h"
#include "core/include/debug.h"
#include "core/include/constants.h"
#include "core/include/utils_math.h"

namespace feasst {

void Matrix::check() const {
  ASSERT(matrix().size() != 0, "empty");
  size_t row_size = matrix()[0].size();
  for (const std::vector<double>& row : matrix_) {
    ASSERT(row.size() != 0, "empty");
    ASSERT(row.size() == row_size, "uneven");
  }
}

void Matrix::transpose() {
  const auto mat = matrix_;
  for (int row = 0; row < static_cast<int>(mat.size()); ++row) {
    for (int col = 0; col < static_cast<int>(mat[row].size()); ++col) {
      matrix_[col][row] = mat[row][col];
    }
  }
}

void Matrix::multiply(const double constant) {
  for (int row = 0; row < static_cast<int>(matrix_.size()); ++row) {
    for (int col = 0; col < static_cast<int>(matrix_[row].size()); ++col) {
      matrix_[row][col] *= constant;
    }
  }
}

Position Matrix::multiply(const Position& vec) const {
  Position result = vec;
  for (int row = 0; row < static_cast<int>(matrix_.size()); ++row) {
    result.set_coord(row, vec.dot_product(matrix_[row]));
  }
  return result;
}

// thanks to https://en.wikipedia.org/wiki/Determinant
double MatrixThreeByThree::determinant() const {
  const double a = value(0, 0);
  const double b = value(0, 1);
  const double c = value(0, 2);
  const double d = value(1, 0);
  const double e = value(1, 1);
  const double f = value(1, 2);
  const double g = value(2, 0);
  const double h = value(2, 1);
  const double i = value(2, 2);
  return a*e*i + b*f*g + c*d*h - c*e*g - b*d*i - a*f*h;
}

void MatrixThreeByThree::invert() {
  const double det = determinant();
  ASSERT(std::abs(det) > NEAR_ZERO, "not invertible");
  transpose();
  multiply(1./det);
}

void MatrixThreeByThree::check() const {
  Matrix::check();
  ASSERT(matrix().size() == 3, "wrong size");
  ASSERT(matrix().size() == matrix()[0].size(), "not square");
}

void RotationMatrix::check() const {
  MatrixThreeByThree::check();
  ASSERT(std::abs(determinant() - 1.) < 10*NEAR_ZERO, "not unit determinant("
    << determinant() << ")");
}

// thanks to https://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
void RotationMatrix::axis_angle(const Position& axis,
    /// angle in degrees
    const double angle) {
  Position unit_axis = axis;
  unit_axis.normalize();
  set_size(unit_axis.size(), unit_axis.size());
  ASSERT(unit_axis.size() == 3, "only implemented for 3D");
  const double radian_angle = degrees_to_radians(angle);
  const double c = cos(radian_angle), C = 1-c;
  const double s = sin(radian_angle);
  const double x = unit_axis.coord(0);
  const double y = unit_axis.coord(1);
  const double z = unit_axis.coord(2);
  set_value(0, 0, x*x*C + c);
  set_value(0, 1, x*y*C - z*s);
  set_value(0, 2, x*z*C + y*s);
  set_value(1, 0, y*x*C + z*s);
  set_value(1, 1, y*y*C + c);
  set_value(1, 2, y*z*C - x*s);
  set_value(2, 0, z*x*C - y*s);
  set_value(2, 1, z*y*C + x*s);
  set_value(2, 2, z*z*C + c);
  check();
}

}  // namespace feasst
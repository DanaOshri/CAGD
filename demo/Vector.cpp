#include "Vector.h"


Vector operator*(double s, const Vector &vec)
{
  return vec * s;
}

double Vector::operator*(const Vector &other) const
{
  double res = data_[0] * other.data_[0] + data_[1] * other.data_[1] + data_[2] * other.data_[2];

  return res;
}

Vector Vector::operator*(double other) const
{
  CAGD_POINT res = { data_[0] * other, data_[1] * other, data_[2] * other };

  return Vector(res);
}

Vector Vector::operator%(const Vector &other) const
{
  CAGD_POINT point;
  point.x = data_[1] * other.data_[2] - data_[2] * other.data_[1];
  point.y = data_[2] * other.data_[0] - data_[0] * other.data_[2];
  point.z = data_[0] * other.data_[1] - data_[1] * other.data_[0];

  return Vector(point);
}

double Vector::norm() const
{
  return sqrt(data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2]);
}

double Vector::get_x() const
{
  return data_[0];
}

double Vector::get_y() const
{
  return data_[1];
}

double Vector::get_z() const
{
  return data_[2];
}

Vector &Vector::normalize()
{
  double denom = norm();
  data_[0] /= denom;
  data_[1] /= denom;
  data_[2] /= denom;

  return *this;
}

Vector::Vector(const CAGD_POINT &p)
{
  data_ = { p.x, p.y, p.z };
}

Vector::Vector(const CAGD_POINT &p1, const CAGD_POINT &p2)
{
  data_ = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
}

Vector::~Vector() {}


#pragma once

#include <vector>
#include <cagd.h>

class Vector
{
public:
  Vector(const CAGD_POINT &point);
  Vector(const CAGD_POINT &, const CAGD_POINT &);
  ~Vector();

  double  operator*(const Vector &) const;
  Vector  operator*(double) const;
  Vector  operator%(const Vector &) const;
  double  norm() const;
  Vector &normalize();

  double get_x() const;
  double get_y() const;
  double get_z() const;

private:
  std::vector<double> data_;

};

Vector operator*(double s, const Vector &vec);
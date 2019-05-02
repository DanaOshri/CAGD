#pragma once

#include <vector>
#include <cagd.h>

class Curve
{
public:
  Curve();
  ~Curve();

  virtual CAGD_POINT  get_point_at_param(double t) const = 0;

  virtual CAGD_POINT *get_control_point(int index) const = 0;
  virtual void        add_control_point(CAGD_POINT point) = 0;

private:
  std::vector<CAGD_POINT *> control_points_;
};

class BezierCurve : public Curve
{
  BezierCurve() {}

  virtual CAGD_POINT get_point_at_param(double t) const;
};

class BSplineCurve : public Curve
{
public:

  BSplineCurve() {}
 
  virtual CAGD_POINT get_point_at_param(double t) const;

  virtual void set_knot_vector(std::vector<int> knots) { knots_ = knots; }
  virtual void set_degree(int degree) { degree_ = degree; }

private:
  std::vector<int> knots_;
  int degree_;
};
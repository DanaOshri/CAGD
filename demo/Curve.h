#pragma once

#include <vector>
#include <cagd.h>
#include <string>
#include "cagd.h"
#include "expr2tree.h"

#define K_NOT_USED 0

class Curve
{
public:
  Curve() {}

  Curve(std::vector<CAGD_POINT *> control_points);
  ~Curve();

  virtual CAGD_POINT  get_point_at_param(double t) const = 0;

  virtual std::vector<CAGD_POINT *> get_control_points() const 
                                    {return control_points_;}

  virtual void        set_control_points(std::vector<CAGD_POINT *> control_points)
                      {control_points_ = control_points;}
  virtual void        draw_curve(std::string path)        = 0;

private:
  std::vector<CAGD_POINT *> control_points_;
};

class BezierCurve : public Curve
{
  BezierCurve(std::vector<CAGD_POINT *> control_points) :
  Curve(control_points){}

  virtual CAGD_POINT get_point_at_param(double t) const;
};

class BSplineCurve : public Curve
{
public:

  BSplineCurve() {}

  BSplineCurve(int degree, 
               std::vector<double> knots,
               std::vector<CAGD_POINT *> control_points);
  
  virtual ~BSplineCurve() {}
 
  virtual CAGD_POINT get_point_at_param(double t) const;

  virtual void set_knot_vector(std::vector<double> knots) { knots_ = knots; }
  virtual void set_degree(int degree) { degree_ = degree; }
  virtual void draw_curve(std::string path);

private:
  std::vector<double> knots_;
  UINT curve_id_;
  int degree_;
};
#include "Curve.h"
#include "Vector.h"
#include "RND_Util.h"


Curve::Curve(std::vector<CAGD_POINT *> control_points)
{
  control_points_ = control_points;
}


Curve::~Curve()
{
  for (int i = 0; i < control_points_.size(); ++i)
  {
    delete control_points_[i];
  }
}

BSplineCurve::
BSplineCurve(int degree, 
             std::vector<double> knots, 
             std::vector<CAGD_POINT *> control_points) : 
  Curve( control_points )
{
  degree_ = degree;
  knots_ = knots;
}


CAGD_POINT 
BSplineCurve::get_point_at_param(double t) const
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  CAGD_POINT point = { x,y,z };

  //find J for it t_J <= t < t_J+1
  int J = -1;
  int n_knots = knots_.size();

  // assume degree_ > 0.
  for ( int i = degree_; i < ( n_knots - degree_ - 1 ); i++ )
  {
    if (knots_[i] <= t && knots_[i + 1] > t)
    {
      J = i;
      break;
    }
  }

  if (J == -1)
    return point;

  std::vector<CAGD_POINT *> control_points = get_control_points();

  int n_control_points = control_points.size();

  std::vector< std::vector<CAGD_POINT> > arr;
  
  arr.resize(degree_+1);
  for (int i = 0; i <= degree_; i++)
  {
    arr[i].resize(n_control_points);
  }

  for (int i = 0; i < n_control_points; i++)
  {
    arr[0][i] = *control_points[i];
  }
  
  //assume #knots = #control_points + degree + 1.
  for (int p = 1; p <= degree_; p++)
  {
    for (int i = J - degree_ + p; i <= J; i++)
    {
      Vector p1(arr[p - 1][i]);
      Vector p2(arr[p - 1][i - 1]);
      double scalar1 = (t - knots_[i]) / (knots_[i + (degree_+1) - p] - knots_[i]);
      double scalar2 = (knots_[i + (degree_+1) - p] - t) / (knots_[i + (degree_+1) - p] - knots_[i]);
      Vector result = scalar1 * p1 + scalar2 * p2;
      arr[p][i] = { result.get_x(), result.get_y(), result.get_z() };
    }
  }
  
  return arr[degree_][J];
}

void 
BSplineCurve::draw_curve(std::string path)
{
  std::vector<double> knots;
  std::vector<CAGD_POINT *> control_points;
  int order;

  get_data_bspline(path, knots, order, control_points);

  set_control_points(control_points);
  set_knot_vector(knots);
  degree_ = order - 1;

  std::vector<CAGD_POINT> loc_curve;

  loc_curve.resize(DENSITY + 1);

  int N_minus_k = knots_.size() - degree_ - 1;
  int k = degree_;

  //The domain
  int t_max = knots_[N_minus_k];
  int t_min = knots_[k];

  double alp = (t_max - t_min) / (double)DENSITY;

  for (double stp = t_min, i = 0; stp < t_max; stp += alp, ++i)
  {   
    CAGD_POINT point = get_point_at_param(stp);
    loc_curve[(int)i] = point;
  }

  cagdSetColor(255, 255, 255);
  curve_id_ = cagdAddPolyline(&loc_curve[0], loc_curve.size() - 1);

  cagdRedraw();  
}
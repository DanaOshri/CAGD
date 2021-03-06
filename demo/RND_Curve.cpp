#include "RND_Curve.h"
#include "RND_Util.h"
#include "Vector.h"

#define _USE_MATH_DEFINES
#include <math.h>

RND_Curve *RND_Curve::my_curve = NULL;

void RND_Curve::reload_curve(std::string path)
{
  if (curve_id_ != K_NOT_USED)
  {
    cagdFreeSegment(curve_id_);
    curve_id_ = K_NOT_USED;
  }

  if (T_id_ != K_NOT_USED)
  {
    cagdFreeSegment(T_id_);
    T_id_ = K_NOT_USED;
  }

  if (N_id_ != K_NOT_USED)
  {
    cagdFreeSegment(N_id_);
    N_id_ = K_NOT_USED;
  }

  if (B_id_ != K_NOT_USED)
  {
    cagdFreeSegment(B_id_);
    B_id_ = K_NOT_USED;
  }

  if (Tor_id_ != K_NOT_USED)
  {
    cagdFreeSegment(Tor_id_);
    Tor_id_ = K_NOT_USED;
  }

  if (K_id_ != K_NOT_USED)
  {
    cagdFreeSegment(K_id_);
    K_id_ = K_NOT_USED;
  }

  draw_curve(path);
}

void RND_Curve::draw_curve(std::string path)
{
  std::vector<std::string> param_eqs(3);

  get_data(path, param_eqs, t_min_, t_max_);

  treeX = e2t_expr2tree(param_eqs[0].c_str());
  treeY = e2t_expr2tree(param_eqs[1].c_str());
  treeZ = e2t_expr2tree(param_eqs[2].c_str());
  std::vector<CAGD_POINT> loc_curve;

  if (treeX && treeY && treeZ)
  {

    curve.resize(DENSITY + 1);
    loc_curve.resize(DENSITY + 1);

    double alp = (t_max_ - t_min_) / ( double )DENSITY;

    for (double stp = t_min_, i = 0; stp <= t_max_; stp += alp, ++i)
    {
      e2t_setparamvalue(stp, E2T_PARAM_T);

      double x = e2t_evaltree(treeX);
      double y = e2t_evaltree(treeY);
      double z = e2t_evaltree(treeZ);

      CAGD_POINT *point = new CAGD_POINT{ x, y, z };
      curve[( int )i] = point;
      loc_curve[( int )i] = *point;
    }

    cagdSetColor(255, 255, 255);
    curve_id_ = cagdAddPolyline(&loc_curve[0], loc_curve.size() - 1);

    cagdRedraw();
  }
  else
  {
    printf("Error loading tree");
  }
}

RND_Curve::RND_Curve(std::string path) : T_id_(K_NOT_USED), N_id_(K_NOT_USED),
                                         B_id_(K_NOT_USED), Tor_id_(K_NOT_USED),
                                         K_id_(K_NOT_USED), curve_id_(K_NOT_USED),
                                         t_max_(0.), t_min_(0.),
                                         lmb_d_(false), rho_(0.)
                                         
{
  draw_curve(path);
}

double RND_Curve::get_t_from_point(int p)
{
  return t_min_ + ((t_max_ - t_min_) / (double)DENSITY) * p;
}

void RND_Curve::T(int point_idx, CAGD_POINT point)
{
  double param = my_curve->get_t_from_point(point_idx);

  e2t_setparamvalue(param, E2T_PARAM_T);

  e2t_expr_node *drv_treeX = e2t_derivtree(treeX, E2T_PARAM_T);
  e2t_expr_node *drv_treeY = e2t_derivtree(treeY, E2T_PARAM_T);
  e2t_expr_node *drv_treeZ = e2t_derivtree(treeZ, E2T_PARAM_T);

  double drv_x = e2t_evaltree(drv_treeX);
  double drv_y = e2t_evaltree(drv_treeY);
  double drv_z = e2t_evaltree(drv_treeZ);

  CAGD_POINT p_vector_1 = { drv_x, drv_y, drv_z };
  Vector beta_t = Vector(p_vector_1);

  Vector T = beta_t.normalize();

  std::vector<CAGD_POINT> vector;

  vector.push_back(point);

  CAGD_POINT second_point = { T.get_x() + point.x, T.get_y() + point.y, T.get_z() + point.z };

  vector.push_back(second_point);

  if (T_id_ != K_NOT_USED)
    cagdFreeSegment(T_id_);

  cagdSetColor(0, 255, 0);
  T_id_ = cagdAddPolyline(&vector[0], vector.size());
}

void RND_Curve::N(int point_idx, CAGD_POINT point)
{
  double param = my_curve->get_t_from_point(point_idx);

  e2t_setparamvalue(param, E2T_PARAM_T);

  e2t_expr_node *drv_treeX = e2t_derivtree(treeX, E2T_PARAM_T);
  e2t_expr_node *drv_treeY = e2t_derivtree(treeY, E2T_PARAM_T);
  e2t_expr_node *drv_treeZ = e2t_derivtree(treeZ, E2T_PARAM_T);

  double drv_x = e2t_evaltree(drv_treeX);
  double drv_y = e2t_evaltree(drv_treeY);
  double drv_z = e2t_evaltree(drv_treeZ);

  CAGD_POINT p_vector_1 = { drv_x, drv_y, drv_z };
  Vector beta_t = Vector(p_vector_1);

  e2t_expr_node *drv_drv_treeX = e2t_derivtree(drv_treeX, E2T_PARAM_T);
  e2t_expr_node *drv_drv_treeY = e2t_derivtree(drv_treeY, E2T_PARAM_T);
  e2t_expr_node *drv_drv_treeZ = e2t_derivtree(drv_treeZ, E2T_PARAM_T);

  double drv_drv_x = e2t_evaltree(drv_drv_treeX);
  double drv_drv_y = e2t_evaltree(drv_drv_treeY);
  double drv_drv_z = e2t_evaltree(drv_drv_treeZ);

  CAGD_POINT p_vector_2 = { drv_drv_x, drv_drv_y, drv_drv_z };
  Vector beta_tt = Vector(p_vector_2);

  Vector N = (beta_t % beta_tt).normalize() % beta_t.normalize();

  std::vector<CAGD_POINT> vector;

  vector.push_back(point);

  CAGD_POINT second_point = { N.get_x() + point.x, N.get_y() + point.y, N.get_z() + point.z };

  vector.push_back(second_point);


  if (N_id_ != K_NOT_USED)
    cagdFreeSegment(N_id_);

  cagdSetColor(0, 0, 255);
  N_id_ = cagdAddPolyline(&vector[0], vector.size());
}

void RND_Curve::B(int point_idx, CAGD_POINT point)
{
  double param = my_curve->get_t_from_point(point_idx);

  e2t_setparamvalue(param, E2T_PARAM_T);

  e2t_expr_node *drv_treeX = e2t_derivtree(treeX, E2T_PARAM_T);
  e2t_expr_node *drv_treeY = e2t_derivtree(treeY, E2T_PARAM_T);
  e2t_expr_node *drv_treeZ = e2t_derivtree(treeZ, E2T_PARAM_T);

  double drv_x = e2t_evaltree(drv_treeX);
  double drv_y = e2t_evaltree(drv_treeY);
  double drv_z = e2t_evaltree(drv_treeZ);

  CAGD_POINT p_vector_1 = { drv_x /*- point.x*/, drv_y /*- point.y*/, drv_z /*- point.z*/ };
  Vector beta_t = Vector(p_vector_1);

  e2t_expr_node *drv_drv_treeX = e2t_derivtree(drv_treeX, E2T_PARAM_T);
  e2t_expr_node *drv_drv_treeY = e2t_derivtree(drv_treeY, E2T_PARAM_T);
  e2t_expr_node *drv_drv_treeZ = e2t_derivtree(drv_treeZ, E2T_PARAM_T);

  double drv_2_x = e2t_evaltree(drv_drv_treeX);
  double drv_2_y = e2t_evaltree(drv_drv_treeY);
  double drv_2_z = e2t_evaltree(drv_drv_treeZ);

  CAGD_POINT p_vector_2 = { drv_2_x, drv_2_y, drv_2_z };
  Vector beta_tt = Vector(p_vector_2);

  e2t_expr_node *drv_3_treeX = e2t_derivtree(drv_drv_treeX, E2T_PARAM_T);
  e2t_expr_node *drv_3_treeY = e2t_derivtree(drv_drv_treeY, E2T_PARAM_T);
  e2t_expr_node *drv_3_treeZ = e2t_derivtree(drv_drv_treeZ, E2T_PARAM_T);

  double drv_3_x = e2t_evaltree(drv_3_treeX);
  double drv_3_y = e2t_evaltree(drv_3_treeY);
  double drv_3_z = e2t_evaltree(drv_3_treeZ);

  CAGD_POINT p_vector_3 = { drv_3_x, drv_3_y, drv_3_z };
  Vector beta_ttt = Vector(p_vector_3);

  Vector B = (beta_t % beta_tt).normalize();

  double k_denom = (beta_t % beta_tt).norm();

  rho_ = (beta_ttt * B) / k_denom;
  Vector N = (beta_t % beta_tt).normalize() % beta_t.normalize();

  Vector torsion_vec = -rho_ * N.normalize();

  std::vector<CAGD_POINT> vector;

  vector.push_back(point);

  CAGD_POINT second_point = { B.get_x() + point.x, B.get_y() + point.y, B.get_z() + point.z };

  vector.push_back(second_point);

  std::vector<CAGD_POINT> tor_vec;

  tor_vec.push_back(point);

  CAGD_POINT tor_point = { torsion_vec.get_x() + point.x, torsion_vec.get_y() + point.y, torsion_vec.get_z() + point.z };

  tor_vec.push_back(tor_point);


  if (Tor_id_ != K_NOT_USED)
    cagdFreeSegment(Tor_id_);

  if (B_id_ != K_NOT_USED)
    cagdFreeSegment(B_id_);

  cagdSetColor(255, 0, 255);
  Tor_id_ = cagdAddPolyline(&tor_vec[0], tor_vec.size());

  cagdSetColor(255, 0, 0);
  B_id_ = cagdAddPolyline(&vector[0], vector.size());
}

void RND_Curve::K(int point_idx, CAGD_POINT point)
{
  double param = my_curve->get_t_from_point(point_idx);

  e2t_setparamvalue(param, E2T_PARAM_T);

  e2t_expr_node *drv_treeX = e2t_derivtree(treeX, E2T_PARAM_T);
  e2t_expr_node *drv_treeY = e2t_derivtree(treeY, E2T_PARAM_T);
  e2t_expr_node *drv_treeZ = e2t_derivtree(treeZ, E2T_PARAM_T);

  double drv_x = e2t_evaltree(drv_treeX);
  double drv_y = e2t_evaltree(drv_treeY);
  double drv_z = e2t_evaltree(drv_treeZ);

  CAGD_POINT p_vector_1 = { drv_x, drv_y, drv_z };
  Vector beta_t = Vector(p_vector_1);

  e2t_expr_node *drv_drv_treeX = e2t_derivtree(drv_treeX, E2T_PARAM_T);
  e2t_expr_node *drv_drv_treeY = e2t_derivtree(drv_treeY, E2T_PARAM_T);
  e2t_expr_node *drv_drv_treeZ = e2t_derivtree(drv_treeZ, E2T_PARAM_T);

  double drv_drv_x = e2t_evaltree(drv_drv_treeX);
  double drv_drv_y = e2t_evaltree(drv_drv_treeY);
  double drv_drv_z = e2t_evaltree(drv_drv_treeZ);

  CAGD_POINT p_vector_2 = { drv_drv_x, drv_drv_y, drv_drv_z };
  Vector beta_tt = Vector(p_vector_2);

  double K = (beta_t % beta_tt).norm() / (std::pow(beta_t.norm(), 3));

  double radius = 1 / K;

  Vector T = beta_t.normalize();
  Vector N = (beta_t % beta_tt).normalize() % T;

  CAGD_POINT center = { point.x + radius * N.get_x(), point.y + radius * N.get_y(), point.z + radius * N.get_z() };

  std::vector<CAGD_POINT> circle(DENSITY+1);

  double alp = (2*M_PI) / (double)DENSITY;

  for (double stp = 0, i = 0; stp <= 2*M_PI; stp += alp, ++i)
  {
    double x = center.x + radius * std::cos(stp) * T.get_x() + radius * std::sin(stp) * N.get_x();
    double y = center.y + radius * std::cos(stp) * T.get_y() + radius * std::sin(stp) * N.get_y();
    double z = center.z + radius * std::cos(stp) * T.get_z() + radius * std::sin(stp) * N.get_z();

    CAGD_POINT point = { x, y, z };
    circle[(int)i] = point;
  }


  if (K_id_ != K_NOT_USED)
    cagdFreeSegment(K_id_);

  cagdSetColor(255, 255, 0);
  K_id_ = cagdAddPolyline(&circle[0], circle.size() - 1);
}
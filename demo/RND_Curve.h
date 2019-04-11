#pragma once

#include <string>
#include <vector>
#include "cagd.h"
#include "expr2tree.h"


//-------------------------------------------------------------------------------------
class RND_Curve
{
public:
  static RND_Curve *get_instance(std::string path)
  {
    if (my_curve == NULL)
      my_curve = new RND_Curve(path);

    return my_curve;
  }

public:
  void T(int point_idx, CAGD_POINT point);
  void N(int point_idx, CAGD_POINT point);
  void B(int point_idx, CAGD_POINT point);
  void K(int point_idx, CAGD_POINT point);

  UINT get_T_id() const { return T_id_; }
  UINT get_N_id() const { return N_id_; }
  UINT get_B_id() const { return B_id_; }
  UINT get_K_id() const { return K_id_; }

  double get_t_from_point(int p);

public:
  std::vector<CAGD_POINT *> curve;
  e2t_expr_node *treeX, *treeY, *treeZ;
  static RND_Curve *my_curve;

private:
  RND_Curve(std::string path);
  UINT T_id_;
  UINT N_id_;
  UINT B_id_;
  UINT K_id_;
  double t_min_, t_max_;

};
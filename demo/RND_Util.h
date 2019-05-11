#pragma once

#include <string>
#include <vector>
#include "cagd.h"

#define DENSITY 10000

void get_data(const std::string &path, 
              std::vector<std::string> &param_eqs, 
              double &tmin, 
              double &tmax);
void get_data_bspline(const std::string &path, 
                      std::vector<double> &knots, 
                      int &order, 
                      std::vector<CAGD_POINT *> &points);
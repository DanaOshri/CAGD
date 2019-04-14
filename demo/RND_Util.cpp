#include "RND_Util.h"
#include "RND_Curve.h"
#include <sstream>
#include <fstream>

void get_data(const std::string &path, std::vector<std::string> &param_eqs, double &tmin, double &tmax)
{
  std::string line;
  std::ifstream infile(path);
  param_eqs.resize(3);
  int prm_count = 0;

  while (std::getline(infile, line))
  {
    // Test for empty line.
    if (line.empty())
      continue;

    // Test for #line comment.
    std::istringstream iss(line);
    char hash;

    iss >> hash;
    if (hash == '#')
      continue;

    // Parse equation line.
    if (prm_count < 3)
    {
      param_eqs[prm_count] = line;
      ++prm_count;
    }
    // Parse parameters line.
    else
    {
      size_t ind = line.find(',');

      if (ind != std::string::npos)
        line.replace(line.find(','), 1, " ");

      std::istringstream params_stream(line);
      std::string str_prm;

      params_stream >> tmin >> tmax;
    }
  }
}

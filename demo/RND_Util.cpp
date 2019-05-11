#include "RND_Util.h"
#include "RND_Curve.h"
#include <sstream>
#include <fstream>
#include <iterator>

template<char delimiter>
class WordDelimitedBy : public std::string
{};

void get_data(const std::string              &path, 
                    std::vector<std::string> &param_eqs, 
                    double                   &tmin, 
                    double                   &tmax)
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

void 
get_data_bspline( const std::string         &path, 
                  std::vector<double>       &knots, 
                  int                       &order, 
                  std::vector<CAGD_POINT *> &points )
{
  std::string line;
  std::ifstream infile(path);
  int param_cnt = 0;
  int knots_cnt = 0;
  int num_knots = 0;

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
    {
      continue;
    }

    //First parameter is the bspline degree.
    if (param_cnt == 0)
    {
      order = std::stoi(line);
      param_cnt++;
    }

    //Second parameter is the bspline knots list.
    else if (param_cnt == 1 && num_knots == 0)
    {
      //Get the number of knots ([num_knots]).
      size_t ind1 = line.find('[');
      size_t ind2 = line.find(']');
      
      std::string str_num_knots = line.substr((ind1 + 1), (ind2 - ind1 - 1));
      num_knots = std::stoi(str_num_knots);

      knots.resize(num_knots);

      //Get the knots.
      size_t ind3 = line.find('=');
      std::string knots_str = line.substr(ind3 + 1, line.size());

      std::istringstream knots_iss(knots_str);
      std::vector<std::string> results((std::istream_iterator<WordDelimitedBy<' '>>(knots_iss)),
                                        std::istream_iterator<WordDelimitedBy<' '>>());

      for (int i = 0; i < results.size(); ++i)
      {
        knots[knots_cnt] = std::stod(results[i]);
        knots_cnt++;
      }
    }

    else if (param_cnt == 1 && num_knots > 0 && knots_cnt < num_knots )
    {
      //Get the knots.
      size_t ind = line.find('\t');
      std::string knots_str = line.substr(ind + 1, line.size());

      std::istringstream knots_iss(knots_str);
      std::vector<std::string> results((std::istream_iterator<WordDelimitedBy<' '>>(knots_iss)),
                                        std::istream_iterator<WordDelimitedBy<' '>>());

      for (int i = 0; i < results.size(); ++i)
      {
        knots[knots_cnt] = std::stod(results[i]);
        knots_cnt++;
      }
    }

    //Third parameter is the control points list.
    else if (param_cnt == 1 && num_knots == knots_cnt)
      param_cnt++;

    //Each line represent a point (x_value, y_value, z_value).
    if (param_cnt == 2)
    {
      double x, y, z;
      
      size_t ind = line.find('\t');
      std::string point_str = line.substr(ind + 1, line.size());

      std::istringstream point_iss(point_str);
      std::vector<std::string> results((std::istream_iterator<WordDelimitedBy<' '>>(point_iss)),
                                        std::istream_iterator<WordDelimitedBy<' '>>());
      if (results.size() == 0)
        continue;

      CAGD_POINT *point = new CAGD_POINT();
      
      point->x = std::stod(results[0]);
      point->y = std::stod(results[1]);;
      point->z = std::stod(results[2]);;
      
      points.push_back(point);
    }
  }

}
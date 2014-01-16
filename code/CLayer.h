#pragma once
//athour xu liu 07/10/13
//the class of layer
#include <vector>
#include <fstream>
#include <string>
#include <map>
class CMap;
class CPoint;
class CContour;
extern const int MatrixRes;
extern const int NumRows;
extern const int NumCols;
extern const float layer_interval_scale;
extern const int distance_medialaxis_contour;
class CLayer
{
 public:
  CLayer(const float ID);
  std::string filename;
  float LayerID;
  int contour_Num;
  
  void operator=(CLayer &temp);
  bool read_layer_multi_contour_with_z(std::fstream& fin);
  void setup(CPoint);



  std::vector<CPoint*> vec_medial_points;
  CPoint* center_point;
  CPoint* moment_one_point;
  double max_x,max_y,min_x,min_y;

  void reset();
  std::map<int,int> map_contourID;//contour_Num,contourID
  std::map<int,CContour*> map_contour;//contourID,CContour*
  double** medial_axis;
  int medial_axis_count;
private:
  void check_edge(double,double);
  void get_center();
  void calculate_one_moment();
  float sum_x,sum_y;
  double last_x,last_y;
  void calculate_medial_axis();
};

#include"CContour.h"
#include"CPoint.h"
#include "assert.h"
#include "CLayer.h"
using namespace std;

CContour::CContour(const float ID,CLayer* layer):lattice_x(NULL),lattice_y(NULL)
{
  m_layer=layer;
  contourID=ID;
  filename="";
  max_x=-9999999;
  max_y=-9999999;
  min_x=9999999;
  min_y=9999999;
  center_point= new CPoint();
  moment_one_point= new CPoint();
  length=0;
  last_x=last_y=9999999;
  use_as_higher_contour_count=0;
  use_as_lower_contour_count=0;
}

void CContour:: operator=(CContour &temp)
{
  contourID = temp.contourID;
  m_Map = temp.m_Map;
  sum_x=sum_y=0;
}
#include "stdio.h"
#include "iostream"
void CContour:: check_edge(double tempx,double tempy )
{
    if (tempx>max_x)
    {
      max_x=tempx;
    }
    if (tempx<min_x)
    {
      min_x=tempx;
    }
    sum_x+=tempx;

    if (tempy>max_y)
    {
      max_y=tempy;
    }
    if(tempy<min_y)
    {
      min_y=tempy;
    }
    sum_y+=tempy;
    if (last_x==9999999&&last_y==9999999)
    {
      last_x=tempx;last_y=tempy;
    }
    length+=(tempx-last_x)*(tempx-last_x)+(tempy-last_y)*(tempy-last_y);
    // std::cout<<length<<"\t"<<tempx<<"\t"<<last_x<<"\t"<<tempy<<"\t"<<last_y<<"\n";
    last_x=tempx;last_y=tempy;
}

void CContour::get_center()
{
  center_point->x=(max_x+min_x)/2;
  center_point->y=(max_y+min_y)/2;
  center_point->z=LayerID;//because when do the normalize, we don't want change the z level;
}

void CContour::calculate_one_moment()
{
  moment_one_point->x=sum_x/(PointNum*1.0);
  moment_one_point->y=sum_y/(PointNum*1.0);
  moment_one_point->z=LayerID;
}
bool CContour::read_single_layer_without_z(fstream& fin)
{
  int oldz=0,newz=-9999;

  double tempvalue;
  while(fin.good())
  {
    CPoint *temp=new CPoint();
    fin>>temp->x;
    fin>>temp->y;
    check_edge(temp->x,temp->y);
    temp->z=LayerID*10;//TODO
    if(fin.good())
    {
      vec_points.push_back(temp);
    }
  }
  PointNum=vec_points.size();
  get_center();
  calculate_one_moment();
  return true;
}


bool CContour::read_contour_with_z(fstream& fin)
{
  int levelID=-1,temp_contourID=-1;
  int pos=-1;
  if(fin.good())
    {
      pos=fin.tellg();
      fin>>levelID;
      fin>>temp_contourID;
      if (temp_contourID!=contourID)
      {
        return false;
      }
      LayerID=levelID;
      fin.seekg(pos);
    }
  else
    {
      return false;
    }
  int count=0;
  while(fin.good())
    {
      count++;
      CPoint *temp=new CPoint();
      int pos=fin.tellg();
      fin>>temp->z;
      fin>>temp_contourID;
      fin>>temp->x;
      fin>>temp->y;
    

      if(temp_contourID!=contourID||temp->z!=LayerID)
	{
          fin.seekg(pos);
	  delete temp;
	  break;
	}
      else if(count%interval_of_point==0)
	{
          temp->z=temp->z*layer_interval_scale;
          temp->index=temp->get_index();
	  vec_points.push_back(temp);
          check_edge(temp->x,temp->y);
	}
      if(fin.eof())
      {
        break;
      }
    }
  PointNum=vec_points.size();
  get_center();
  calculate_one_moment();
  return true;
}

void CContour::InitMap()
{
  // normalize();
  m_Map= new CMap(this);
  m_Map->setup();
  m_Map->gradient();
}

void CContour::normalize(CPoint shape_center_point)//may be use the moment point
{
  std::vector<CPoint*>::iterator itr,etr;
  itr=vec_points.begin();
  etr=vec_points.end();

  int map_center_x=NumRows/2;
  int map_center_y=NumCols/2;
  for (;itr!=etr;++itr)
  {
    CPoint* temp = new CPoint();
    temp->x=(*itr)->x-shape_center_point.x + map_center_x;
    temp->y=(*itr)->y-shape_center_point.y + map_center_y;
    temp->z=(*itr)->z;
    vec_Points_Origin.push_back(temp);
  }
  // center_point->x=center_point->x-shape_center_point.x+map_center_x;
  // center_point->y=center_point->y-shape_center_point.y+map_center_y;
  // center_point->z=LayerID;

  // moment_one_point->x=moment_one_point->x-shape_center_point.x+map_center_x;
  // moment_one_point->y=moment_one_point->y-shape_center_point.y+map_center_y;
  // moment_one_point->z=LayerID;
}

void CContour::reset()
{
  vec_Points_Inter.clear();
  vec_Points_Vicinity.clear();
  vec_new_points_vicinity.clear();//change after bspline_update
}

void CContour::smooth()
{
  float smooth_factor=30.0;
  int size=vec_Points_Origin.size();
  for (int i = 0; i < size; ++i)
  {
    CPoint temp1 = *vec_Points_Origin[i];
    for(int j=1;j < smooth_factor;j++ )
    {
      CPoint temp2 = *vec_Points_Origin[(i+j)%size];
      temp1 = (temp1 + temp2);
    }
    CPoint temp = temp1/smooth_factor;
    *vec_Points_Origin[i]= temp;
  } 
}

void CContour::calculate_medial_map(double** medial_axis)
{
  m_medial_map = new CMedialMap(this,medial_axis);
  m_medial_map->setup();
  m_medial_map->gradient();
}

void CContour::swap_map_medialmap()
{
  assert(m_Map);
  assert(m_medial_map);
  m_temp_map=m_Map;
  m_Map=(CMap*)m_medial_map;
}

void CContour::swap_medialmap_map()
{
  assert(m_Map);
  assert(m_medial_map);
  m_medial_map=(CMedialMap*)m_Map;
  m_Map=m_temp_map;
  delete m_medial_map;
}

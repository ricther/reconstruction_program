#include "CSkeleton.h"
#include "CContour.h"
#include "assert.h"
#include "iostream"

using namespace std;
CSkeletalPoint::CSkeletalPoint(int id)
{
  ID=id;
  color=0;
  //  pre_node=NULL;
  //  next_node=NULL;
  use_as_lower_skeletal_counter=0;
  use_as_higher_skeletal_counter=0;
  node=NULL;
}

CBranch::CBranch(int id,std::map<int,CSkeletalPoint*>&map):map_id_skeletal(map)
{
  branchID=id;
  map_id_spoint.clear();
}

void CBranch::build_branch(int sign, CPoint* lower_point, std::map<float,CLayer*>::iterator nitr,std::map<float,CLayer*>::iterator etr)
{
  if (sign>=0)// lower_contour_num>higher_contour_num
  {
    CSkeletalPoint* temp_point=check_skeletalpoint(lower_point);

    temp_point->color+=1<<branchID;
    map_id_spoint.insert(make_pair(nodeID_counter++,temp_point));
    while(nitr!=etr)
    {
      CPoint* next_point=find_next_skeletalpoint(sign,lower_point,nitr->second);
      temp_point=check_skeletalpoint(next_point);
      temp_point->node=next_point;
      temp_point->color+=1<<branchID;
      map_id_spoint.insert(make_pair(nodeID_counter++,temp_point));
      ++nitr;
      lower_point = next_point;
    }
  }
  if (sign<0)
  {
    CSkeletalPoint* temp_point=check_skeletalpoint(lower_point);
    temp_point->color+=1<<branchID;
    //    map_id_spoint.insert(make_pair(nodeID_counter++,temp_point));
    std::map<float,CLayer*>::iterator itr=nitr;
    itr--;
    CPoint* pre_point=find_next_skeletalpoint(sign,lower_point,itr->second);
    CSkeletalPoint* pre_node=check_skeletalpoint(pre_point);
    pre_node->color+=1<<branchID;
    map_id_spoint.insert(make_pair(nodeID_counter++,pre_node));
    map_id_spoint.insert(make_pair(nodeID_counter++,temp_point));
    nitr++;
    if (nitr==etr)
    {
      assert(false);
    }
    while(nitr!=etr)
    {
      CPoint* next_point=find_next_skeletalpoint(sign,lower_point,nitr->second);
      temp_point=check_skeletalpoint(next_point);
      temp_point->node=next_point;
      temp_point->color+=1<<branchID;
      map_id_spoint.insert(make_pair(nodeID_counter++,temp_point));
      ++nitr;
      lower_point = next_point;
    }
  }
}

CSkeletalPoint* CBranch::check_skeletalpoint(CPoint* point)
{
  itr=map_id_skeletal.begin();etr=map_id_skeletal.end();
  for (; itr!=etr; ++itr)
  {
    if (itr->second->node == point)
    {
      return itr->second;
    }
  }
  return new_skeletalpoint(point);
}

CSkeletalPoint* CBranch::new_skeletalpoint(CPoint* point)
{
  assert(false);
  CSkeletalPoint* temp_point= new CSkeletalPoint(nodeID_counter++);
  temp_point->node=point;
  map_id_skeletal.insert(make_pair(temp_point->ID,temp_point));
  return temp_point;
}
CPoint* CBranch::find_next_skeletalpoint(int sign,CPoint* point,CLayer* next_layer)
{
  std::map<int,CContour*>::iterator itr,etr;
  CPoint* temp_point=NULL;
  itr=next_layer->map_contour.begin();etr=next_layer->map_contour.end();
  double dis=999999;
  CPoint* point1,*point2;
  for (;itr!=etr;++itr)
  {
    //    CPoint* point1= lower_contour->center_point;
    //    CPoint* point2= (itr->second)->center_point;
    point1= point;
    point2= (itr->second)->center_point;
    float tempdis=(point1->x-point2->x)*(point1->x-point2->x)+(point1->y-point2->y)*(point1->y-point2->y);
    if (tempdis<dis)
    {

      CSkeletalPoint* node= check_skeletalpoint(point2);
      if (sign>=0)
      {
        if (node->use_as_lower_skeletal_counter <= sign)
        {
          dis=tempdis;
          node->use_as_lower_skeletal_counter++;
          temp_point=point2;
        }
      }
      if (sign<0)
      {
        if (node->use_as_higher_skeletal_counter <= -sign)
        {
          dis=tempdis;
          node->use_as_higher_skeletal_counter++;
          temp_point=point2;
        }

      }
    }
  }
  return temp_point;
}

CSkeleton::CSkeleton()
{
  branchID_counter=0;
  skeletal_point_counter=0;
  map_id_branch.clear();
}

void CSkeleton::build_skeleton(map<float,CLayer*>& map_layer)
{
  initial_skeletal_points(map_layer);
  std::map<float,CLayer*>::iterator itr=map_layer.begin(),etr=map_layer.end(),nitr;
  nitr=itr;
  int sign=-2,old_sign=-2;
  for (; itr!=etr; ++itr)
  {
    nitr++;
    if (nitr==etr)
    {
      break;
    }
    int lower_layer_contour_num=itr->second->contour_Num;
    int higher_layer_contour_num=nitr->second->contour_Num;
    int sign=check_contour_num(lower_layer_contour_num,higher_layer_contour_num);
    if (old_sign!=sign)
    {
      for (int i = 0; i < lower_layer_contour_num; ++i)
      {
        CBranch* new_branch=new CBranch(branchID_counter,map_id_skeletal);
        branchID_counter++;
        int id=itr->second->map_contourID[i];
        CContour* temp_contour=itr->second->map_contour[id];
        new_branch->build_branch(sign,temp_contour->center_point,nitr,etr);
        map_id_branch.insert(make_pair(new_branch->branchID,new_branch));
      }
    }
    break;
  }
}


void CSkeleton::build_skeleton_new(std::map<float ,CLayer*>& map_layer)
{
  initial_skeletal_points(map_layer);
  std::map<float,CLayer*>::iterator itr=map_layer.begin(),etr=map_layer.end(),nitr;
  std::queue< skeletal_branch > m_queue;
  int sign=-2,old_sign=-2,old_higher_num=-1;
  for (; itr!=etr; ++itr)
  {
    nitr=itr;
    nitr++;
    if (nitr==etr)
    {
      break;
    }
    int lower_layer_contour_num=itr->second->contour_Num;
    int higher_layer_contour_num=nitr->second->contour_Num;
    int sign=check_contour_num(lower_layer_contour_num,higher_layer_contour_num);
    if (higher_layer_contour_num!=old_higher_num)
    {
      if (m_queue.size()!=0)
      {
        m_queue.back().etr=nitr;//etr=nitr,cause when use loop itr==etr finish the loop, and the pre-etr need calculate, so etr = nitr;
      }
      insert_queue(itr,etr,m_queue);
      cout<<"branch seciton:"<<itr->second->LayerID<<"\n";
      old_higher_num=higher_layer_contour_num;
    }
  }
  build_skeleton_from_queue(m_queue);
}


void CSkeleton::insert_queue(std::map<float,CLayer*>::iterator itr, std::map<float,CLayer*> :: iterator etr, std::queue<skeletal_branch>& queue)
{

  skeletal_branch temp;
  temp.itr=itr;
  temp.etr=etr;
  queue.push(temp);
    
}
//if the high num changed , deal it specially.
void CSkeleton::build_skeleton_from_queue(std::queue<skeletal_branch>& queue)
{
  int N=queue.size();
  for (int i = 0; i < N; ++i)
  {
    skeletal_branch temp=queue.front();
    queue.pop();
    std::map<float,CLayer*> ::iterator itr=temp.itr,etr=temp.etr;
    std::map<float,CLayer*> ::iterator nitr=itr;
    nitr++;
    int lower_layer_contour_num=itr->second->contour_Num;
    int higher_layer_contour_num=nitr->second->contour_Num;
    int sign=check_contour_num(lower_layer_contour_num,higher_layer_contour_num);
    if (sign>=0)
    {
      for (int i = 0; i < lower_layer_contour_num; ++i)
      {
        CBranch* new_branch=new CBranch(branchID_counter,map_id_skeletal);
        branchID_counter++;
        int id=itr->second->map_contourID[i];
        CContour* temp_contour=itr->second->map_contour[id];
        new_branch->build_branch(sign,temp_contour->center_point,nitr,etr);
        map_id_branch.insert(make_pair(new_branch->branchID,new_branch));
      }
    }
    else
    {
      for (int i = 0; i < higher_layer_contour_num; ++i)
      {
        CBranch* new_branch=new CBranch(branchID_counter,map_id_skeletal);
        branchID_counter++;
        int id=nitr->second->map_contourID[i];
        CContour* temp_contour=nitr->second->map_contour[id];
        new_branch->build_branch(sign,temp_contour->center_point,itr,etr);
        map_id_branch.insert(make_pair(new_branch->branchID,new_branch));
      }
    }
  }
}
void CSkeleton::initial_skeletal_points(map<float,CLayer*>& map_layer)
{
  std::map<float,CLayer*>::iterator itr=map_layer.begin(),etr=map_layer.end();
  for (; itr!=etr; ++itr)
  {
    std::map<int,CContour*>::iterator c_itr=itr->second->map_contour.begin(),c_etr=itr->second->map_contour.end();
    for (; c_itr!=c_etr; ++c_itr)
    {
      CSkeletalPoint *temp_point=new CSkeletalPoint(skeletal_point_counter);
      temp_point->node=c_itr->second->center_point;
      map_id_skeletal.insert(make_pair(skeletal_point_counter++,temp_point));
      cout<<(temp_point->node)->x<<","<<(temp_point->node)->y<<","<<(temp_point->node)->z<<"\n";
    }
  }  
}
int CSkeleton::check_contour_num(int lower_num,int higher_num)
{
  return lower_num-higher_num;
}

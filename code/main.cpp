#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkContourWidget.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkDebugLeaks.h>
#include <vtkCamera.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkMath.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkPolyDataMapper.h>
#include <vtkLightCollection.h>
#include "CShape.h"
#include "fstream"
#include "string"
#include <unistd.h>
#include "CPoint.h"
#include "CLayer.h"
#include <vtkLight.h>
using namespace std;

const int MatrixRes=56;
const int NumRows=500;
const int NumCols=500;
const int interval_of_point=1;
const float layer_interval_scale=5;
const int interval_of_line_layer=1;// the interval of the lines within one layer
CShape m_source;


void init_data(CShape&);
void vtk_setup();
void vtk_draw_view1(vtkSmartPointer<vtkRenderWindow>,vtkSmartPointer<vtkRenderWindowInteractor>);
void vtk_draw_view2(vtkSmartPointer<vtkRenderWindow>,vtkSmartPointer<vtkRenderWindowInteractor>);
void vtk_draw_view3(vtkSmartPointer<vtkRenderWindow>,vtkSmartPointer<vtkRenderWindowInteractor>);
void vtk_draw_view4(vtkSmartPointer<vtkRenderWindow>,vtkSmartPointer<vtkRenderWindowInteractor>);
void vtk_draw_view5(vtkSmartPointer<vtkRenderWindow>,vtkSmartPointer<vtkRenderWindowInteractor>);
int main( int argc, char *argv[] )
{
  // Create the RenderWindow, Renderer and both Actors
  //

  init_data(m_source);
  
  vtk_setup();
  
  return EXIT_SUCCESS;
}


void init_data(CShape&m_source)
{
  char currentPath[200];
  getcwd(currentPath, sizeof(currentPath));
  fstream fin("../config3",ios_base::in);
  std::vector<string> vec_filename;
  string temp;
  if(fin.is_open())
  {
    while(!fin.eof())
    {
      fin>>temp;
      if(temp!="end")
      {
        vec_filename.push_back(temp);
      }
    }
  }
  

  m_source.initial(vec_filename);
  m_source.Setup();
  m_source.Registration();
}

void vtk_setup()
{
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  m_source.initial_display(renderWindow,interactor);
  interactor->SetRenderWindow(renderWindow);  
  //  renderWindow->SetFullScreen(true);
  renderWindow->SetSize(600,600);
  int N=5;
  for (int i = 0; i < N; ++i)
  {
    switch(i+1)
    {
      case 1:
        vtk_draw_view1(renderWindow,interactor);
        break;
      case 2:
        vtk_draw_view2(renderWindow,interactor);
        break;
      case 3:
        vtk_draw_view3(renderWindow,interactor);
        break;
      case 4:
        vtk_draw_view4(renderWindow,interactor);
        break;
      case 5:
        vtk_draw_view5(renderWindow,interactor);
        break;
    }
  }
  renderWindow->Render();
  interactor->Initialize();
  interactor->Start();
}

double xmins[5] = {0,.25,0.5,0,.5};
double xmaxs[5] = {0.25,0.50,1,0.5,1};
double ymins[5] = {0,0,0,.5,.5};
double ymaxs[5]= {0.5,0.5,0.5,1,1};


void vtk_draw_view1(vtkSmartPointer<vtkRenderWindow> renderWindow,vtkSmartPointer<vtkRenderWindowInteractor> interactor)
{
  vtkSmartPointer<vtkRenderer> renderer= vtkSmartPointer<vtkRenderer>::New();
  //  renderer->RemoveLight( renderer->GetLights()->GetNextItem());
  renderWindow->AddRenderer(renderer);
  
  renderer->SetBackground(0.1, 0.2, 0.4);

  renderer->SetViewport(xmins[0],ymins[0],xmaxs[0],ymaxs[0]);
  
  //  m_source.m_display->draw_origin_points(renderer);
  m_source.m_display->draw_normal_points(renderer);
  
  renderer->ResetCamera();

  // renderWindow->Render();
}

void vtk_draw_view2(vtkSmartPointer<vtkRenderWindow> renderWindow,vtkSmartPointer<vtkRenderWindowInteractor> interactor)
{
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(renderer);
  
  //renderer->RemoveLight( renderer->GetLights()->GetNextItem());
  vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
  light->SetLightTypeToSceneLight();
  light->SetPosition(0, 0, 0);
  light->SetFocalPoint(250,250,250);
  light->SetColor(1,1,1);
  light->SetPositional(true); // required for vtkLightActor below
  renderer->AddLight(light);
  renderer->UpdateLightsGeometryToFollowCamera();
  light->SetSwitch(true);
  // light->SetConeAngle(10);
  // light->SetFocalPoint(lightFocalPoint[0], lightFocalPoint[1], lightFocalPoint[2]);
  // light->SetDiffuseColor(1,0,0);
  // light->SetAmbientColor(0,1,0);
  // light->SetSpecularColor(0,0,1);
  
  renderer->SetBackground(0.1, 0.2, 0.4);

  renderer->SetViewport(xmins[1],ymins[1],xmaxs[1],ymaxs[1]);
  
  //  m_source.m_display->draw_normal_points(renderer);
  m_source.m_display->draw_triangle(renderer,interactor);
  
  renderer->ResetCamera();

  // renderWindow->Render();
}

void vtk_draw_view3(vtkSmartPointer<vtkRenderWindow> renderWindow,vtkSmartPointer<vtkRenderWindowInteractor> interactor)
{
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(renderer);
  
  renderer->SetBackground(0.1, 0.2, 0.4);

  renderer->SetViewport(xmins[2],ymins[2],xmaxs[2],ymaxs[2]);
  //if transformed vector size =0 ; will render the origin points
  m_source.m_display->draw_transformed_points(renderer);
  
  renderer->ResetCamera();

  // renderWindow->Render();
}
#include "vtkSphereSource.h"
void vtk_draw_view4(vtkSmartPointer<vtkRenderWindow> renderWindow,vtkSmartPointer<vtkRenderWindowInteractor> interactor)
{
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(renderer);
  
  renderer->SetBackground(0.1, 0.2, 0.4);

  renderer->SetViewport(xmins[3],ymins[3],xmaxs[3],ymaxs[3]);
  
  m_source.m_display->draw_skeleton(renderer,interactor);
  
  renderer->ResetCamera();

  // renderWindow->Render();
}

void vtk_draw_view5(vtkSmartPointer<vtkRenderWindow> renderWindow,vtkSmartPointer<vtkRenderWindowInteractor> interactor)
{
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(renderer);
  
  renderer->SetBackground(0.1, 0.2, 0.4);

  renderer->SetViewport(xmins[4],ymins[4],xmaxs[4],ymaxs[4]);
  
  m_source.m_display->draw_correspond(renderer);
  
  renderer->ResetCamera();

  // renderWindow->Render();
      // Create a sphere
}

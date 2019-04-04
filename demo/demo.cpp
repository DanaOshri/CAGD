#include <cagd.h>
#include <stdio.h>
#include "expr2tree.h"
#include "resource.h"

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#define D_CAGD

#define DENSITY 1000.

enum {
  MY_CLICK = CAGD_USER,
  MY_POLY,
  MY_ANIM,
  MY_DRAG,
  MY_DRAWCURVE,
  MY_ADD,
  MY_COLOR,
  MY_REMOVE,
};

char *animText[] = {
  "Animation Demo",
  "During the animation you can freely\n"
  "rotate, translate and scale the scene."
};

char *dragText[] = {
  "Drag, Popups & Dialog Demo",
  "Click right mouse button to call popup menu.\n"
  "A contents of the menu depends on where you\n"
  "clicked. Clicking on an existing point allows\n"
  "you to remove the point or change its color via\n"
  "dialog. If there is no point you could add one.\n\n"
  "Using left mouse button you can drag an existing\n"
  "points. Watch carefully a text appearing near\n"
  "the point being draged and don't miss."
};

char *clickText[] = {
  "Click Demo",
  "CAGD unable to convert 2D point defined by your\n"
  "click to single 3D point located in the object\n"
  "space. Instead it returns you two 3D points\n"
  "as specification of vector. Initially you are\n"
  "looking in the direction of this vector and cannot\n"
  "see anything. Try to rotate the scene after few\n"
  "clicks. You'll see polylines defined by the returned\n"
  "vectors."
};

char *polyText[] = {
  "Polyline Demo",
  "Click polyline. The nearest vertex will be marked with\n"
  "text. Remember that \"nearest\" defined by the minimal\n"
  "distance on the screen (window coordinates) and not\n"
  "in the object space."
};

HMENU myPopup;
UINT myText;
char myBuffer[BUFSIZ];

void myMessage(PSTR title, PSTR message, UINT type)
{
  MessageBox(cagdGetWindow(), message, title, MB_OK | MB_APPLMODAL | type);
}

void myTimer(int x, int y, PVOID userData)
{
  cagdRotate(2, 0, 1, 0);
  cagdRedraw();
}

LRESULT CALLBACK myDialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam)
{
  if(message != WM_COMMAND)
    return FALSE;
  switch(LOWORD(wParam)){
    case IDOK:
      GetDlgItemText(hDialog, IDC_EDIT, myBuffer, sizeof(myBuffer));
      EndDialog(hDialog, TRUE);
      return TRUE;
    case IDCANCEL:
      EndDialog(hDialog, FALSE);
      return TRUE;
    default:
      return FALSE;
  }
}

void myDragRightUp(int x, int y, PVOID userData)
{
  UINT id;
  CAGD_POINT p[2];
  int red, green, blue;
  HMENU hMenu = (HMENU)userData;
  cagdHideSegment(myText);
  for(cagdPick(x, y); id = cagdPickNext();)
    if(cagdGetSegmentType(id) == CAGD_SEGMENT_POINT)
      break;
  EnableMenuItem(hMenu, MY_ADD, id? MF_GRAYED: MF_ENABLED);
  EnableMenuItem(hMenu, MY_COLOR, id? MF_ENABLED: MF_GRAYED);
  EnableMenuItem(hMenu, MY_REMOVE, id? MF_ENABLED: MF_GRAYED);
  switch(cagdPostMenu(hMenu, x, y)){
    case MY_ADD:
      cagdToObject(x, y, p);
      p->z = 0;
      cagdAddPoint(p);
      break;
    case MY_COLOR:
      if(DialogBox(cagdGetModule(), 
             MAKEINTRESOURCE(IDD_COLOR),
             cagdGetWindow(),
             (DLGPROC)myDialogProc))
        if(sscanf(myBuffer, "%d %d %d", &red, &green, &blue) == 3)
          cagdSetSegmentColor(id, (BYTE)red, (BYTE)green, (BYTE)blue);
        else
          myMessage("Change color", "Bad color!", MB_ICONERROR);
      break;
    case MY_REMOVE:
      cagdFreeSegment(id);
      break;
  }
  cagdRedraw();
}

void myDragMove(int x, int y, PVOID userData)
{
  CAGD_POINT p[2];
  cagdToObject(x, y, p);
  p->z = 0;
  cagdReusePoint((UINT)userData, p);
  cagdReuseText(myText, p, " Leave me alone!");
  cagdRedraw();
}

void myDragLeftUp(int x, int y, PVOID userData)
{
  CAGD_POINT p;
  cagdGetSegmentLocation(myText, &p);
  cagdReuseText(myText, &p, " Ufff!");
  cagdRegisterCallback(CAGD_MOUSEMOVE, NULL, NULL);
  cagdRegisterCallback(CAGD_LBUTTONUP, NULL, NULL);
  cagdRedraw();
}

void myDragLeftDown(int x, int y, PVOID userData)
{
  UINT id;
  for(cagdPick(x, y); id = cagdPickNext();)
    if(cagdGetSegmentType(id) == CAGD_SEGMENT_POINT)
      break;
  if(id){
    CAGD_POINT p;
    BYTE red, green, blue;
    cagdGetSegmentLocation(id, &p);
    cagdReuseText(myText, &p, " Don't touch me!");
    cagdGetSegmentColor(id, &red, &green, &blue);
    cagdSetSegmentColor(myText, red, green, blue);
    cagdShowSegment(myText);
    cagdRegisterCallback(CAGD_MOUSEMOVE, myDragMove, (PVOID)id);
    cagdRegisterCallback(CAGD_LBUTTONUP, myDragLeftUp, NULL);
  } else
    myMessage("Ha-ha!", "You missed...", MB_ICONERROR);
  cagdRedraw();
}

void myClickLeftDown(int x, int y, PVOID userData)
{
  CAGD_POINT p[2];
  cagdToObject(x, y, p);
  cagdAddPolyline(p, sizeof(p) / sizeof(CAGD_POINT));
}

void myPolyLeftDown(int x, int y, PVOID userData)
{
  CAGD_POINT p;
  UINT id;
  int v;
  for(cagdPick(x, y); id = cagdPickNext();)
    if(cagdGetSegmentType(id) == CAGD_SEGMENT_POLYLINE)
      break;
  if(id){
    if(v = cagdGetNearestVertex(id, x, y)){
      cagdGetVertex(id, --v, &p);
      sprintf(myBuffer, " near #%d", v);
      cagdReuseText(myText, &p, myBuffer);
      cagdShowSegment(myText);
    }
  } else 
    myMessage("Ha-ha!", "You missed...", MB_ICONERROR);
  cagdRedraw();
}

auto draw_curve();
void my_curve_left_down(int x, int y, PVOID userData);
#define stastic static

class RND_Curve
{
public:
  stastic RND_Curve *get_instance(std::string path)
  {
    if (my_curve == NULL)
      my_curve = new RND_Curve(path);

    return my_curve;
  }

public:

private:
  RND_Curve(std::string path);
public:
  std::vector<CAGD_POINT *> curve;
  e2t_expr_node *treeX, *treeY, *treeZ;
  stastic RND_Curve *my_curve;
};

RND_Curve *RND_Curve::my_curve = NULL;

RND_Curve::RND_Curve(std::string path)
{
  std::vector<std::string> param_eqs(3);
  double tmin = 0., tmax = 0.;

  get_data(path, param_eqs, tmin, tmax);

  double x;
  char s[128];

  treeX = e2t_expr2tree(param_eqs[0].c_str());
  treeY = e2t_expr2tree(param_eqs[1].c_str());
  treeZ = e2t_expr2tree(param_eqs[2].c_str());
  std::vector<CAGD_POINT*> curve;
  std::vector<CAGD_POINT> loc_curve;

  if (treeX && treeY && treeZ)
  {

    curve.resize(DENSITY + 1);
    loc_curve.resize(DENSITY + 1);

    double alp = (tmax - tmin) / DENSITY;

    for (double stp = tmin, i = 0; stp <= tmax; stp += alp, ++i)
    {
      e2t_setparamvalue(stp, E2T_PARAM_T);

      double x = e2t_evaltree(treeX);
      double y = e2t_evaltree(treeY);
      double z = e2t_evaltree(treeZ);

      CAGD_POINT *point = new CAGD_POINT{ x, y, z };
      curve[( int )i] = point;
      loc_curve[( int )i] = *point;
    }

    cagdHideSegment(myText = cagdAddText(&loc_curve[0], ""));
    cagdAddPolyline(&loc_curve[0], loc_curve.size() - 1);
  }
  else
  {
    printf("Error loading tree");
  }
}

void myCommand(int id, int unUsed, PVOID userData)
{
  int i;
  CAGD_POINT p[] = {{1, 1, 1}, {1, -1, 1}, {-1, -1, 1}, {-1, 1, 1}, {1, 1, 1}};
  static int state = MY_CLICK;
  HMENU hMenu = (HMENU)userData;
  CheckMenuItem(hMenu, state, MF_UNCHECKED);
  CheckMenuItem(hMenu, state = id, MF_CHECKED);
  cagdFreeAllSegments();
  cagdReset();
  cagdSetView(CAGD_ORTHO);
  cagdSetDepthCue(FALSE);
  cagdSetColor(255, 255, 255);
  cagdRegisterCallback(CAGD_TIMER, NULL, NULL);
  cagdRegisterCallback(CAGD_LBUTTONDOWN, NULL, NULL);
  cagdRegisterCallback(CAGD_LBUTTONUP, NULL, NULL);
  cagdRegisterCallback(CAGD_RBUTTONUP, NULL, NULL);
  cagdRedraw();
  switch(id){
    case MY_ANIM:
      cagdSetView(CAGD_PERSP);
      cagdSetDepthCue(TRUE);
      cagdSetColor(0, 255, 255);
      cagdAddPolyline(p, sizeof(p) / sizeof(CAGD_POINT));
      p[0].z = p[1].z = p[2].z = p[3].z = p[4].z = -1;
      cagdAddPolyline(p, sizeof(p) / sizeof(CAGD_POINT));
      p[1].z = p[2].z = p[1].y = p[2].y = 1;
      cagdAddPolyline(p, sizeof(p) / sizeof(CAGD_POINT));
      p[0].y = p[1].y = p[2].y = p[3].y = p[4].y = -1;
      cagdAddPolyline(p, sizeof(p) / sizeof(CAGD_POINT));
      for(i = 0; i < 100; i++){
        p->x = (GLdouble)rand() / RAND_MAX * 2 - 1;
        p->y = (GLdouble)rand() / RAND_MAX * 2 - 1;
        p->z = (GLdouble)rand() / RAND_MAX * 2 - 1;
        cagdAddPoint(p);
      }
      SetWindowText(cagdGetWindow(), animText[0]);
      cagdSetHelpText(animText[1]);
      cagdShowHelp();
      cagdRegisterCallback(CAGD_TIMER, myTimer, NULL);
      break;
    case MY_DRAG:
      SetWindowText(cagdGetWindow(), dragText[0]);
      cagdSetHelpText(dragText[1]);
      cagdShowHelp();
      cagdHideSegment(myText = cagdAddText(p, ""));
      cagdRegisterCallback(CAGD_RBUTTONUP, myDragRightUp, ( PVOID )myPopup);
      cagdRegisterCallback(CAGD_LBUTTONDOWN, myDragLeftDown, NULL);
      break;
    case MY_DRAWCURVE:
      auto curve = draw_curve();
      cagdRegisterCallback(CAGD_LBUTTONDOWN, my_curve_left_down, (PVOID)curve);
      break;
    case MY_CLICK:
      cagdSetView(CAGD_PERSP);
      cagdSetDepthCue(TRUE);
      SetWindowText(cagdGetWindow(), clickText[0]);
      cagdSetHelpText(clickText[1]);
      cagdShowHelp();
      cagdRegisterCallback(CAGD_LBUTTONDOWN, myClickLeftDown, NULL);
      break;
    case MY_POLY:
      cagdScale(0.5, 0.5, 0.5);
      cagdRotate(45, 0, 0, 1);
      cagdAddPolyline(p, sizeof(p) / sizeof(CAGD_POINT) - 1);
      cagdHideSegment(myText = cagdAddText(p, ""));
      SetWindowText(cagdGetWindow(), polyText[0]);
      cagdSetHelpText(polyText[1]);
      cagdShowHelp();
      cagdRegisterCallback(CAGD_LBUTTONDOWN, myPolyLeftDown, NULL);
      break;
  }
  cagdRedraw();
}

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
      line.replace(line.find(','), 1, " ");
      std::istringstream params_stream(line);
      std::string str_prm;

      //std::getline(params_stream, str_prm, ',');
      params_stream >> tmin >> tmax;
    }
  }
}

auto draw_curve()
{
  cagdScale(0.5, 0.5, 0.5);
  cagdRotate(45, 0, 0, 1);
  std::cout << "FUCK YEAH!!!!!!" << std::endl;

  const std::string PATH = "allen.dat";
  std::vector<std::string> param_eqs(3);
  double tmin = 0., tmax = 0.;

  get_data(PATH, param_eqs, tmin, tmax);

  e2t_expr_node *treeX = NULL, *treeY = NULL, *treeZ = NULL;
  double x;
  char s[128];

  treeX = e2t_expr2tree(param_eqs[0].c_str());
  treeY = e2t_expr2tree(param_eqs[1].c_str());
  treeZ = e2t_expr2tree(param_eqs[2].c_str());
  std::vector<CAGD_POINT*> *curve = NULL;
  std::vector<CAGD_POINT> loc_curve;

  if (treeX && treeY && treeZ)
  {

    curve = new std::vector<CAGD_POINT*>(DENSITY+1);
    loc_curve.resize(DENSITY + 1);

    double alp = (tmax - tmin) / DENSITY;

    for (double stp = tmin, i = 0; stp <= tmax; stp += alp, ++i)
    {
      e2t_setparamvalue(stp, E2T_PARAM_T);

      double x = e2t_evaltree(treeX);
      double y = e2t_evaltree(treeY);
      double z = e2t_evaltree(treeZ);

      CAGD_POINT *point = new CAGD_POINT{ x, y, z };
      (*curve)[( int )i] = point;
      loc_curve[( int )i] = *point;
    }

    cagdHideSegment(myText = cagdAddText(&loc_curve[0], ""));
    cagdAddPolyline(&loc_curve[0], loc_curve.size() - 1);
  }
  else
  {
    printf("Error loading tree");
  }

  return curve;
}

void my_curve_left_down(int x, int y, PVOID curve)
{
  auto my_curve = (std::vector<CAGD_POINT*> *)curve;
  CAGD_POINT p;
  UINT id;
  int v;
  for (cagdPick(x, y); id = cagdPickNext();)
    if (cagdGetSegmentType(id) == CAGD_SEGMENT_POLYLINE)
      break;
  if (id)
  {
    if (v = cagdGetNearestVertex(id, x, y))
    {
      cagdGetVertex(id, --v, &p);
      sprintf(myBuffer, " near #%d", v);
      cagdReuseText(myText, &p, myBuffer);
      cagdShowSegment(myText);
    }
  }
  else
    myMessage("Ha-ha!", "You missed...", MB_ICONERROR);
  cagdRedraw();
}
#ifdef D_CAGD

int main(int argc, char *argv[])
{
  HMENU hMenu;
  cagdBegin("CAGD", 512, 512);
  hMenu = CreatePopupMenu();
  AppendMenu(hMenu, MF_STRING, MY_CLICK, "Click");
  AppendMenu(hMenu, MF_STRING, MY_POLY, "Polyline");
  AppendMenu(hMenu, MF_STRING, MY_ANIM, "Animation");
  AppendMenu(hMenu, MF_STRING, MY_DRAG, "Drag, Popup & Dialog");
  AppendMenu(hMenu, MF_STRING, MY_DRAWCURVE, "Draw Curve");
  cagdAppendMenu(hMenu, "Demos");
  myPopup = CreatePopupMenu();
  AppendMenu(myPopup, MF_STRING | MF_DISABLED, 0, "Point");
  AppendMenu(myPopup, MF_SEPARATOR, 0, NULL);
  AppendMenu(myPopup, MF_STRING, MY_ADD, "Add");
  AppendMenu(myPopup, MF_SEPARATOR, 0, NULL);
  AppendMenu(myPopup, MF_STRING, MY_COLOR, "Change color...");
  AppendMenu(myPopup, MF_STRING, MY_REMOVE, "Remove");
  cagdRegisterCallback(CAGD_MENU, myCommand, (PVOID)hMenu);
  cagdShowHelp();
  cagdMainLoop();
  return 0;
}

#else // D_CAGD



int main()
{
  const std::string PATH = "allen.dat";
  std::vector<std::string> param_eqs(3);
  double tmin = 0., tmax = 0.;

  get_data(PATH, param_eqs, tmin, tmax);

  e2t_expr_node *tree = NULL, *drv_tree = NULL;
  double x;
  char s[128];

  e2t_setparamvalue(0, E2T_PARAM_X);
  e2t_setparamvalue(0, E2T_PARAM_Y);
  e2t_setparamvalue(0, E2T_PARAM_Z);

  /*while (1) {
    printf("Enter func:");
    gets(s);
    tree = e2t_expr2tree(s);
    if (!tree) {
      printf("Error %d\n", e2t_parsing_error);
      continue;
    }
    printf("The tree is:");
    e2t_printtree(tree, ( char * )NULL);

    double drv_val = 0.;
    printf("\nEnter x value:"); gets(s);
    sscanf(s, "%lf", &x);
    printf("\n\nDtree/Dx at %f is:", x);
    drv_tree = e2t_derivtree(tree, E2T_PARAM_X);
    e2t_setparamvalue(x, E2T_PARAM_X);
    e2t_printtree(drv_tree, ( char * )NULL);
    printf("   == %lf\n\n", e2t_evaltree(drv_tree));

    e2t_setparamvalue(x, E2T_PARAM_X);
    printf("Tree value for x = %lf is %lf\n", x, e2t_evaltree(tree));
  }*/
}

#endif // D_CAGD
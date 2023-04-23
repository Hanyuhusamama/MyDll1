#include<iostream>
#include"..\MyDll1\my_mouse_event.h"
#include"..\MyDll1\my_windows_operation.h"

using namespace std;

int main()
{
    //MouseEvent Emove_test(1000, 1000, MouseEvent::MouseMove);
    //Emove_test._Send_input();
    MouseStimulator si;
    MyVec2i _vex = si._GetResolution();
    MouseEvent Emove_test(_vex.x, _vex.y, MouseEvent::MouseMove);
    CapatureWindow(GetDesktopWindow(), L"testi.png");
    return 0;
}

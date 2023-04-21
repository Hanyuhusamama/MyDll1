#include<iostream>
#include"..\MyDll1\my_mouse_event.h"

using namespace std;

int main()
{
    MouseEvent Emove_test(1000, 1000, MouseEvent::MouseMove);
    Emove_test._Send_input();
    return 0;
}

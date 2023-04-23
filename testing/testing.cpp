#include<iostream>
#include"..\MyDll1\my_mouse_event.h"
#include"..\MyDll1\my_windows_operation.h"
#include"..\MyDll1\my_file_operatoin.h"

using namespace std;

template<size_t N>
void ArrayIniti(char (&arr)[N],
    char val) {
    for (size_t i = 0; i < N; ++i) {
        arr[i] = val;
    }
}

int main()
{
    //MouseEvent Emove_test(1000, 1000, MouseEvent::MouseMove);
    //Emove_test._Send_input();
    MouseStimulator si;
    MyVec2i _vex = si._GetResolution();
    MouseEvent Emove_test(_vex.x, _vex.y, MouseEvent::MouseMove);
    CapatureWindow(GetDesktopWindow(), L"testi.png");
    char buf[16];
    ArrayIniti(buf, 'f');
    cout << buf;
    MyFile file;
    file.open("C:\\Users\\15693\\Desktop\\test.txt", MyFile::FILE_WRITE);
    return 0;
}

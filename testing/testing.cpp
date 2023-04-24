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
    MyFile file;
    file.open("C:\\Users\\15693\\Desktop\\test.txt", MyFile::FILE_WRITE);
    char buffer[64];
    file >> buffer;
    int _var;
    file >> _var;
    cout << buffer << _var;
    file.close();
    return 0;
}

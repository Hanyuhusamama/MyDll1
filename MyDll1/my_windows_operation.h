#ifndef _MY_WINDOWS_OPERATION_H_
#define _MY_WINDOWS_OPERATION_H_

#include<windows.h>

#ifndef MY_API
#ifdef _USRDLL
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif 
#endif // !MY_API

MY_API wchar_t* my_str_to_w_str(const char* _str);

MY_API int CapatureWindow(HWND hWin, const wchar_t* _store_path);



#endif
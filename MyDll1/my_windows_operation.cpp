#include "pch.h"
#include "my_windows_operation.h"
#include<atlimage.h>	

MY_API wchar_t* my_str_to_w_str(const char* _str)
{
    unsigned len;
    len = MultiByteToWideChar(CP_ACP, 0, _str, strlen(_str), NULL, 0);
    wchar_t* _w_char_ret = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, _str, strlen(_str), _w_char_ret, len);
    _w_char_ret[len] = 0;
    return _w_char_ret;
}

MY_API int CapatureWindow(HWND hWin, const wchar_t* _name)
{
    if (hWin == NULL)return -1;
    HDC hdc = GetDC(hWin);
    int pixs = GetDeviceCaps(hdc, BITSPIXEL);
    int Width = GetDeviceCaps(hdc, HORZRES);
    int Height = GetDeviceCaps(hdc, VERTRES);
    UINT dpi = GetDpiForWindow(hWin);
    float Scale = 1.0f;
    switch (dpi)
    {
    case 120:
        Scale = 1.25f;
        break;
    case 144:
        Scale = 1.5f;
        break;
    case 192:
        Scale = 2.0f;
        break;
    case 216:
        Scale = 1.25;
        break;
    default:
        break;
    }
    Width = (float)Width * Scale;
    Height = (float)Height * Scale;
    CImage image;
    image.Create(Width, Height, pixs);
    BitBlt(image.GetDC(), 0, 0, Width, Height, hdc, 0, 0, SRCCOPY);
    image.Save(_name, Gdiplus::ImageFormatPNG);
    image.ReleaseDC();
    ReleaseDC(hWin, hdc);
    return 0;
}

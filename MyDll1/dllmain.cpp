// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

void _my_table_initialize();
void _number_fsm_init();
void _number_fsm_delete();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        _my_table_initialize();
        _number_fsm_init();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        void _number_fsm_delete();
        break;
    }
    return TRUE;
}


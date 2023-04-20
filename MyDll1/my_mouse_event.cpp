#include "pch.h"
#include "my_mouse_event.h"

void MouseEvent::_Send_input()
{
	INPUT input;
	ZeroMemory(&input, sizeof(INPUT));
	input.type = INPUT_MOUSE;
	input.mi.dx = (LONG)dx;
	input.mi.dy = (LONG)dy;
	switch (EventType)
	{
	case MouseEvent::MouseMove:
		input.mi.dwFlags = MOUSEEVENTF_MOVE;
		break;
	case MouseEvent::MouseDown:
		input.mi.dwFlags = (EventLeft) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
		break;
	case MouseEvent::MouseUp:
		input.mi.dwFlags = (EventLeft) ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
		break;
	case MouseEvent::MouseMoveAbsolute:
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		break;
	}
	SendInput(1, &input, sizeof(INPUT));
}

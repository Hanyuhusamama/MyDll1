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
		input.mi.dwFlags = (ExtraInfo.EventLeft) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
		break;
	case MouseEvent::MouseUp:
		input.mi.dwFlags = (ExtraInfo.EventLeft) ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
		break;
	case MouseEvent::MouseMoveAbsolute:
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		break;
	case MouseEvent::MouseEventWheel:
		input.mi.dwFlags = MOUSEEVENTF_WHEEL;
		input.mi.mouseData = ExtraInfo.WhellMove;
		break;
	}
	SendInput(1, &input, sizeof(INPUT));
}

MouseEvent::MouseEvent(int dX, int dY, MouseEvent::MouseEventType _type, bool EventLeftKey)
{
	dx = dX;
	dy = dY;
	EventType = _type;
	ExtraInfo.EventLeft = EventLeftKey;
}

MouseEvent::MouseEvent()
{
	dx = dy = 0;
	ExtraInfo.WhellMove = 0;
	EventType = MouseMove;
}

MouseEvent::MouseEvent(const MouseEvent&b)
{
	dx = b.dx;
	dy = b.dy;
	EventType = b.EventType;
	ExtraInfo = b.ExtraInfo;
}

void MouseEvent::SetEventType(MouseEvent::MouseEventType _type, bool EventLeftKey)
{
	EventType = _type;
	ExtraInfo.EventLeft = EventLeftKey;
}

void MouseEvent::SetEventCoord(int dX, int dY)
{
	dx = dX;
	dy = dY;
}

void MouseEvent::SetWheelEvent(int wheel_move)
{
	ExtraInfo.WhellMove = wheel_move;
	EventType = MouseEventWheel;
}

MouseStimulator::MouseStimulator()
{
	_event_initialize = MouseEvent(0, 0, MouseEvent::MouseMoveAbsolute);
	CurX = 0;
	CurY = 0;
	Scale = 1.0;
}

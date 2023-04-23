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
	this->Res_x = this->Res_y = 0;
	_event_initialize._Send_input();
}

void MouseStimulator::LeftClick(bool _double_click)
{
	MouseEvent _eventd(0, 0, MouseEvent::MouseDown);
	_eventd._Send_input();
	MouseEvent _eventu(0, 0, MouseEvent::MouseUp);
	_eventu._Send_input();
	if (_double_click) {
		_eventd._Send_input();
		_eventu._Send_input();
	}
}

void MouseStimulator::RightClick()
{
	MouseEvent _eventd(0, 0, MouseEvent::MouseDown,false);
	_eventd._Send_input();
	MouseEvent _eventu(0, 0, MouseEvent::MouseUp,false);
	_eventu._Send_input();
}

void MouseStimulator::MoveMouse(MyVec2i move_vector)
{
	MouseEvent event((float)move_vector.x*Scale, 
		(float)move_vector.y*Scale,
		MouseEvent::MouseMove);
	event._Send_input();
}

MyVec2i MouseStimulator::_GetResolution()
{
	RECT rect{ 0 };
	SystemParametersInfo(SPI_GETWORKAREA, NULL,
		&rect, 0);
	std::cout << rect.left<<" " << rect.right << std::endl;
	Res_x = rect.right - rect.left;
	Res_y = rect.bottom - rect.top;
	return MyVec2i(Res_x, Res_y);
}

void MouseStimulator::SetMousePosition(float _x_Ratio, float _y_Ratio)
{
	MouseEvent Events((float)Res_x * _x_Ratio,
		(float)Res_y * _y_Ratio, MouseEvent::MouseMoveAbsolute);
	Events._Send_input();
}

MyVec2i::MyVec2i(const MyVec2i& b)
{
	x = b.x;
	y = b.y;
}

MyVec2i& MyVec2i::_add(const MyVec2i& b)
{
	x += b.x;
	y += b.y;
	return *this;
}

MyVec2i& MyVec2i::_sub(const MyVec2i& b)
{
	x -= b.x;
	y -= b.y;
	return *this;
}

MyVec2i& MyVec2i::operator+=(const MyVec2i& b)
{
	x += b.x;
	y += b.y;
	return *this;
}

MyVec2i& MyVec2i::operator-=(const MyVec2i& b)
{
	x -= b.x;
	y -= b.y;
	return *this;
}

MyVec2i MyVec2i::operator+(const MyVec2i& b)const
{
	MyVec2i ret(*this);
	return ret._add(b);
}

MyVec2i MyVec2i::operator-(const MyVec2i& b)const
{
	MyVec2i ret(*this);
	return ret._sub(b);
}

MyVec2f::MyVec2f(const MyVec2f& b)
{
	x = b.x;
	y = b.y;
}

MyVec2f& MyVec2f::_add(const MyVec2f&b)
{
	x += b.x;
	y += b.y;
	return *this;
}

MyVec2f& MyVec2f::_sub(const MyVec2f& b)
{
	x -= b.x;
	y -= b.y;
	return *this;
}

MyVec2f& MyVec2f::operator+=(const MyVec2f& b)
{
	x += b.x;
	y += b.y;
	return *this;
}

MyVec2f& MyVec2f::operator-=(const MyVec2f& b)
{
	x -= b.x;
	y -= b.y;
	return *this;
}

MyVec2f MyVec2f::operator+(const MyVec2f& b) const
{
	return MyVec2f(x+b.x,y+b.y);
}

MyVec2f MyVec2f::operator-(const MyVec2f& b) const
{
	return MyVec2f(x-b.x,y-b.y);
}

MY_API std::istream& operator>>(std::istream& _istream, MyVec2i&vec)
{
	_istream >> vec.x >> vec.y;
	return _istream;
}

MY_API std::ostream& operator<<(std::ostream& _ostream, const MyVec2i& vec)
{
	_ostream << "x: " << vec.x << " y:" << vec.y << std::endl;
	return _ostream;
}

MY_API std::ostream& operator<<(std::ostream& _ostr, const MyVec2f& vec)
{
	_ostr <<"x: " << vec.x << " y:" << vec.y << std::endl;
	return _ostr;
}

MY_API std::istream& operator>>(std::istream& _istr, MyVec2f&vec)
{
	_istr >> vec.x >> vec.y;
	return _istr;
}

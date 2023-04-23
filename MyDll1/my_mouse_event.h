#ifndef _MY_MOUSE_EVENT_H_
#define _MY_MOUSE_EVENT_H_

#ifndef MY_API
#ifdef _USRDLL
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif 
#endif // !MY_API

#include<iostream>

class MY_API MouseEvent {
private:
	int dx;
	int dy;
public:
	enum MouseEventType
	{
		MouseMove,
		MouseDown,
		MouseUp,
		MouseMoveAbsolute,
		MouseEventWheel
	};
	void _Send_input();
private:
	MouseEventType EventType;
	union {
		bool EventLeft;
		int WhellMove;
	}ExtraInfo;
public:
	MouseEvent(int dX, int dY, MouseEvent::MouseEventType _type, bool EventLeftKey = true);
	MouseEvent();
	MouseEvent(const MouseEvent&);
	void SetEventType(MouseEvent::MouseEventType _type, bool EventLeftKey = true);
	void SetEventCoord(int dX, int dY);
	void SetWheelEvent(int wheel_move);
};

class MyVec2i {
public:
	int x;
	int y;
	constexpr MyVec2i():x(0),y(0){}
	constexpr MyVec2i(int _x,int _y):x(_x),y(_y){}
	MyVec2i(const MyVec2i& b);
	~MyVec2i(){}
	MyVec2i& _add(const MyVec2i& b);
	MyVec2i& _sub(const MyVec2i& b);
	MyVec2i& operator +=(const MyVec2i& b);
	MyVec2i& operator -=(const MyVec2i& b);
	MyVec2i operator+(const MyVec2i& b)const;
	MyVec2i operator-(const MyVec2i& b)const;
};

MY_API std::istream& operator>>(std::istream&_istream, MyVec2i&);

MY_API std::ostream& operator<<(std::ostream& _ostream, const MyVec2i& vec);

class MyVec2f {
public:
	float x;
	float y;
	constexpr MyVec2f():x(0.0f),y(0.0f){}
	constexpr MyVec2f(float _x, float _y) : x(_x), y(_y) {}
	MyVec2f(const MyVec2f& b);
	MyVec2f& _add(const MyVec2f&b);
	MyVec2f& _sub(const MyVec2f& b);
	MyVec2f& operator+=(const MyVec2f& b);
	MyVec2f& operator -=(const MyVec2f& b);
	MyVec2f operator+ (const MyVec2f& b)const;
	MyVec2f operator-(const MyVec2f& b)const;
};

MY_API std::ostream& operator<<(std::ostream& _ostr, const MyVec2f& vec);

MY_API std::istream& operator>>(std::istream& _istr, MyVec2f&);

class MY_API MouseStimulator {
private:
	int CurX;
	int CurY;
	MouseEvent _event_initialize;
	float Scale;
	int Res_x;
	int Res_y;
public:
	MouseStimulator();
	void LeftClick(bool _double_click=false);
	void RightClick();
	void MoveMouse(MyVec2i move_vector);
	MyVec2i _GetResolution();
	void SetMousePosition(float _x_Ratio, float _y_Ratio);
};

#endif // !_MY_MOUSE_EVENT_H_

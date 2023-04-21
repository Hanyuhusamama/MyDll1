#ifndef _MY_MOUSE_EVENT_H_
#define _MY_MOUSE_EVENT_H_

#ifdef _USRDLL
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif 


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

class MY_API MouseStimulator {
private:
	int CurX;
	int CurY;
	MouseEvent _event_initialize;
	float Scale;
public:
	MouseStimulator();
	void LeftClick(bool _double_click=false);
	void RightClick();
	void MoveMouse();

};

#endif // !_MY_MOUSE_EVENT_H_

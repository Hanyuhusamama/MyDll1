#ifndef _MY_MOUSE_EVENT_H_
#define _MY_MOUSE_EVENT_H_

class MouseEvent {
private:
	int dx;
	int dy;
public:
	enum MouseEventType
	{
		MouseMove,
		MouseDown,
		MouseUp,
		MouseMoveAbsolute
	};
	void _Send_input();
private:
	MouseEventType EventType;
	bool EventLeft;
public:
	MouseEvent(int dX, int dY, MouseEvent::MouseEventType _type, bool EventLeftKey = true);
};

#endif // !_MY_MOUSE_EVENT_H_

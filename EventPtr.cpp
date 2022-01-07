#include "EventPtr.h"

EventPtr::EventPtr()
{
}

EventPtr::EventPtr(int _t, int _id, double _vx, double _vy, double _x, double _y)
{
	t = _t;
	id = _id;
	vx = _vx;
	vy = _vy;
	x = _x;
	y = _y;
}
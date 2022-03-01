#include "EventPtr.h"
#include "Util.h"

Vessel::Vessel(int _id, double _x, double _y, double _vx, double _vy, double _r) {
	id = _id;
	loc = Point(_x, _y);
	vx = _vx;
	vy = _vy;
	r = _r;
	filterRad = r + (Util::interval * sqrt(pow(vx, 2) + pow(vy, 2)));
	queryRad = r + (Util::smInterval * sqrt(pow(vx, 2) + pow(vy, 2)));
}

void Vessel::predictLoc(int futureT) {
	Point newP = Point((double)loc.x + futureT * vx, (double)loc.y + futureT * vy);
	loc = newP;
}

EventPtr::EventPtr()
{
}

EventPtr::EventPtr(int _t, int _id, double _vx, double _vy, double _x, double _y, double _r)
{
	t = _t;
	id = _id;
	vx = _vx;
	vy = _vy;
	loc = Point(_x, _y);
	extLoc = nullptr;
	r = _r;
}

void EventPtr::predictLoc(int futureT) {
	extLoc = new Point(loc.x + ((double)futureT * vx), loc.y + ((double)futureT * vy));
	if (loc.x < extLoc->x) {
		left = loc;
		right = *extLoc;
	}
	else {
		left = *extLoc;
		right = loc;
	}
}
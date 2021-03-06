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

Event::Event()
{
}

Event::Event(int _t, int _id, double _vx, double _vy, double _x, double _y)
{
	t = _t;
	id = _id;
	vx = _vx;
	vy = _vy;
	loc = Point(_x, _y);
	extLoc = Point(0, 0);
}

void Event::predictLoc(int futureT) {
	extLoc = Point(loc.x + (futureT * vx), loc.y + (futureT * vy));
	hasPredicted = true;
}
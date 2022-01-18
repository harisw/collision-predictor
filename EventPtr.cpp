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
Vessel* Vessel::predictLoc(int futureT) {
	return new Vessel(id, (loc.x + (futureT * vx)), (loc.y + (futureT * vy)),
		vx, vy, r);
}

Event::Event()
{
}

Event::Event(int _t, int _id, double _vx, double _vy, double _x, double _y, double _r)
{
	t = _t;
	id = _id;
	vx = _vx;
	vy = _vy;
	loc = Point(_x, _y);
	r = _r;
}

void Event::predictLoc(int futureT) {
	extLoc = Point(loc.x + (futureT * vx), loc.y + (futureT * vy));
	hasPredicted = true;
}
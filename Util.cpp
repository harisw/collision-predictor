#include "Util.h"
#include "EventPtr.h"

double Util::distance(Point p, Point q) {
	return sqrt(pow(q.x - p.x, 2) + pow(q.y - p.y, 2));
}
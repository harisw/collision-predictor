#include "Util.h"
#include "EventPtr.h"
#include<cmath>
#include<utility>
using namespace std;
int Util::interval;
int Util::smInterval;

double Util::distance(Point p, Point q) {
	return sqrt(pow(q.x - p.x, 2) + pow(q.y - p.y, 2));
}

double Util::lineToPointDistance(Point a, Point b, Point c)
{
	double x = c.x;
	double y = c.y;
	if ((a.x - b.x) * (x - b.x) + (a.y - b.y) * (y - b.y) <= 0)
		return sqrt((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y));

	if ((b.x - a.x) * (x - a.x) + (b.y - a.y) * (y - a.y) <= 0)
		return sqrt((x - a.x) * (x - a.x) + (y - a.y) * (y - a.y));

	return abs((b.y - a.y) * x - (b.x - a.x) * y + b.x * a.y - b.y * a.x) /
		sqrt((a.y - b.y) * (a.y - b.y) + (a.x - b.x) * (a.x - b.x));
}

double Util::lineToLineDistance(Point a1, Point a2, Point b1, Point b2)
{
	double result = 99999999;
	double distA, distB;
	distA = min(lineToPointDistance(b1, b2, a1), lineToPointDistance(b1, b2, a2));
	distB = min(lineToPointDistance(a1, a2, b1), lineToPointDistance(a1, a2, b2));

	return min(distA, distB);
}
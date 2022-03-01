#pragma once
#include <math.h>

class Point
{
public:
	double x;
	double y;
	Point() {
	};
	Point(double _x, double _y) {
		x = _x;
		y = _y;
	}
}; 

class Vessel
{
public:
	int id;
	Point loc;
	double vx;
	double vy;
	double r;
	double filterRad;
	double queryRad;

	Vessel(int _id, double _x, double _y, double _vx, double _vy, double _r);
	void predictLoc(int futureT);
};

class EventPtr
{
public:
	int t;
	int id;
	double vx;
	double vy;
	double r;
	Point loc;
	Point* extLoc;
	Point left;
	Point right;
	bool filtered = false;
	bool isCandidate = false;
	bool isCollide = false;
	EventPtr();
	EventPtr(int _t, int _id, double _vx, double _vy, double _x, double _y, double _r = 0);
	void predictLoc(int futureT);
};
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
	Vessel(int _id, double _x, double _y, double _vx, double _vy, double _r);
	Vessel* predictLoc(int futureT);
};

class Event
{
public:
	int t;
	int id;
	double vx;
	double vy;
	Point loc;
	Event();
	Event(int _t, int _id, double _vx, double _vy, double _x, double _y);
	Event* predictLoc(int futureT);
};
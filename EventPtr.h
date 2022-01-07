#pragma once
class EventPtr
{
public:
	int t;
	int id;
	double vx;
	double vy;
	double x;
	double y;
	EventPtr();
	EventPtr(int _t, int _id, double _vx, double _vy, double _x, double _y);
};
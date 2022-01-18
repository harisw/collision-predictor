#pragma once
#include<utility>
using namespace std;
class Point;
class Util
{
public:
	static int interval;
	static int smInterval;
	static double distance(Point p, Point q);
	static double lineToPointDistance(Point a, Point b, Point circle);
	static double lineToLineDistance(pair<Point, Point> a, pair<Point, Point> b);
	//static void refineAISData();
};
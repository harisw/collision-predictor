#pragma once
class Point;
class Util
{
public:
	static int interval;
	static int smInterval;
	static double distance(Point p, Point q);
	static double lineToPointDistance(Point a, Point b, Point circle);
	static double lineToLineDistance(Point a1, Point a2, Point b1, Point b2);
	//static double lineToLineDistance(pair<Point, Point> a, pair<Point, Point> b);
	//static void refineAISData();
};
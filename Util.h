#pragma once
class Point;
class Util
{
public:
	static double distance(Point p, Point q);
	static double lineToPointDistance(Point a, Point b, Point circle);
	static void refineAISData();
};
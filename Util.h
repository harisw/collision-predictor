#pragma once
#include <vector>
#include <string>

using namespace std;
class Point;
class EventPtr;
class Vessel;
class Util
{
public:
	static int interval;
	static int smInterval;
	static int maxT;
	static string vesselFilename;
	static string objFilename;
	static double distance(Point p, Point q);
	static double lineToPointDistance(Point a, Point b, Point circle);
	static double lineToLineDistance(Point a1, Point a2, Point b1, Point b2);
	static void importVesselData(vector<Vessel*>& input, vector<Vessel*>& input2, 
		int& vesselNum);
	static void importObjData(vector<vector<EventPtr*>>& input, int& objNum);
	static void exportResult(vector< vector<pair<int,int> > >& naiveResult, vector<vector<pair<int, int> > >& TPRResult,
		vector<vector<pair<int, int> > >& hybridResult);
	//static double lineToLineDistance(pair<Point, Point> a, pair<Point, Point> b);
	//static void refineAISData();
};
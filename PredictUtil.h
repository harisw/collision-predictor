#pragma once
#include<vector>
#include<set>
using namespace std;

class Vessel;
class Event;
class TPRTree;
class PredictUtil
{
public:
	static vector<Vessel*> predictBufferZones(vector<Vessel*>& inputVessel);
	static void trajectoryFilter(set<int>& inputIDs, vector<Vessel*>& inputVessel, vector<Event*>& inputObj, int _time);
};
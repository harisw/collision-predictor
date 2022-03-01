#pragma once
#include<vector>
#include<set>
using namespace std;

class Vessel;
class EventPtr;
class TPRTree;
class PredictUtil
{
public:
	static vector<Vessel*> predictBufferZones(vector<Vessel*>& inputVessel);
	//static void trajectoryFilter(vector<Event*>& inputObj, TPRTree& indexTree);
	static void trajectoryFilter(set<int>& inputIDs, vector<EventPtr*>& inputObj, int _time);
};
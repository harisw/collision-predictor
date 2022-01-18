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
	static void trajectoryFilter(vector<Event*>& inputObj, TPRTree& indexTree);
};
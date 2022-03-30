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
	static void trajectoryFilter(set<int>& inputIDs, vector<EventPtr*>& inputObj);
};
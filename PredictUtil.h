#pragma once
#include<vector>
using namespace std;

class Vessel;
class Event;
class PredictUtil
{
public:
	static vector<Vessel*> predictBufferZones(vector<Vessel*>& inputVessel);
	static vector<Vessel*> predictMBRs(vector<Vessel*> inputVessel);
	static vector<int> trajectoryFilter(vector<Vessel*>& inputVessel, vector<Event*>& inputObj);
};
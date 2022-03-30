#include <math.h>
#include <algorithm>
#include <chrono>
#include <unordered_map>
using namespace std;
using namespace std::chrono; 
#include "PredictUtil.h"
#include "EventPtr.h"
#include "Util.h"
#include "TPRTree.h"

void PredictUtil::trajectoryFilter(set<int>& inputIDs, vector<EventPtr*>& inputObj)
{
	for (int i = 0; i < inputObj.size()-1; i++) {
		if (inputObj[i]->isCandidate)
			continue;
	
		if(inputObj[i]->extLoc == nullptr)
			inputObj[i]->predictLoc(Util::interval);

		for (int j = i+1; j < inputObj.size(); j++) {
			
			if(inputObj[j]->extLoc == nullptr)
				inputObj[j]->predictLoc(Util::interval);

			double dist = Util::lineToLineDistance(inputObj[i]->loc, *inputObj[i]->extLoc, inputObj[j]->loc, *inputObj[j]->extLoc);

			double maxRadius = max(inputObj[i]->r, inputObj[j]->r);
			if (dist <= maxRadius) {
				inputIDs.insert(inputObj[i]->id);
				inputIDs.insert(inputObj[j]->id);
				inputObj[j]->isCandidate = true;
				break;
			}
		}
	}
	return;
}
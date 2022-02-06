#include <math.h>
#include <algorithm>
#include "PredictUtil.h"
#include "EventPtr.h"
#include "Util.h"
#include "TPRTree.h"
#define PI 3.1416

//bool onSegment(Point p, Point q, Point r)
//{
//	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
//		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
//		return true;
//
//	return false;
//}
//
//int orientation(Point p, Point q, Point r)
//{
//	int val = (q.y - p.y) * (r.x - q.x) -
//		(q.x - p.x) * (r.y - q.y);
//
//	if (val == 0) return 0;  // collinear
//
//	return (val > 0) ? 1 : 2; // clock or counterclock wise
//}

void cleanVect(vector<Vessel*>& input) {
	int j = 0;
	vector<Vessel*> res;
	while (j < input.size()) {
		if (input[j] != NULL)
			res.push_back(input[j]);
		j++;
	}
	input = res;
}

void PredictUtil::trajectoryFilter(set<int>& inputIDs, vector<Event*>& inputObj, int currTime)
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
			//double dist = Util::lineToPointDistance(a, b, inputVessel[j]->loc);

			double stretchedBufferRadius = max(inputObj[i]->r, inputObj[j]->r) * Util::interval;
			if (dist <= stretchedBufferRadius) {
				inputIDs.insert(inputObj[i]->id);
				inputIDs.insert(inputObj[j]->id);
				inputObj[i]->isCandidate = true;
				inputObj[j]->isCandidate = true;
				break;
			}
		}
	}
	return;
}
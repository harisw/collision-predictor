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

void PredictUtil::trajectoryFilter(set<int>& inputIDs, vector<Vessel*>& inputVessel, vector<Event*>& inputObj, int currTime, TPRTree* const &vesselTree)
{
	vector<bool> vesselCandidates;
	vesselCandidates.insert(vesselCandidates.end(), inputVessel.size(), false);

	for (int i = 0; i < inputObj.size(); i++) {
		inputObj[i]->predictLoc(Util::interval);

		Point a = inputObj[i]->loc;
		Point b = inputObj[i]->extLoc;
		for (int j = 0; j < inputVessel.size(); j++) {
			double dist = Util::lineToPointDistance(a, b, inputVessel[j]->loc);
			double stretchedBufferRadius = inputVessel[j]->filterRad;
			if (dist <= stretchedBufferRadius) {
				inputIDs.insert(inputObj[i]->id);
				vesselCandidates[j] = true;
				break;
			}
		}
	}

	if (vesselTree != 0) {
		for (int j = 0; j < vesselCandidates.size(); j++) {
			if (vesselCandidates[j])
				vesselTree->Insert(CEntry(inputVessel[j]->id, 0, inputVessel[j]->loc.x, inputVessel[j]->loc.y, 0.0, inputVessel[j]->vx, inputVessel[j]->vy, 0.0, inputVessel[j]->r));
		}
	}
}
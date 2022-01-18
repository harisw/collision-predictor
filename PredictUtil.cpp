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

void PredictUtil::trajectoryFilter(vector<Event*>& inputObj, TPRTree& indexTree)
{
	//vector<Event*> predictedObj;
	set<int> candidateID;

	for (int i = 0; i < inputObj.size(); i++) {
		/*Event* futureEv = inputObj[i]->predictLoc(Util::interval);
		predictedObj.push_back(futureEv);*/
		Event* currentEv = inputObj[i];
		if (!currentEv->hasPredicted)
			currentEv->predictLoc(Util::interval);
		
		if (currentEv->filtered)
			continue;
		for(int j=i+1; j < inputObj.size(); j++) {
			if (!inputObj[j]->hasPredicted)
				inputObj[j]->predictLoc(Util::interval);


			if (inputObj[j]->filtered)
				continue;
			double dist = Util::lineToLineDistance(currentEv->loc, currentEv->extLoc, inputObj[j]->loc, inputObj[j]->extLoc);
			if (dist <= max(currentEv->r, inputObj[j]->r) ) {
				indexTree.Insert(CEntry(currentEv->id, 0.0, currentEv->loc.x, currentEv->loc.y, 0.0,
					currentEv->vx, currentEv->vy, 0.0, currentEv->r));

				indexTree.Insert(CEntry(inputObj[j]->id, 0.0, inputObj[j]->loc.x, inputObj[j]->loc.y, 0.0,
					inputObj[j]->vx, inputObj[j]->vy, 0.0, inputObj[j]->r));

				inputObj[i]->filtered = true;
				inputObj[j]->filtered = true;
				break;
			}
		}
	}
}
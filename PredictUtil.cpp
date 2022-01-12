#include <math.h>
#include <algorithm>
#include "PredictUtil.h"
#include "EventPtr.h"
#include "Util.h"
#define PI 3.1416

bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}

int orientation(Point p, Point q, Point r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // collinear

	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

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

vector<int> PredictUtil::trajectoryFilter(vector<Vessel*>& inputVessel, vector<Event*>& inputObj)
{
	vector<Event*> predictedObj;
	vector<int> candidateID;


	for (int i = 0; i < inputObj.size(); i++) {
		Event* futureEv = inputObj[i]->predictLoc(Util::interval);
		predictedObj.push_back(futureEv);
			
		Point a = inputObj[i]->loc;
		Point b = predictedObj[i]->loc;

		for (int j = 0; j < inputVessel.size(); j++) {
			
			double dist = Util::lineToPointDistance(a, b, inputVessel[j]->loc);
			double stretchedBufferRadius = inputVessel[j]->filterRad;
			if (dist <= stretchedBufferRadius) {
				candidateID.push_back(inputObj[i]->id);
				break;
			}
		}
	}
	return candidateID;
}
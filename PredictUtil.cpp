#include <math.h>
#include <algorithm>
#include "PredictUtil.h"
#include "EventPtr.h"
#include "Util.h"
#define PI 3.1416
#define I 10

//vector<Vessel*> PredictUtil::predictBufferZones(vector<Vessel*>& inputVessel)
//{
//	vector<Vessel*> predicted;
//	for (int j = 0; j < inputVessel.size(); j++) {
//		Vessel* futureVes = inputVessel[j]->predictLoc(I);
//	}
//	int j = 0;
//	int size = inputVessel.size();
//	while (j < size) {
//		bool noMerge = true;
//		int i = j + 1;
//		while (i < size) {
//
//			//get the largest area whether future or now
//			double distNow = distance(inputVessel[j], inputVessel[i]);
//			double distFuture = distance(predicted[j], predicted[i]);
//		}
//	}
//	return vector<Vessel>();
//}

//vector<Vessel*> PredictUtil::predictTrajectory(vector<Vessel*>& inputVessel)
//{
//	vector<Vessel*> predicted;
//	for (int j = 0; j < inputVessel.size(); j++) {
//		Vessel* futureVes = inputVessel[j]->predictLoc(I);
//		predicted.push_back(futureVes);
//	}
//	return predicted;
//}

bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}

int orientation(Point p, Point q, Point r)
{
	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
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
vector<Vessel*> PredictUtil::predictMBRs(vector<Vessel*> inputVessel)
{
	int j = 0;
	vector<Vessel*> vesselMBRs;
	while(j < inputVessel.size()) {
		int k = j + 1;
		while (k < inputVessel.size()) {
			double maxR = max(inputVessel[j]->r, inputVessel[k]->r);
			if (Util::distance(inputVessel[j]->loc, inputVessel[k]->loc) <= (I * maxR)) {
				Vessel* newVes = new Vessel(0, (inputVessel[j]->loc.x + inputVessel[k]->loc.x) / 2,
					(inputVessel[j]->loc.y + inputVessel[k]->loc.y) / 2, max(abs(inputVessel[j]->vx), abs(inputVessel[k]->vx)),
					max(abs(inputVessel[j]->vy), abs(inputVessel[k]->vy)), maxR);
				inputVessel[j] = newVes;
				inputVessel[k] = nullptr;
				cleanVect(inputVessel);
				break;
			}
			else
				k++;
		}
		if (k >= inputVessel.size())
			j++;
		if(j >= inputVessel.size())
			break;
	}
	vesselMBRs = inputVessel;
	return vesselMBRs;
}

vector<int> PredictUtil::trajectoryFilter(vector<Vessel*>& inputVessel, vector<Event*>& inputObj)
{
	vector<Vessel*> predictedVessel;
	vector<Event*> predictedObj;
	vector<int> candidateID;

	for (int i = 0; i < inputVessel.size(); i++) {
		Vessel* futureVes = inputVessel[i]->predictLoc(I);
		predictedVessel.push_back(futureVes);
	}
	for (int i = 0; i < inputObj.size(); i++) {
		Event* futureEv = inputObj[i]->predictLoc(I);
		predictedObj.push_back(futureEv);
	}


	for (int k = 0; k < inputObj.size(); k++) {
			
		Point p1 = inputObj[k]->loc;
		Point p2 = predictedObj[k]->loc;

		for (int j = 0; j < inputVessel.size(); j++) {
			Point q1 = inputVessel[j]->loc;
			Point q2 = predictedVessel[j]->loc;
			// Find the four orientations needed for general and
			// special cases
			int o1 = orientation(p1, q1, p2);
			int o2 = orientation(p1, q1, q2);
			int o3 = orientation(p2, q2, p1);
			int o4 = orientation(p2, q2, q1);
			// General case
			if (o1 != o2 && o3 != o4) {
				candidateID.push_back(inputObj[k]->id);
				break;
			}
			// Special Cases
			if (o1 == 0 && onSegment(p1, p2, q1)) {
				candidateID.push_back(inputObj[k]->id);
				break;
			}
			if (o2 == 0 && onSegment(p1, q2, q1)) {
				candidateID.push_back(inputObj[k]->id);
				break;
			}
			if (o3 == 0 && onSegment(p2, p1, q2)) {
				candidateID.push_back(inputObj[k]->id);
				break;
			}
			if (o4 == 0 && onSegment(p2, q1, q2)) {
				candidateID.push_back(inputObj[k]->id);
				break;
			}
		}
	}
	return candidateID;
}

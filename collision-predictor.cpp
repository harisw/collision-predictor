// collision-predictor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <math.h> 
#include <chrono>
#include <iterator>
using namespace std;
using namespace std::chrono;
#include "Miniball.hpp"
#include "EventPtr.h"
#include "CEntry.h"
#include "TPRTree.h"
#include "PredictUtil.h"
#include "Util.h"
#define VESSEL_FILENAME "vessel_499.csv"
#define FILENAME "events_Approach - Bypass1000.txt"
#define START_T 0		//GENERATED
#define MAX_T 100		//GENERATED
#define I 10
#define SMALL_I 5
#define CALCULATE_INTERVAL 5
#define SHOW_WARN 1
//#define SHORT_EXP
vector< vector<Event*> > inputEvents;
vector<Vessel*> ourVessels;

vector<Vessel*> buVessels;
int numOfObj;
int numOfVessel;

vector< vector<pair<int, int> > > naiveResult;	//t * collisionIDs
vector< vector<pair<int, int> > > TPRResult;
vector< vector<pair<int, int> > > hybridResult;

void refineAISData();

void updateVesselLoc() {
	for (int j = 0; j < ourVessels.size(); j++) {
		Point newLoc = Point(ourVessels[j]->loc.x + ourVessels[j]->vx, ourVessels[j]->loc.y + ourVessels[j]->vy);
		ourVessels[j]->loc = newLoc;
	}
}

void naiveMethod() {
	cout << endl << "Naive" << endl;
	int currentT = START_T;
	int maxT = MAX_T;
	unsigned long curDuration = 0;
	unsigned long total = 0;	
	while (currentT < maxT) {
		naiveResult.push_back({});
		auto start = high_resolution_clock::now();
		for(int j=0; j<inputEvents[currentT].size(); j++) {
			for (int k = 0; k < ourVessels.size(); k++) {
				double dist = Util::distance(ourVessels[k]->loc, inputEvents[currentT][j]->loc);
#ifdef SHOW_WARN
				if (dist <= ourVessels[k]->r) {
					naiveResult[currentT].push_back(make_pair(k, inputEvents[currentT][j]->id));
				}
#endif // SHOW_WARN
			}
		}
#pragma region TIME_MEASUREMENT
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		curDuration += duration.count();
#ifdef SHORT_EXP
		total += curDuration;
		cout << "#" << currentT << " Time taken by cycle " << currentT << ":  " << curDuration << endl;
		curDuration = 0;
#endif // SHORT_EXP
#ifndef SHORT_EXP
		if (currentT > 0 && currentT % CALCULATE_INTERVAL == 0) {
			total += (curDuration / CALCULATE_INTERVAL);
			cout << "#" << currentT << " Time taken by cycle " << currentT / CALCULATE_INTERVAL << ":  " << curDuration / CALCULATE_INTERVAL << endl;
			curDuration = 0;
		}
#endif
#pragma endregion
		currentT++;
		updateVesselLoc();
	}
	cout << " Total duration " << total << endl;
	ourVessels = buVessels;
}
void TPRMethod() {
	cout << endl << "TPR Only" << endl;
	int currentT = 0;
	int maxT = MAX_T;
	set<int> inputIDs;
	set<int>::iterator inputItt;
	vector< vector<int> > candidateIDs;
	candidateIDs.insert(candidateIDs.end(), ourVessels.size(), {});
	TPRTree* tree = nullptr;
	unsigned long curDuration = 0;
	unsigned long total = 0;
	while (currentT < maxT) {
		TPRResult.push_back({});
		auto start = high_resolution_clock::now();
		if (currentT % I == 0) {
			tree = new TPRTree();
			PredictUtil::trajectoryFilter(inputIDs, ourVessels, inputEvents[currentT], currentT);
			for (inputItt = inputIDs.begin(); inputItt != inputIDs.end(); inputItt++) {
				Event* ev = inputEvents[currentT][*inputItt];
				tree->Insert(CEntry(inputEvents[currentT][*inputItt]->id, currentT, inputEvents[currentT][*inputItt]->loc.x, inputEvents[currentT][*inputItt]->loc.y,
					0.0, inputEvents[currentT][*inputItt]->vx, inputEvents[currentT][*inputItt]->vy, 0.0));
			}
		}

		for (int j = 0; j < ourVessels.size(); j++) {
			vector<CEntry> tempCandidates;
			Vessel* currArea = ourVessels[j];
			tree->rangeQueryKNN4(currArea->loc.x, currArea->loc.y, 0.0, currArea->r, tempCandidates, currentT % I);
			for (int k = 0; k < tempCandidates.size(); k++) {
#ifdef SHOW_WARN
				TPRResult[currentT].push_back(make_pair(j, inputEvents[currentT][j]->id));
#endif // SHOW_WARN
			}
		}
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		curDuration += duration.count();
#ifdef SHORT_EXP
		total += curDuration;
		cout << "#" << currentT << " Time taken by cycle " << currentT << ":  " << curDuration << endl;
		curDuration = 0;
#endif // SHORT_EXP
#ifndef SHORT_EXP
		if (currentT > 0 && currentT % CALCULATE_INTERVAL == 0) {
			total += (curDuration / CALCULATE_INTERVAL);
			cout << "#" << currentT << " Time taken by cycle " << currentT / CALCULATE_INTERVAL << ":  " << curDuration / CALCULATE_INTERVAL << endl;
			curDuration = 0;
		}
#endif
		currentT++;
		updateVesselLoc();
	}
	cout << " Total duration " << total << endl;
	ourVessels = buVessels;
}

void newHybridMethod() {
	cout << endl << "Hybrid" << endl;
	int currentT = START_T;
	int maxT = MAX_T;
	set<int> inputIDs;
	set<int>::iterator inputItt;
	vector< vector<int> > candidateIDs;
	candidateIDs.insert(candidateIDs.end(), ourVessels.size(), {});
	TPRTree* tree = nullptr;
	TPRTree* vesselTree = nullptr;
	unsigned long curDuration = 0;
	unsigned long total = 0;
	set<int> tempCandidates;
	set<int> overlappingVessel;
	set<int>::iterator objItt, vesselItt;
	while (currentT < maxT) {
		hybridResult.push_back({});
		auto start = high_resolution_clock::now();
		if (currentT % I == 0) {
			tree = new TPRTree();
			vesselTree = new TPRTree();
			inputIDs.clear();
			PredictUtil::trajectoryFilter(inputIDs, ourVessels, inputEvents[currentT], currentT, vesselTree);

			for (inputItt = inputIDs.begin(); inputItt != inputIDs.end(); inputItt++) {
				Event* ev = inputEvents[currentT][*inputItt];
				tree->Insert(CEntry(inputEvents[currentT][*inputItt]->id, 0, inputEvents[currentT][*inputItt]->loc.x, inputEvents[currentT][*inputItt]->loc.y,
					0.0, inputEvents[currentT][*inputItt]->vx, inputEvents[currentT][*inputItt]->vy, 0.0));
			}
		}
		if (currentT % SMALL_I == 0) {
			//vesselTree->PrintAllEntry();
			tempCandidates.clear();
			overlappingVessel.clear();
			vesselTree->FindOverlapping(tempCandidates, overlappingVessel, tree, (currentT%I) + SMALL_I);
		}

		for (vesselItt = overlappingVessel.begin(); vesselItt != overlappingVessel.end(); vesselItt++) {	//FOR VESS
			Vessel* currVessel = ourVessels[*vesselItt];
			for (objItt = tempCandidates.begin(); objItt != tempCandidates.end(); objItt++) {	//FOR OBJ
				double dist = Util::distance(currVessel->loc, inputEvents[currentT][*objItt]->loc);
				//double dist = Util::distance(ourVessels[j]->loc, inputEvents[currentT][candidateIDs[j][k]]->loc);
#ifdef SHOW_WARN
				if (dist <= currVessel->r)
					hybridResult[currentT].push_back(make_pair(*vesselItt, *objItt));

#endif // SHOW_WARN
			}
		}
#pragma region TIME_MEASUREMENT
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		curDuration += duration.count();
#ifdef SHORT_EXP
		total += curDuration;
		cout << "#" << currentT << " Time taken by cycle " << currentT << ":  " << curDuration << endl;
		curDuration = 0;
#endif // SHORT_EXP
#ifndef SHORT_EXP
		if (currentT > 0 && currentT % CALCULATE_INTERVAL == 0) {
			total += (curDuration / CALCULATE_INTERVAL);
			cout << "#" << currentT << " Time taken by cycle " << currentT / CALCULATE_INTERVAL << ":  " << curDuration / CALCULATE_INTERVAL << endl;
			curDuration = 0;
		}
#endif
#pragma endregion
		currentT++;
		updateVesselLoc();
	}
	cout << " Total duration " << total << endl;
	ourVessels = buVessels;
}

int main()
{
	Util::objFilename = FILENAME;
	Util::vesselFilename = VESSEL_FILENAME;
	Util::maxT = MAX_T;
	Util::interval = I;
	Util::smInterval = SMALL_I;
	Util::importVesselData(ourVessels, buVessels, numOfVessel);
	Util::importObjData(inputEvents, numOfObj);


	naiveMethod();
	TPRMethod();
	newHybridMethod();
	//refineAISData();
	Util::exportResult(naiveResult, TPRResult, hybridResult);
}

//
//
//void refineAISData()
//{
//	string filename("events_AIS.csv");
//	fstream newfile;
//	newfile.open(filename, ios::in); //open a file to perform read operation using file object
//	vector< vector<Event*> > collectedEvents = {};
//	int objMax = 120;
//	if (newfile.is_open()) {   //checking whether the file is open
//		string tp;
//		int global_itt = 0;
//		int curr_itt;
//		int obj_id;
//		int obj_count = 0;
//		string token, vx, vy, x, y;
//
//		collectedEvents.push_back({});
//		collectedEvents[0].insert(collectedEvents[0].end(), objMax, nullptr);
//
//		int collective_itt = 0;
//		bool first = true;
//		bool boolID[380] = { 0 };
//		Event* defaultEv = nullptr;
//		getline(newfile, tp);		//skip header
//		while (getline(newfile, tp)) { //read data from file object and put it into string.
//			istringstream tokenizer(tp);
//
//			getline(tokenizer, token, '|'); //get timestamp
//			curr_itt = stoi(token);
//			if (curr_itt > global_itt) {
//				for (int j = 0; j < objMax; j++) {
//					if (collectedEvents[global_itt][j] == nullptr) {
//						if (global_itt == 0) {
//							Event* currentEv = new Event(global_itt, j, defaultEv->vx, defaultEv->vy,
//								defaultEv->loc.x + defaultEv->vx, defaultEv->loc.y + defaultEv->vy);
//							collectedEvents[global_itt][j] = currentEv;
//						}
//						else {
//							Event* prevEv = collectedEvents[global_itt - 1][j];
//							Event* currentEv = new Event(global_itt, j, prevEv->vx, prevEv->vy,
//								prevEv->loc.x + prevEv->vx, prevEv->loc.y + prevEv->vy);
//							collectedEvents[global_itt][j] = currentEv;
//						}
//					}
//				}
//				collectedEvents.push_back({});
//				collectedEvents[curr_itt].insert(collectedEvents[curr_itt].end(), objMax + 1, nullptr);
//				global_itt = curr_itt;
//			}
//
//			if (global_itt >= 1000)
//				break;
//
//			getline(tokenizer, x, '|');
//			getline(tokenizer, y, '|');
//			getline(tokenizer, token, '|');
//			obj_id = stoi(token);
//
//			getline(tokenizer, token, '|');
//			istringstream tokenizer2(token);
//			getline(tokenizer2, vx, ','); getline(tokenizer2, vy, ',');
//
//
//			Event* currentEv = new Event(global_itt, obj_id, stod(vx), stod(vy), stod(x), stod(y));
//			collectedEvents[global_itt][obj_id] = currentEv;
//			if (global_itt == 0)
//				defaultEv = currentEv;
//		}
//		numOfObj = obj_id;
//		newfile.close(); //close the file object.
//	}
//	else {
//		cerr << "Error Opening File!!" << endl;
//		return;
//	}
//	//CREATE NEW FILE
//
//	ostringstream oss;
//	oss << "refined_events_AIS.txt";
//	filename = oss.str();
//
//	ofstream outfile;
//	outfile.open(filename, ofstream::trunc); // opens the file
//	if (!outfile) { // file couldn't be opened
//		cerr << "Error: file could not be opened" << endl;
//		exit(1);
//	}
//
//	int global_itt = 0;
//	int max_itt = 105;
//	double speedX, speedY;
//	while (true && global_itt < max_itt) {
//		bool hasFinished = true;
//		for (int i = 0; i < objMax; ++i) {
//
//			outfile << global_itt << "|" << collectedEvents[global_itt][i]->id << "|" << collectedEvents[global_itt][i]->vx << ","
//				<< collectedEvents[global_itt][i]->vy << "|" << collectedEvents[global_itt][i]->loc.x << "," << collectedEvents[global_itt][i]->loc.y << endl;
//		}
//		global_itt++;
//	}
//	outfile.close();
//}
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
#define FILENAME "sc1 - Stop502.txt"
#define START_T 0		//GENERATED
#define MAX_T 20		//GENERATED
#define I 5
//#define SMALL_I 5
#define CALCULATE_INTERVAL 10
#define SHOW_WARN 1
//#define SHORT_EXP

vector< vector<EventPtr*> > inputEvents;
vector<Vessel*> ourVessels;
vector<Vessel*> buVessels;
int numOfObj;
int numOfVessel;
vector< vector<pair<int, int> > > naiveResult;	//t * collisionIDs
vector< vector<pair<int, int> > > treeOnlyResult;
vector< vector<pair<int, int> > > proposedResult;

void refineAISData();

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
			for (int k = j+1; k < inputEvents[currentT].size()-1; k++) {
				double dist = Util::distance(inputEvents[currentT][j]->loc, inputEvents[currentT][k]->loc);
				double r = max(inputEvents[currentT][j]->r, inputEvents[currentT][k]->r);
#ifdef SHOW_WARN
				if (dist <= r)
					naiveResult[currentT].push_back(make_pair(inputEvents[currentT][j]->id, inputEvents[currentT][k]->id));
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
	}
	cout << " Total duration " << total << endl;
}

void proposedMethod() {
	cout << endl << "Hybrid" << endl;
	int currentT = START_T;
	int maxT = MAX_T;
	set<int> inputIDs;
	set<int>::iterator inputItt;
	vector< vector<int> > candidateIDs;
	candidateIDs.insert(candidateIDs.end(), ourVessels.size(), {});
	TPRTree* tree = nullptr;
	unsigned long curDuration = 0;
	unsigned long total = 0;
	vector< pair<int, int> > tempCandidates;
	while (currentT < maxT) {
		proposedResult.push_back({});
		auto start = high_resolution_clock::now();
		if (currentT % I == 0) {
			tree = new TPRTree();
			inputIDs.clear();
			PredictUtil::trajectoryFilter(inputIDs, inputEvents[currentT]);

			for (inputItt = inputIDs.begin(); inputItt != inputIDs.end(); inputItt++) {
				tree->Insert(CEntry(*inputItt, currentT, inputEvents[currentT][*inputItt]->loc.x, inputEvents[currentT][*inputItt]->loc.y,
					0.0, inputEvents[currentT][*inputItt]->vx, inputEvents[currentT][*inputItt]->vy, 0.0, inputEvents[currentT][*inputItt]->r));
			}
			tempCandidates.clear();
			tree->FindOverlapping(tempCandidates);
		}
		if (!tempCandidates.empty()) {
			for(int j=0; j< tempCandidates.size(); j++){
				int firstVessel = tempCandidates[j].first;
				int secVessel = tempCandidates[j].second;
				double dist = Util::distance(inputEvents[currentT][firstVessel]->loc, inputEvents[currentT][secVessel]->loc);
				double r = max(inputEvents[currentT][firstVessel]->r, inputEvents[currentT][secVessel]->r);
#ifdef SHOW_WARN
				if (dist <= r) {
					proposedResult[currentT].push_back(make_pair(inputEvents[currentT][firstVessel]->id, inputEvents[currentT][secVessel]->id));
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
	}
	cout << " Total duration " << total << endl;
	ourVessels = buVessels;
}

int main()
{
	Util::objFilename = FILENAME;
	Util::maxT = MAX_T;
	Util::interval = I;
	Util::importObjData(inputEvents, numOfObj);
	
	naiveMethod();
	proposedMethod();
	//refineAISData();
	Util::exportResult(naiveResult, treeOnlyResult, proposedResult);
}

void refineAISData()
{
	string filename("events_AIS.csv");
	fstream newfile;
	newfile.open(filename, ios::in); //open a file to perform read operation using file object
	vector< vector<EventPtr*> > collectedEvents = {};
	int objMax = 120;
	if (newfile.is_open()) {   //checking whether the file is open
		string tp;
		int global_itt = 0;
		int curr_itt;
		int obj_id;
		int obj_count = 0;
		string token, vx, vy, x, y, r;

		collectedEvents.push_back({});
		collectedEvents[0].insert(collectedEvents[0].end(), objMax, nullptr);

		int collective_itt = 0;
		bool first = true;
		bool boolID[380] = { 0 };
		EventPtr* defaultEv = nullptr;
		getline(newfile, tp);		//skip header
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);

			getline(tokenizer, token, '|'); //get timestamp
			curr_itt = stoi(token);
			if (curr_itt > global_itt) {
				for (int j = 0; j < objMax; j++) {
					if (collectedEvents[global_itt][j] == nullptr) {
						if (global_itt == 0) {
							EventPtr* currentEv = new EventPtr(global_itt, j, defaultEv->vx, defaultEv->vy,
								defaultEv->loc.x + defaultEv->vx, defaultEv->loc.y + defaultEv->vy);
							collectedEvents[global_itt][j] = currentEv;
						}
						else {
							EventPtr* prevEv = collectedEvents[global_itt - 1][j];
							EventPtr* currentEv = new EventPtr(global_itt, j, prevEv->vx, prevEv->vy,
								prevEv->loc.x + prevEv->vx, prevEv->loc.y + prevEv->vy);
							collectedEvents[global_itt][j] = currentEv;
						}
					}
				}
				collectedEvents.push_back({});
				collectedEvents[curr_itt].insert(collectedEvents[curr_itt].end(), objMax + 1, nullptr);
				global_itt = curr_itt;
			}

			if (global_itt >= 1000)
				break;

			getline(tokenizer, token, '|');
			obj_id = stoi(token);

			getline(tokenizer, token, '|');
			istringstream tokenizer2(token);
			getline(tokenizer2, x, ','); getline(tokenizer2, y, ',');

			getline(tokenizer, token, '|');
			istringstream tokenizer3(token);
			getline(tokenizer3, vx, ','); getline(tokenizer3, vy, ',');

			getline(tokenizer, r, '|');

			EventPtr* currentEv = new EventPtr(global_itt, obj_id, stod(vx), stod(vy), stod(x), stod(y), stod(r));
			collectedEvents[global_itt][obj_id] = currentEv;
			if (global_itt == 0)
				defaultEv = currentEv;
		}
		numOfObj = obj_id;
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl;
		return;
	}
	//CREATE NEW FILE

	ostringstream oss;
	oss << "refined_events_AIS.txt";
	filename = oss.str();

	ofstream outfile;
	outfile.open(filename, ofstream::trunc); // opens the file
	if (!outfile) { // file couldn't be opened
		cerr << "Error: file could not be opened" << endl;
		exit(1);
	}

	int global_itt = 0;
	int max_itt = 105;
	double speedX, speedY;
	while (true && global_itt < max_itt) {
		bool hasFinished = true;
		for (int i = 0; i < objMax; ++i) {

			outfile << global_itt << "|" << collectedEvents[global_itt][i]->id << "|" << collectedEvents[global_itt][i]->vx << ","
				<< collectedEvents[global_itt][i]->vy << "|" << collectedEvents[global_itt][i]->loc.x << "," << collectedEvents[global_itt][i]->loc.y
				<< "|" << collectedEvents[global_itt][i]->r << endl;
		}
		global_itt++;
	}
	outfile.close();
}
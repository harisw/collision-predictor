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
using namespace std;
using namespace std::chrono;
#include "EventPtr.h"
#include "CEntry.h"
#include "TPRTree.h"
#include "PredictUtil.h"
#include "Util.h"
#define VESSEL_FILENAME "cq3.txt"
#define FILENAME "events_Approach - Stop1000.txt"
#define MAX_T 100
#define I 10

vector< vector<Event*> > inputEvents;
vector<Vessel*> ourVessels;
int numOfObj;
int numOfVessel;

void importVesselData() {
	string filename(VESSEL_FILENAME);
	fstream newfile;

	newfile.open(filename, ios::in); //open a file to perform read operation using file object
	if (newfile.is_open()) {   //checking whether the file is open
		string tp;
		string token, vx, vy, x, y, r;
		vector< vector<Event*> > collectedEvents = {};
		collectedEvents.push_back({});
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);

			getline(tokenizer, token, '|');

			getline(tokenizer, token, '|');
			int obj_id = stoi(token);

			getline(tokenizer, x, '|');
			getline(tokenizer, y, '|');

			getline(tokenizer, vx, '|');
			getline(tokenizer, vy, '|');

			getline(tokenizer, r, '|');
			
			Vessel* currentVessel = new Vessel(obj_id, stod(x), stod(y), stod(vx), stod(vy), stod(r));
			ourVessels.push_back(currentVessel);
		}
		numOfVessel = ourVessels.size();
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl;
		return;
	}
}

void importGeneratedData() {
	string filename(FILENAME);
	fstream newfile;

	newfile.open(filename, ios::in); //open a file to perform read operation using file object
	if (newfile.is_open()) {   //checking whether the file is open
		string tp;
		int global_itt = 0;
		int obj_id;
		int obj_count = 0;

		string token, vx, vy, x, y;
		inputEvents.push_back({});
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);

			getline(tokenizer, token, '|'); //READ timestamp
			if (stoi(token) > global_itt) {
				if (global_itt >= MAX_T)
					break;

				global_itt++;
				inputEvents.push_back({});
			}

			getline(tokenizer, token, '|');
			obj_id = stoi(token);
			if (obj_id > obj_count) obj_count = obj_id;

			getline(tokenizer, token, '|');
			istringstream tokenizer2(token);
			getline(tokenizer2, vx, ','); getline(tokenizer2, vy, ',');

			getline(tokenizer, token, '|');
			istringstream tokenizer3(token);
			getline(tokenizer3, x, ','); getline(tokenizer3, y, ',');
			Event* currentEv = new Event(stoi(token), obj_id, stod(vx), stod(vy), stod(x), stod(y) );
			inputEvents[global_itt].push_back(currentEv);
		}
		numOfObj = obj_id;
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl;
		return;
	}
}

void importAISData() {
	string filename("cleaned_AIS.csv");
	fstream newfile;

	newfile.open(filename, ios::in); //open a file to perform read operation using file object
	if (newfile.is_open()) {   //checking whether the file is open
		string tp;
		int global_itt = 0;
		int obj_id;
		int obj_count = 0;

		string token, vx, vy, x, y;
		vector< vector<Event*> > collectedEvents = {};
		collectedEvents.push_back({});

		string currentT = "";
		int collective_itt = 0;
		bool first = true;
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);

			getline(tokenizer, token, '|'); //skip first id

			getline(tokenizer, token, '|'); //get timestamp

			if (first) {
				currentT = token;
				first = false;
			}

			if (token != currentT) {
				currentT = token;
				if (collective_itt == 5) {
					collective_itt = 0;
					global_itt++;
					collectedEvents.push_back({});
				}
				else
					collective_itt++;

				if (global_itt >= 1000)
					break;

			}

			getline(tokenizer, x, '|');

			getline(tokenizer, y, '|');

			getline(tokenizer, token, '|');
			obj_id = stoi(token);
			if (obj_id > obj_count) obj_count = obj_id;

			getline(tokenizer, token, '|');
			istringstream tokenizer2(token);
			getline(tokenizer2, vx, ','); getline(tokenizer2, vy, ',');


			Event* currentEv = new Event(stoi(token), obj_id, stod(vx), stod(vy), stod(x), stod(y));
			collectedEvents[global_itt].push_back(currentEv);
		}
		numOfObj = obj_id;
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl;
		return;
	}
}

void updateVesselLoc() {
	for (int j = 0; j < ourVessels.size(); j++) {
		Point newLoc = Point(ourVessels[j]->loc.x + ourVessels[j]->vx, ourVessels[j]->loc.y + ourVessels[j]->vy);
		ourVessels[j]->loc = newLoc;
	}
}
void naiveMethod() {
	cout << endl << "Naive" << endl;
	int currentT = 0;
	int maxT = MAX_T;
	unsigned long curDuration = 0;
	while (currentT < maxT) {
		auto start = high_resolution_clock::now();

		for(int j=0; j<inputEvents[currentT].size(); j++) {
			for (int k = 0; k < ourVessels.size(); k++) {
				double dist = Util::distance(ourVessels[k]->loc, inputEvents[currentT][j]->loc);
				/*if (dist <= ourVessels[k]->r)
					cout << "COLLISION Vessel " << ourVessels[k]->id << " and obj #" << inputEvents[currentT][j]->id << endl;*/			}
		}
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		curDuration += duration.count();
		if (currentT > 0 && currentT % 10 == 0) {
			cout << "Time taken by cycle " << currentT / 10 << ":  " << curDuration / 10 << endl;
			curDuration = 0;
		}
		currentT++;
		updateVesselLoc();
	}
}
void TPRMethod() {
	cout << endl << "TPR Only" << endl;

	int currentT = 0;
	int maxT = MAX_T;
	vector<int> inputIDs;
	set<int> candidateIDs;
	set<int>::iterator itt;
	vector<Vessel*> predictedMBRs;
	TPRTree* tree = nullptr;
	unsigned long curDuration = 0;
	while (currentT < maxT) {
		auto start = high_resolution_clock::now();
		if (currentT % 10 == 0) {
			inputIDs.empty();
			inputIDs = PredictUtil::trajectoryFilter(ourVessels, inputEvents[currentT]);
			predictedMBRs = PredictUtil::predictMBRs(ourVessels);
			tree = new TPRTree();

			for (int j = 0; j < inputIDs.size(); j++) {
				if (inputIDs[j] >= inputEvents[currentT].size())
					continue;

				Event* ev = inputEvents[currentT][inputIDs[j]];
				tree->Insert(CEntry(ev->id, currentT, ev->loc.x, ev->loc.y, 0.0, ev->vx, ev->vy, 0.0));
			}
		}

		vector<CEntry> tempCandidates;
		for (int j = 0; j < predictedMBRs.size(); j++) {
			tempCandidates.empty();
			Vessel* currArea = predictedMBRs[j];
			tree->rangeQueryKNN4(currArea->loc.x, currArea->loc.y, 0.0, currArea->r + (sqrt(pow(currArea->vx, 2) +
				pow(currArea->vy, 2))), tempCandidates, currentT % 10);
			for (int k = 0; k < tempCandidates.size(); k++)
				candidateIDs.insert(tempCandidates[k].m_id);

		}
		if (!candidateIDs.empty()) {
			for (itt = candidateIDs.begin(); itt != candidateIDs.end(); itt++) {
				if (*itt >= inputEvents[currentT].size())
					continue;
				for (int k = 0; k < ourVessels.size(); k++) {
					double dist = Util::distance(ourVessels[k]->loc, inputEvents[currentT][*itt]->loc);
					/*if (dist <= ourVessels[k]->r)
						cout << "COLLISION Vessel " << ourVessels[k]->id << " and obj #" << inputEvents[currentT][*itt]->id << endl;*/
				}
			}
		}
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		curDuration += duration.count();
		if (currentT > 0 && currentT % 10 == 0) {
			cout << "#" << currentT << " Time taken by cycle " << currentT / 10 << ":  " << curDuration / 10 << endl;
			curDuration = 0;
		}
		currentT++;
		updateVesselLoc();
	}
}

void hybridMethod() {
	cout << endl << "Hybrid" << endl;
	int currentT = 0;
	int maxT = MAX_T;
	vector<int> inputIDs;
	set<int> candidateIDs;
	set<int>::iterator itt;
	vector<Vessel*> predictedMBRs;
	TPRTree* tree = nullptr;
	unsigned long curDuration = 0;
	while (currentT < maxT) {
		auto start = high_resolution_clock::now();

		if (currentT % 10 == 0) {
			inputIDs.empty();
			//enlargedBufferZone.empty();
			inputIDs = PredictUtil::trajectoryFilter(ourVessels, inputEvents[currentT]);
			predictedMBRs = PredictUtil::predictMBRs(ourVessels);

			/*predictBufferVessels(enlargedBufferZone);
			trajectoryFilter(inputIDs);*/
			tree = new TPRTree();

			for (int j = 0; j < inputIDs.size(); j++) {
				if (inputIDs[j] >= inputEvents[currentT].size())
					continue;

				Event* ev = inputEvents[currentT][inputIDs[j]];
				tree->Insert(CEntry(ev->id, currentT, ev->loc.x, ev->loc.y, 0.0, ev->vx, ev->vy, 0.0));
			}
		}

		if (currentT % 5 == 0) {
			vector<CEntry> tempCandidates;
			//enlargedBufferZone.empty();
			//predictBufferZones(enlargedBufferZone);
			for (int j = 0; j < predictedMBRs.size(); j++) {
				//tempCandidates.empty();
				Vessel* currArea = predictedMBRs[j];
				tree->rangeQueryKNN4(currArea->loc.x, currArea->loc.y, 0.0, currArea->r + (I * sqrt(pow(currArea->vx, 2) +
					pow(currArea->vy, 2))), tempCandidates, currentT % 10);

				for (int k = 0; k < tempCandidates.size(); k++)
					candidateIDs.insert(tempCandidates[k].m_id);

			}
		}


		if (!candidateIDs.empty()) {
			for (itt = candidateIDs.begin(); itt != candidateIDs.end(); itt++) {
				if (*itt >= inputEvents[currentT].size())
					continue;

				for (int k = 0; k < ourVessels.size(); k++) {
					double dist = Util::distance(ourVessels[k]->loc, inputEvents[currentT][*itt]->loc);
					/*if (dist <= ourVessels[k]->r)
						cout << "COLLISION Vessel " << ourVessels[k]->id << " and obj #" << inputEvents[currentT][*itt]->id << endl;*/
				}
			}
		}
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		curDuration += duration.count();
		if (currentT > 0 && currentT % 10 == 0) {
			cout << "#" << currentT << " Time taken by cycle " << currentT / 10 << ":  " << curDuration / 10 << endl;
			curDuration = 0;
		}

		currentT++;
		updateVesselLoc();
	}
}
int main()
{
	importVesselData();
	importGeneratedData();
	naiveMethod();
	TPRMethod();
	hybridMethod();
}
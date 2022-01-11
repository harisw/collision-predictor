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
#define VESSEL_FILENAME "vessel_AIS.csv"
#define FILENAME "refined_events_AIS.txt"
//#define FILENAME "events_Approach - U Turn502.txt"
#define MAX_T 50
#define I 15
#define SMALL_I 5
#define CALCULATE_INTERVAL 5
//#define SHOW_WARN 1
#define SHORT_EXP
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
			getline(tokenizer, x, '|');getline(tokenizer, y, '|');
			getline(tokenizer, vx, '|');getline(tokenizer, vy, '|');
			getline(tokenizer, r, '|');
			Vessel* currentVessel = new Vessel(obj_id, stod(x), stod(y), stod(vx), stod(vy), stod(r));
			ourVessels.push_back(currentVessel);
		}
		numOfVessel = ourVessels.size();
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl;return;
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
		cerr << "Error Opening File!!" << endl;return; }
}

void importVesselAIS() {
	string filename(VESSEL_FILENAME);
	fstream newfile;
	newfile.open(filename, ios::in); //open a file to perform read operation using file object
	if (newfile.is_open()) {   //checking whether the file is open
		string tp;
		string token, vx, vy, x, y, r;
		getline(newfile, tp);	//skip header row
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);
			getline(tokenizer, token, '|');
			getline(tokenizer, x, '|'); getline(tokenizer, y, '|');
			getline(tokenizer, token, '|');
			int obj_id = stoi(token);

			getline(tokenizer, token, '|');
			istringstream tokenizer2(token);
			getline(tokenizer2, vx, ','); getline(tokenizer2, vy, ',');

			getline(tokenizer, r, '|');
			Vessel* currentVessel = new Vessel(obj_id, stod(x), stod(y), stod(vx), stod(vy), stod(r));
			ourVessels.push_back(currentVessel);
		}
		numOfVessel = ourVessels.size();
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl; return;
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
	unsigned long total = 0;	
	while (currentT < maxT) {
		auto start = high_resolution_clock::now();
		for(int j=0; j<inputEvents[currentT].size(); j++) {
			for (int k = 0; k < ourVessels.size(); k++) {
				double dist = Util::distance(ourVessels[k]->loc, inputEvents[currentT][j]->loc);
#ifdef SHOW_WARN
			if (dist <= ourVessels[k]->r)
				cout << "COLLISION Vessel " << ourVessels[k]->id << " and obj #" << inputEvents[currentT][j]->id << endl;
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
}
void TPRMethod() {
	cout << endl << "TPR Only" << endl;
	int currentT = 0;
	int maxT = MAX_T;
	vector<int> inputIDs;
	set<int> candidateIDs;
	set<int>::iterator itt;
	TPRTree* tree = nullptr;
	unsigned long curDuration = 0;
	unsigned long total = 0;
	while (currentT < maxT) {
#ifndef SHORT_EXP
		auto start = high_resolution_clock::now();
#endif // SHORT_EXP

		if (currentT % I == 0) {
			inputIDs.empty();
			inputIDs = PredictUtil::trajectoryFilter(ourVessels, inputEvents[currentT]);
			tree = new TPRTree();
			for (int j = 0; j < inputIDs.size(); j++) {
				if (inputIDs[j] >= inputEvents[currentT].size())
					continue;

				Event* ev = inputEvents[currentT][inputIDs[j]];
				tree->Insert(CEntry(ev->id, currentT, ev->loc.x, ev->loc.y, 0.0, ev->vx, ev->vy, 0.0));
			}
		}
#ifdef SHORT_EXP
		auto start = high_resolution_clock::now();
#endif // SHORT_EXP

		for (int j = 0; j < ourVessels.size(); j++) {
			vector<CEntry> tempCandidates;
			Vessel* currArea = ourVessels[j];
			tree->rangeQueryKNN4(currArea->loc.x, currArea->loc.y, 0.0, currArea->r, tempCandidates, currentT % 10);
			for (int k = 0; k < tempCandidates.size(); k++) {
#ifdef SHOW_WARN
				cout << "COLLISION Vessel " << currArea->id << " and obj #" << tempCandidates[k].m_id << endl;
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
}

void hybridMethod() {
	cout << endl << "Hybrid" << endl;
	int currentT = 0;
	int maxT = MAX_T;
	vector<int> inputIDs;
	vector< vector<int> > candidateIDs;
	candidateIDs.insert(candidateIDs.end(), ourVessels.size(), {});
	set<int>::iterator itt;
	vector<Vessel*> predictedMBRs;
	TPRTree* tree = nullptr;
	unsigned long curDuration = 0;
	unsigned long total = 0;
	while (currentT < maxT) {
#ifndef SHORT_EXP
		auto start = high_resolution_clock::now();
#endif // SHORT_EXP

		if (currentT % I == 0) {
			inputIDs.empty();
			inputIDs = PredictUtil::trajectoryFilter(ourVessels, inputEvents[currentT]);
			tree = new TPRTree();

			for (int j = 0; j < inputIDs.size(); j++) {
				if (inputIDs[j] >= inputEvents[currentT].size())
					continue;
				Event* ev = inputEvents[currentT][inputIDs[j]];
				tree->Insert(CEntry(ev->id, currentT, ev->loc.x, ev->loc.y, 0.0, ev->vx, ev->vy, 0.0));
			}
		}

#ifdef SHORT_EXP
		auto start = high_resolution_clock::now();
#endif
		if (currentT % SMALL_I == 0) {
			for (int j = 0; j < ourVessels.size(); j++) {
				vector<CEntry> tempCandidates;
				Vessel* currArea = ourVessels[j];
				tree->rangeQueryKNN4(currArea->loc.x, currArea->loc.y, 0.0, currArea->r + (SMALL_I * sqrt(pow(currArea->vx, 2) +
					pow(currArea->vy, 2))), tempCandidates, currentT % I);
				if (tempCandidates.empty())
					continue;

				candidateIDs[j].push_back({ tempCandidates[0].m_id });
				for (int k = 1; k < tempCandidates.size(); k++)
					candidateIDs[j].push_back(tempCandidates[k].m_id);
			}
		}


		if (!candidateIDs.empty()) {
			for (int j = 0; j < candidateIDs.size(); j++) {	//FOR VESS
				for (int k = 0; k < candidateIDs[j].size(); k++) {	//FOR OBJ
					double dist = Util::distance(ourVessels[j]->loc, inputEvents[currentT][candidateIDs[j][k]]->loc);
#ifdef SHOW_WARN
					if (dist <= ourVessels[k]->r)
						cout << "COLLISION Vessel " << ourVessels[k]->id << " and obj #" << 
						inputEvents[currentT][candidateIDs[j][k]]->id << endl;
#endif // SHOW_WARN
				}
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
}


void refineAISData()
{
	string filename("events_AIS.csv");
	fstream newfile;
	newfile.open(filename, ios::in); //open a file to perform read operation using file object
	vector< vector<Event*> > collectedEvents = {};
	int objMax = 120;
	if (newfile.is_open()) {   //checking whether the file is open
		string tp;
		int global_itt = 0;
		int curr_itt;
		int obj_id;
		int obj_count = 0;
		string token, vx, vy, x, y;

		collectedEvents.push_back({});
		collectedEvents[0].insert(collectedEvents[0].end(), objMax, nullptr);

		int collective_itt = 0;
		bool first = true;
		bool boolID[380] = { 0 };
		Event* defaultEv = nullptr;
		getline(newfile, tp);		//skip header
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);

			getline(tokenizer, token, '|'); //get timestamp
			curr_itt = stoi(token);
			if (curr_itt > global_itt) {
				for (int j = 0; j < objMax; j++) {
					if (collectedEvents[global_itt][j] == nullptr) {
						if (global_itt == 0) {
							Event* currentEv = new Event(global_itt, j, defaultEv->vx, defaultEv->vy,
								defaultEv->loc.x + defaultEv->vx, defaultEv->loc.y + defaultEv->vy);
							collectedEvents[global_itt][j] = currentEv;
						}
						else {
							Event* prevEv = collectedEvents[global_itt - 1][j];
							Event* currentEv = new Event(global_itt, j, prevEv->vx, prevEv->vy,
								prevEv->loc.x + prevEv->vx, prevEv->loc.y + prevEv->vy);
							collectedEvents[global_itt][j] = currentEv;
						}
					}
				}
				collectedEvents.push_back({});
				collectedEvents[curr_itt].insert(collectedEvents[curr_itt].end(), objMax+1, nullptr);
				global_itt = curr_itt;
			}
			
			if (global_itt >= 1000)
				break;

			getline(tokenizer, x, '|');
			getline(tokenizer, y, '|');
			getline(tokenizer, token, '|');
			obj_id = stoi(token);
			
			getline(tokenizer, token, '|');
			istringstream tokenizer2(token);
			getline(tokenizer2, vx, ','); getline(tokenizer2, vy, ',');


			Event* currentEv = new Event(global_itt, obj_id, stod(vx), stod(vy), stod(x), stod(y));
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
	int max_itt = 55;
	double speedX, speedY;
	while (true && global_itt < max_itt) {
		bool hasFinished = true;
		for (int i = 0; i < objMax; ++i) {
			
			outfile << global_itt << "|" << collectedEvents[global_itt][i]->id << "|" << collectedEvents[global_itt][i]->vx << ","
				<< collectedEvents[global_itt][i]->vy << "|" << collectedEvents[global_itt][i]->loc.x << "," << collectedEvents[global_itt][i]->loc.y << endl;
		}
		global_itt++;
	}
	outfile.close();
}

int main()
{
	//importVesselData();
	importVesselAIS();
	importGeneratedData();
	naiveMethod();
	TPRMethod();
	hybridMethod();
	//refineAISData();
}
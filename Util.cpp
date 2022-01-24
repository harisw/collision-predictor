#include "Util.h"
#include "EventPtr.h"
#include <iostream>
#include<cmath>
#include<utility>
#include <fstream>
#include <sstream>

int Util::maxT;
string Util::vesselFilename;
string Util::objFilename;
int Util::interval;
int Util::smInterval;

double Util::distance(Point p, Point q) {
	return sqrt(pow(q.x - p.x, 2) + pow(q.y - p.y, 2));
}

double Util::lineToPointDistance(Point a, Point b, Point c)
{
	double x = c.x;
	double y = c.y;
	if ((a.x - b.x) * (x - b.x) + (a.y - b.y) * (y - b.y) <= 0)
		return sqrt((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y));

	if ((b.x - a.x) * (x - a.x) + (b.y - a.y) * (y - a.y) <= 0)
		return sqrt((x - a.x) * (x - a.x) + (y - a.y) * (y - a.y));

	return abs((b.y - a.y) * x - (b.x - a.x) * y + b.x * a.y - b.y * a.x) /
		sqrt((a.y - b.y) * (a.y - b.y) + (a.x - b.x) * (a.x - b.x));
}

double Util::lineToLineDistance(Point a1, Point a2, Point b1, Point b2)
{
	double result = 99999999;
	double distA, distB;
	distA = min(lineToPointDistance(b1, b2, a1), lineToPointDistance(b1, b2, a2));
	distB = min(lineToPointDistance(a1, a2, b1), lineToPointDistance(a1, a2, b2));

	return min(distA, distB);
}

void Util::importVesselData(vector<Vessel*>& input, vector<Vessel*>& buInput, int& vesselNum)
{
	string filename(vesselFilename);
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
			int obj_id = stoi(token);
			getline(tokenizer, token, '|');
			istringstream tokenizer2(token);
			getline(tokenizer2, x, ','); getline(tokenizer2, y, ',');

			getline(tokenizer, token, '|');
			istringstream tokenizer3(token);
			getline(tokenizer3, vx, ','); getline(tokenizer3, vy, ',');

			getline(tokenizer, r, '|');
			Vessel* currentVessel = new Vessel(obj_id, stod(x), stod(y), stod(vx), stod(vy), stod(r));
			Vessel* ves2 = new Vessel(obj_id, stod(x), stod(y), stod(vx), stod(vy), stod(r));
			input.push_back(currentVessel);
			buInput.push_back(ves2);
		}
		vesselNum = input.size();
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl; return;
	}
}

void Util::importObjData(vector< vector<Event*> >& input, int& objNum)
{
	string filename(objFilename);
	fstream newfile;
	newfile.open(filename, ios::in); //open a file to perform read operation using file object
	if (newfile.is_open()) {   //checking whether the file is open
		string tp;
		int global_itt = 0;
		int obj_id;
		int obj_count = 0;
		double r;
		string token, vx, vy, x, y;
		input.push_back({});
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);
			getline(tokenizer, token, '|'); //READ timestamp
			if (stoi(token) > global_itt) {
				if (global_itt >= maxT)
					break;
				global_itt++;
				input.push_back({});
			}

			getline(tokenizer, token, '|');
			obj_id = stoi(token);
			if (obj_id > obj_count) obj_count = obj_id;

			getline(tokenizer, token, '|');
			istringstream tokenizer3(token);
			getline(tokenizer3, x, ','); getline(tokenizer3, y, ',');

			getline(tokenizer, token, '|');
			istringstream tokenizer2(token);
			getline(tokenizer2, vx, ','); getline(tokenizer2, vy, ',');

			Event* currentEv = new Event(global_itt, obj_id, stod(vx), stod(vy), stod(x), stod(y));
			input[global_itt].push_back(currentEv);
		}
		objNum = obj_id;
		newfile.close(); //close the file object.
	}
	else {
		cerr << "Error Opening File!!" << endl; return;
	}
}

void Util::exportResult(vector< vector<int> >& naiveResult, vector<vector<int> >& TPRResult, vector<vector<int> >& hybridResult)
{
	string filename = "naive_result.csv";
	if (!naiveResult.empty()) {
		ofstream outfile;
		outfile.open(filename, ofstream::trunc); // opens the file
		if (!outfile) { // file couldn't be opened
			cerr << "Error: file could not be opened" << endl;
			exit(1);
		}
		outfile << "t" << "|" << "obj_id" << endl;
		for (int j = 0; j < naiveResult.size(); j++) {
			for (int k = 0; k < naiveResult[j].size(); k++) {
				outfile << j << "|" << naiveResult[j][k] << endl;
			}
		}
		outfile.close();
	}
	if (!TPRResult.empty()) {
		filename = "TPR_result.csv";
		ofstream outfile;
		outfile.open(filename, ofstream::trunc); // opens the file
		if (!outfile) { // file couldn't be opened
			cerr << "Error: file could not be opened" << endl;
			exit(1);
		}
		outfile << "t" << "|" << "obj_id" << endl;
		for (int j = 0; j < TPRResult.size(); j++) {
			for (int k = 0; k < TPRResult[j].size(); k++) {
				outfile << j << "|" << TPRResult[j][k] << endl;
			}
		}
		outfile.close();
	}
	if (!hybridResult.empty()) {
		ofstream outfile;
		filename = "hybrid_result.csv";
		outfile.open(filename, ofstream::trunc); // opens the file
		if (!outfile) { // file couldn't be opened
			cerr << "Error: file could not be opened" << endl;
			exit(1);
		}
		outfile << "t" << "|" << "obj_id" << endl;
		for (int j = 0; j < hybridResult.size(); j++) {
			for (int k = 0; k < hybridResult[j].size(); k++) {
				outfile << j << "|" << hybridResult[j][k] << endl;
			}
		}
		outfile.close();
	}
	return;
}
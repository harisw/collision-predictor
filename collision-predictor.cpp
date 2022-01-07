// collision-predictor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
#include "EventPtr.h"

#define FILENAME "events_Approach - Bypass.txt"

vector<EventPtr> inputEvents;
int numOfObj;
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
		vector< vector<EventPtr> > collectedEvents = {};
		collectedEvents.push_back({});
		while (getline(newfile, tp)) { //read data from file object and put it into string.
			istringstream tokenizer(tp);

			getline(tokenizer, token, '|'); //READ timestamp
			if (stoi(token) > global_itt) {
				if (global_itt >= 15)
					break;

				global_itt++;
				collectedEvents.push_back({});
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
			EventPtr currentEv(stoi(token), obj_id, stod(vx), stod(vy), stod(x), stod(y) );
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
		vector< vector<EventPtr> > collectedEvents = {};
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


			EventPtr currentEv(stoi(token), obj_id, stod(vx), stod(vy), stod(x), stod(y));
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

int main()
{
    std::cout << "Hello World!\n";
}
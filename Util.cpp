#include "Util.h"
#include "EventPtr.h"
#include<cmath>

double Util::distance(Point p, Point q) {
	return sqrt(pow(q.x - p.x, 2) + pow(q.y - p.y, 2));
}

double Util::lineToPointDistance(Point a, Point b, Point circle)
{
	double x = circle.x;
	double y = circle.y;
	if ((a.x - b.x) * (x - b.x) + (a.y - b.y) * (y - b.y) <= 0)
		return sqrt((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y));

	if ((b.x - a.x) * (x - a.x) + (b.y - a.y) * (y - a.y) <= 0)
		return sqrt((x - a.x) * (x - a.x) + (y - a.y) * (y - a.y));

	return abs((b.y - a.y) * x - (b.x - a.x) * y + b.x * a.y - b.y * a.x) /
		sqrt((a.y - b.y) * (a.y - b.y) + (a.x - b.x) * (a.x - b.x));
}

//void Util::refineAISData()
//{
//	string filename("cleaned_AIS.csv");
//	fstream newfile;
//
//	newfile.open(filename, ios::in); //open a file to perform read operation using file object
//	if (newfile.is_open()) {   //checking whether the file is open
//		string tp;
//		int global_itt = 0;
//		int obj_id;
//		int obj_count = 0;
//
//		string token, vx, vy, x, y;
//		vector< vector<Event*> > collectedEvents = {};
//		collectedEvents.push_back({});
//
//		string currentT = "";
//		int collective_itt = 0;
//		bool first = true;
//		while (getline(newfile, tp)) { //read data from file object and put it into string.
//			istringstream tokenizer(tp);
//
//			getline(tokenizer, token, '|'); //skip first id
//			getline(tokenizer, token, '|'); //get timestamp
//
//			if (first) {
//				currentT = token;
//				first = false;
//			}
//
//			if (token != currentT) {
//				currentT = token;
//				if (collective_itt == 5) {
//					collective_itt = 0;
//					global_itt++;
//					collectedEvents.push_back({});
//				}
//				else
//					collective_itt++;
//
//				if (global_itt >= 1000)
//					break;
//
//			}
//
//			getline(tokenizer, x, '|');
//
//			getline(tokenizer, y, '|');
//
//			getline(tokenizer, token, '|');
//			obj_id = stoi(token);
//			if (obj_id > obj_count) obj_count = obj_id;
//
//			getline(tokenizer, token, '|');
//			istringstream tokenizer2(token);
//			getline(tokenizer2, vx, ','); getline(tokenizer2, vy, ',');
//
//
//			Event* currentEv = new Event(stoi(token), obj_id, stod(vx), stod(vy), stod(x), stod(y));
//			collectedEvents[global_itt].push_back(currentEv);
//		}
//		numOfObj = obj_id;
//		newfile.close(); //close the file object.
//	}
//	else {
//		cerr << "Error Opening File!!" << endl;
//		return;
//	}
//}
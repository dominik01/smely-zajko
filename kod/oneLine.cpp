#include <iostream>
#include "oneLine.h"

using namespace std;

void oneLine::get_info() {
	cout <<id<<": "<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<" "<<dvere<<endl;
}

void oneLine::add_points(int x1, int y1, int x2, int y2) {
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
}
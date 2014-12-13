/* 
 * File:   oneLine.h
 * Author: robotour
 *
 * Created on December 11, 2014, 10:49 AM
 */

#ifndef ONELINE_H
#define	ONELINE_H

using namespace std;

struct oneLine {
	void add_points(int x1, int y1, int x2, int y2);
	void get_info();
	int x1, y1, x2, y2;
	int id;
	string dvere = "None";
        int curve = 0;
};



#endif	/* ONELINE_H */


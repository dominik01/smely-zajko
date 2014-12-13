/* 
 * File:   DataTypes.h
 * Author: myron
 *
 * Created on Štvrtok, 2010, september 2, 14:39
 */

#ifndef DATATYPES_H
#define	DATATYPES_H

#include <vector>

class Ecef{
    double x;
    double y;
    double z;
};

class Lla{
    double latitude;
    double longitude;
    double altitude;
};

class Ll{
public:
    double latitude;
    double longitude;
};

//TODO: get rid of this
class GpsAngles{
public:
    double map;
	double dstToFin;
    //double gps;
};

//class LlAngle: public Ll{
//public:
//    double angle;//loc
//    //double angleGps;
//};


class Point{
public:
    int x;
    int y;
};

#endif	/* DATATYPES_H */

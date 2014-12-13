/* 
 * File:   Localization.h
 * Author: myron
 *
 * Created on Štvrtok, 2010, september 9, 10:17
 */

#ifndef LOCALIZATION_H
#define	LOCALIZATION_H

#include "DataTypes.h"
#include <vector>
#include <map>
#include <cv.h>

using namespace std;

class IdDist{
public:
    double id;
    double dist;
};

class FindOnWay{
public:
    Ll pointFound;
    double pointId1;
    double pointId2;
    int pathId;
    int pathPosition;
};

class WayPoint : public Ll {
public:
    vector<double> nextPoints;

    //for searching
    double dist;
    double previous;

    WayPoint(){
        dist = DBL_MAX;
        previous = -1;
    };
    WayPoint(Ll src){
        dist = DBL_MAX;
        previous = -1;
        this->latitude = src.latitude;
        this->longitude = src.longitude;
    };
};

class Path{
public:
    vector<double> points;
};


class BoundsLl{
public:
    double minlat;
    double minlon;
    double maxlat;
    double maxlon;
};

class LocalizationAndPlaning {
public:
    vector<Path > paths;
    map< double, WayPoint> points;
    BoundsLl bounds;

    int guiWidth;
    int guiHeight;

    //Ll destionation;
    double destId;
    vector<int> way;
    //dint curPath;
    double curPoint;
    double wasCurPoint;

    double lastCurPoint;



    double headingPointId;
    Ll lastPosition;

    LocalizationAndPlaning(int guiWidth, int guiHeight);
    virtual ~LocalizationAndPlaning();

    void readMap(char* filename);

    IplImage* getGui();
    IplImage* getGui(Ll point);

    void setDestination(Ll point);//prvate?
    void readDestination(char* filename);

    GpsAngles update( Ll gps);//TODO: , ImuData imu);

    void findWay(vector<double>* usedPoints, double destPoint, double curPoint, double dist, int level);
    void findWay2(double destPoint, double curPoint);

    //bool distCompare(IdDist i, IdDist j);
    Ll reverse(Point location);

private:
    double dist_point_linesegment(double x, double y, double x1, double y1, double x2, double y2, double *nx, double *ny);
    FindOnWay find_on_way(Ll point);
    double add_point_on_way(Ll point);
    Point convert(Ll point);

    vector<double> bestWay;
    double bestDist;
    
    double distance(Ll p1, Ll p2);
    double distance2(Ll p1, Ll p2);
    double wayDistance(vector<double> w);
    
};

#endif	/* LOCALIZATION_H */


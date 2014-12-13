/* 
 * File:   Jazda.h
 * Author: robotour
 *
 * Created on December 11, 2014, 10:57 AM
 */

#ifndef JAZDA_H
#define	JAZDA_H

#include "oneLine.h"
#include <vector>

using namespace std;

class Jazda {
public:
    vector <oneLine> lines;
    int position [2];
    int zakruta(int s1, int s2, int x, int y, int index);
    int v_strede(int s1, int s2);
    int get_lineId(int x, int y);
    string get_doors(int lineId);
    void vyrovnavaj(int angle);
    void load_map();
    void update_position(int index, int lstep, int rstep);
    Jazda();
    Jazda(const Jazda& orig);
    virtual ~Jazda();
private:

};

#endif	/* JAZDA_H */


/* 
 * File:   Jazda.cpp
 * Author: robotour
 * 
 * Created on December 11, 2014, 10:57 AM
 */

#include <iostream>
#include "Jazda.h"
#include "SbotThread.h"
#include <vector>
#include "oneLine.h"
#include "rapidxml.hpp"
#include "fstream"
#include <math.h>
#include <unistd.h>

using namespace std;
using namespace rapidxml;

SbotThread sbot;

Jazda::Jazda() {
    vector <oneLine> lines;
    int position [2] = {0,0};
}

void Jazda::update_position(int index, int lstep, int rstep){
    double dist;
    dist = (lstep*1.33 + rstep*1.33) / 2;
    position[index] = round(dist);
    //cout << "lstep, rstep" << lstep << " " << rstep << endl;
    //cout << "position: " << position[0] << " " << position[1] << endl;
}

string Jazda::get_doors(int lineId) {
	for (unsigned int i=0; i<lines.size(); i++) {
		if (lines[i].id == lineId) {
			if (lines[i].dvere != "None") {
				return lines[i].dvere;
			}
		}
	}
	return "None";
}

int Jazda::get_lineId(int x, int y) {
	for (unsigned int i=0; i<lines.size(); i++) {
		if (x == lines[i].x2 && x == lines[i].x1) {
			if ((lines[i].y1 <= y && y < lines[i].y2) || (lines[i].y2 < y && y <= lines[i].y1)) {
				return lines[i].id;
			}
		} else if (y == lines[i].y2 && y == lines[i].y1) {
			if ((lines[i].x1 <= x && x < lines[i].x2) ||
					(lines[i].x2 < x && x <= lines[i].x1)) {
				return lines[i].id;
			}
		}
	}
        cout << "CHYBA " << x << " " << y << endl;
	return -1;
}

void Jazda::load_map() {
    int width, height, x1, y1, x2, y2, id;
    string doors;
    xml_document<> doc;
    xml_node<> *root_node;
    ifstream theFile("plan.svg");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]);

    root_node = doc.first_node("svg");
    width = atoi(root_node->first_attribute("width")->value());
    height = atoi(root_node->first_attribute("height")->value());
    for (xml_node<> *line_node = root_node->first_node("line"); line_node; line_node = line_node->next_sibling()) {
                x1 = atoi(line_node->first_attribute("x1")->value());
                y1 = atoi(line_node->first_attribute("y1")->value());
                x2 = atoi(line_node->first_attribute("x2")->value());
                y2 = atoi(line_node->first_attribute("y2")->value());
                id = atoi(line_node->first_attribute("lineId")->value());
                oneLine u;
                u.add_points(x1,y1,x2,y2);
                u.id = id;
                xml_attribute<> *doors = line_node->first_attribute("doors");
                if (doors != 0) {
                        u.dvere = doors->value();
                }
                xml_attribute<> *curve = line_node->first_attribute("curve");
                if (curve != 0) {
                        u.curve = atoi(curve->value());
                }
                lines.push_back(u);
     }
}

int Jazda::v_strede(int s1, int s2) {
    if ((s1 < 60 || s2 < 60)) {
        if (s1 < s2) {
            return 30;
        } else {
            return -30;
        }
    }
    return 0;
}

void Jazda::vyrovnavaj(int angle) {
    cout << "TOCIM SA " << angle << endl;
    sbot.setSpeed(0);   
    sbot.setDirection(angle);
    sleep(1);
    sbot.setSpeed(2);
    sleep(3);
    cout << "VYROVNAVAM " << -angle << endl;
    sbot.setSpeed(0);
    sbot.setDirection(-angle);
    sleep(1);
    sbot.setSpeed(2);
}

int Jazda::zakruta(int s1, int s2, int x, int y, int index) {
	if (s1 >= 200 || s2 >= 200) {
		sbot.setSpeed(0);
		if (lines[get_lineId(x, y)].curve == 1) {
			sbot.setDirection(90);
		}
	}
	if (index) {
		return 0;
	} else {
		return 1;
	}
}

Jazda::Jazda(const Jazda& orig) {
}

Jazda::~Jazda() {
}


//
// Created by donluigimx on 29/11/16.
//

#ifndef _NODE_H
#define _NODE_H

#define MOMBO 0
#define PIROLO 1
#define LUCAS 2

#include <tuple>

class Route;

class MapNode {
public:
    int id;
    int type;

    Route* routes[8] = {};

    MapNode();
    MapNode(int type, int id);

    static std::tuple<int,int> get_coordinates(int x, int y, int position);
};

class Route {
public:
    MapNode* from;
    MapNode* to;

    // Weighing of the actors
    float weighing[3];

    // Effort of the actors to move into the next node
    float effort[3];

    Route(MapNode* from, MapNode* to);
};

#endif //_NODE_H

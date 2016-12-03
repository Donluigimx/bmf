//
// Created by donluigimx on 29/11/16.
//

#include <climits>
#include "Node.h"

MapNode::MapNode() {
    this->type = 0;
}

MapNode::MapNode(int type, int id) {
    this->type = type;
    this->id = id;
}

std::tuple<int, int> MapNode::get_coordinates(int x, int y, int position) {
    switch (position) {
        case 0:
            return std::make_tuple(x-1,y-1);
        case 1:
            return std::make_tuple(x,y-1);
        case 2:
            return std::make_tuple(x+1,y-1);
        case 3:
            return std::make_tuple(x+1,y);
        case 4:
            return std::make_tuple(x+1,y+1);
        case 5:
            return std::make_tuple(x,y+1);
        case 6:
            return std::make_tuple(x-1,y+1);
        case 7:
            return std::make_tuple(x-1,y);
        default:
            return std::make_tuple(-1,-1);
    }
}

Route::Route(MapNode *from, MapNode *to) {
    this->from = from;
    this->to = to;

    this->weighing[MOMBO] = INT_MAX/2;
    this->weighing[PIROLO] = INT_MAX/2;
    this->weighing[LUCAS] = INT_MAX/2;

    this->effort[MOMBO] = 0;
    this->effort[PIROLO] = 0;
    this->effort[LUCAS] = 0;
}

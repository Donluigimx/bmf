//
// Created by donluigimx on 29/11/16.
//

#ifndef MYGAME_BMFSCENE_H
#define MYGAME_BMFSCENE_H

#define MAP_SIZE 10

#define DEFAULT_HILL_PERCENTAGE 15
#define DEFAULT_VALLEY_PERCENTAGE 15
#define DEFAULT_WATER_PERCENTAGE 15
#define DEFAULT_WALL_PERCENTAGE 15

#define PLAIN 0
#define HILL 1
#define VALLEY 2
#define WATER 3
#define WALL 4
#define HOME 5

#define NEIGHBOR_SIZE 8
#define X_SIZE 33
#define Y_SIZE 33

#define NODES_MAP_TAG 1000
#define TRAINING_SIZE 20000
#define MAX_ATTEMPTS 10
#define BLOCKED 2000

#include <vector>
#include <cstring>
#include "cocos2d.h"
#include "Node.h"

class BMFScene : public cocos2d::LayerColor {

public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(BMFScene);

private:
    MapNode* map[MAP_SIZE][MAP_SIZE] = {};
    std::vector<cocos2d::Sprite*> node_sprites;
    std::string sprites_file_name[6]{
            "plain.png",
            "hill.png",
            "valley.png",
            "water.png",
            "wall.png",
            "home.png"
    };

    std::string actors_file_name[3]{
            "mombo_small.png",
            "pirolo_small.png",
            "lucas_small.png"
    };
    float efforts[3][4] = {
            {2.5, 0.3, 1.5, 1},
            {0.3, 2.5, 1, 1.5},
            {1.5, 1, 2.5, 0.3}
    };

    float hill_percentage;
    float valley_percentage;
    float water_percentage;
    float wall_percentage;
    float max_effort[3];
    float min_effort[3];

    int globalID = 0;

    void initMap();
    void exploreRoutes();
    void drawNodes();
    void training(int actor);
    void findRoute(int actor);
    float totalEffort(std::vector<Route*> routes, int actor);
};


#endif //MYGAME_BMFSCENE_H

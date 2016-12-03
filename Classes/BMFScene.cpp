//
// Created by donluigimx on 29/11/16.
//

#include <stdlib.h>
#include <tuple>
#include "BMFScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

Scene *BMFScene::createScene() {
    auto scene = Scene::create();
    auto layer = BMFScene::create();
    scene->addChild(layer);
    return scene;
}

bool BMFScene::init() {
    if ( !LayerColor::initWithColor(Color4B(255,255,255,255)) )
    {
        return false;
    }

    this->setColor(Color3B(200,200,200));
    // Init the percentage of the environment in the map
    this->hill_percentage = DEFAULT_HILL_PERCENTAGE;
    this->valley_percentage = DEFAULT_VALLEY_PERCENTAGE;
    this->water_percentage = DEFAULT_WATER_PERCENTAGE;
    this->wall_percentage = DEFAULT_WALL_PERCENTAGE;

    // Adds sprites of actors in the menu
    auto sprite = Sprite::create("lucas_menu.png");
    sprite->setPosition(Vec2(X_SIZE*MAP_SIZE+180,600));
    this->addChild(sprite,0,LUCAS);

    sprite = Sprite::create("mombo_menu.png");
    sprite->setPosition(Vec2(X_SIZE*MAP_SIZE+180,380));
    this->addChild(sprite,0,MOMBO);

    sprite = Sprite::create("pirolo_menu.png");
    sprite->setPosition(Vec2(X_SIZE*MAP_SIZE+180,160));
    this->addChild(sprite,0,PIROLO);

    auto button = ui::Button::create("iniciar_aprendizaje.png", "iniciar_aprendizaje_pressed.png", "iniciar_aprendizaje.png");
    button->setPosition(Vec2(X_SIZE*MAP_SIZE+180, 480));
    button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type){
       switch (type) {
           case ui::Widget::TouchEventType::BEGAN:
               this->training(LUCAS);
               break;
           default:
               break;
       }
    });
    this->addChild(button);

    button = ui::Button::create("iniciar_aprendizaje.png", "iniciar_aprendizaje_pressed.png", "iniciar_aprendizaje.png");
    button->setPosition(Vec2(X_SIZE*MAP_SIZE+180, 260));
    button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type){
        switch (type) {
            case ui::Widget::TouchEventType::BEGAN:
                this->training(MOMBO);
                break;
            default:
                break;
        }
    });
    this->addChild(button);

    button = ui::Button::create("iniciar_aprendizaje.png", "iniciar_aprendizaje_pressed.png", "iniciar_aprendizaje.png");
    button->setPosition(Vec2(X_SIZE*MAP_SIZE+180, 40));
    button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type){
        switch (type) {
            case ui::Widget::TouchEventType::ENDED:
                this->training(PIROLO);
                break;
            default:
                break;
        }
    });
    this->addChild(button);

    this->initMap();
    this->exploreRoutes();
    this->drawNodes();

    return true;
}

/*
 * Initializes the map making use of the percentage
 * assigned by default or by the user
 */
void BMFScene::initMap() {
    int size = MAP_SIZE*MAP_SIZE;
    int posx = -1;
    int posy = -1;
    CCLOG("initMap started");
    srand(time(0));
    // Hill random assignation
    for (int i = 0; i < (size/100)*this->hill_percentage; i++) {
        while(true) {
            posx = rand() % MAP_SIZE;
            posy = rand() % MAP_SIZE;
            if (this->map[posx][posy] == nullptr) {
                CCLOG("New hill created in %d, %d", posx, posy);
                this->map[posx][posy] = new MapNode(HILL,this->globalID++);
                break;
            }
        }
    }

    // Water random assignation
    for (int i = 0; i < (size/100)*this->water_percentage; i++) {
        while(true) {
            posx = rand() % MAP_SIZE;
            posy = rand() % MAP_SIZE;
            if (this->map[posx][posy] == nullptr) {
                CCLOG("New water created in %d, %d", posx, posy);
                this->map[posx][posy] = new MapNode(WATER,this->globalID++);
                break;
            }
        }
    }

    // Valley random assignation
    for (int i = 0; i < (size/100)*this->valley_percentage; i++) {
        while(true) {
            posx = rand() % MAP_SIZE;
            posy = rand() % MAP_SIZE;
            if (this->map[posx][posy] == nullptr) {
                CCLOG("New valley created in %d, %d", posx, posy);
                this->map[posx][posy] = new MapNode(VALLEY,this->globalID++);
                break;
            }
        }
    }

    // Wall random assignation
    for (int i = 0; i < (size/100)*this->wall_percentage; i++) {
        while(true) {
            posx = rand() % MAP_SIZE;
            posy = rand() % MAP_SIZE;
            if (this->map[posx][posy] == nullptr) {
                CCLOG("New wall created in %d, %d", posx, posy);
                this->map[posx][posy] = new MapNode(WALL, this->globalID++);
                break;
            }
        }
    }

    // Rest of the map assignation
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if (this->map[i][j] == nullptr) {
                CCLOG("New plain created in %d, %d", i, j);
                this->map[i][j] = new MapNode();
            }
        }
    }
    // Generates the home random
    posx = rand() % MAP_SIZE;
    posy = rand() % MAP_SIZE;

    delete this->map[posx][posy];
    this->map[posx][posy] = new MapNode(HOME, this->globalID++);
    CCLOG("New home created in %d, %d, id %d", posx, posy, this->globalID);

    for (int i = 0; i < 3; ++i) {
        this->min_effort[i] = -1;
        this->max_effort[i] = -1;
    }
    CCLOG("initMap finished");
}

// Creates routes between the MapNodes
void BMFScene::exploreRoutes() {
    CCLOG("exploreRoutes started");
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            for (int k = 0; k < NEIGHBOR_SIZE; ++k) {
                //CCLOG("started i = %d, j = %d, k = %d",i,j,k);
                auto tuple = MapNode::get_coordinates(i,j,k);
                // Checks that the coordinates given are'nt lower of 0 or higher of MAP_SIZE
                CCLOG("%d %d to %d %d",i,j,std::get<0>(tuple),std::get<1>(tuple));
                if ( std::get<0>(tuple) >= 0 &&
                     std::get<0>(tuple) < MAP_SIZE &&
                     std::get<1>(tuple) >= 0 &&
                     std::get<1>(tuple) < MAP_SIZE ) {
                    int x = std::get<0>(tuple), y = std::get<1>(tuple);
                    //CCLOG("coordinates x = %d,y = %d",x,y);
                    if (this->map[x][y]->type != WALL) {
                        // Creates a new route in the map
                        CCLOG("New route created %d %d: %d",x,y,k);
                        this->map[i][j]->routes[k] = new Route(this->map[i][j], this->map[x][y]);
                    }
                }
                //CCLOG("finished i = %d, j = %d, k = %d",i,j,k);
            }
        }
    }
    CCLOG("exploreRoutes finished");
}

// Creates Sprites
void BMFScene::drawNodes() {
    int tag = NODES_MAP_TAG;

    CCLOG("drawNodes started");
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            //CCLOG("started i = %d,j = %d, type = %d",i,j,this->map[i][j]->type);
            auto sprite = Sprite::create(this->sprites_file_name[this->map[i][j]->type]);
            sprite->setPosition(Vec2(X_SIZE*(i+1), Y_SIZE*(j+1)));
            this->addChild(sprite, 0, tag++);
            //CCLOG("finished i = %d,j = %d",i,j);
        }
    }
    CCLOG("drawNodes finished");
}

void BMFScene::training(int actor) {
    std::vector<Route*> routes;
    std::vector<MapNode*> neighbors;
    MapNode* mapNode;

    int x = 0, y = 0;
    int next = -1;
    int attempt = 0;
    int blocked = 0;

    CCLOG("Training Started");
    for (int i = 0; i < TRAINING_SIZE; ++i) {
        blocked = 0;
        mapNode = this->map[0][0];
        neighbors.push_back(mapNode);
        CCLOG("Started: %d", i);
        while (mapNode->type != HOME) {
            next = rand() % NEIGHBOR_SIZE;
            attempt++;
            blocked++;
            if (mapNode->routes[next] != nullptr) {
                bool encontrado = false;
                for (int i = 0; i < neighbors.size(); ++i) {
                    if (neighbors[i]->id == mapNode->routes[next]->to->id) {
                        encontrado = true;
                        break;
                    }
                }
                auto res = std::find_if(neighbors.begin(), neighbors.end(), [=](const MapNode* m) {
                    return mapNode->routes[next]->to->id == m->id;
                });
                if (!encontrado) {
                    attempt = -1;
                    mapNode->routes[next]->effort[actor] = efforts[actor][mapNode->type];
                    routes.push_back(mapNode->routes[next]);
                    mapNode = mapNode->routes[next]->to;
                    neighbors.push_back(mapNode);
                }
            }
            if (blocked >= BLOCKED) {
                CCLOG("Desbloqueado");
                routes.clear();
                neighbors.clear();
                blocked = 0;
                mapNode = this->map[0][0];
                neighbors.push_back(mapNode);
                attempt = 0;
            }
            if (attempt == MAX_ATTEMPTS) {
                if (routes.empty())
                    continue;
                mapNode = routes[routes.size()-1]->from;
                routes.pop_back();
                attempt = -1;
            }
        }
        CCLOG("Finished %d", i);
        float effort = this->totalEffort(routes, actor);

        if (this->max_effort[actor] == -1 && this->min_effort[actor] == -1) {
            this->max_effort[actor] = effort;
            this->min_effort[actor] = effort;
        }

        float miu = (this->max_effort[actor] + this->min_effort[actor]) / 2;
        float ajuste = effort - miu;
        for (auto route: routes) {
            route->weighing[actor] -= ajuste;
            CCLOG("%d to %d weigh %f", route->from->id, route->to->id, route->weighing[actor]);
        }
        routes.clear();
        neighbors.clear();
    }

    CCLOG("Training Finished");

    //this->findRoute(actor);
}

void BMFScene::findRoute(int actor) {
    /*auto comp = [](const MapNode* lhs, const MapNode* rhs) {

    };
    std::priority_queue<MapNode*, std::vector<MapNode*>, comp>;*/
    int x = 0, y = 0;
    while(this->map[x][y]->type != HOME) {
        float menor = INT_MAX;
        int menor_pos = -1;
        for (int i = 0; i < 8; ++i) {
            if (this->map[x][y]->routes[i] != nullptr) {
                if (this->map[x][y]->routes[i]->to->type == HOME) {
                    menor_pos = i;
                    break;
                }
                if (this->map[x][y]->routes[i]->weighing[actor] < menor) {
                    menor_pos = i;
                }
            }
        }
        auto sprite = Sprite::create(this->actors_file_name[actor]);
        auto tuple = MapNode::get_coordinates(x,y,menor_pos);

        x = std::get<0>(tuple);
        y = std::get<1>(tuple);

        sprite->setPosition(Vec2(X_SIZE*(x+1), Y_SIZE*(y+1)));

        this->addChild(sprite);
    }
}

float BMFScene::totalEffort(std::vector<Route *> routes, int actor) {
    float total = 0;
    for (auto route: routes) {
        total += route->effort[actor];
    }
    return total*routes.size();
}

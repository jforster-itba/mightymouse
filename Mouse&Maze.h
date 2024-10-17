#ifndef MOUSEANDMAZE_H
#define MOUSEANDMAZE_H

#include <stack>

/*------------------------------Tipos de datos personalizados------------------------------*/
enum Orientation {UP = 1, RIGHT, LEFT, DOWN};

struct Square 
{
    bool wallFront;
    bool wallRight;
    bool wallLeft;
    bool wallBack;
    bool frontChecked;
    bool rightChecked;
    bool backChecked;
    bool leftChecked;
    bool visited;
    char floodValue;
    int visitedNum;
};

struct Mouse 
{
    Orientation mouseOrientation;
    struct {
        char x;
        char y;
    };
    char runNumber;
};

/*------------------------------Prototipos de funciones------------------------------*/
void checkNeighborsAndMove(Square mazeArray[16][16], Mouse& mouse, std::stack<Orientation>& movementsStack);
void floodFillCenter(Square mazeArray[16][16], Mouse& mouse);
void floodFillMouse(Square mazeArray[16][16], Mouse& mouse);
void actualizarParedes(Square mazeArray[16][16], Mouse& mouse);

#endif
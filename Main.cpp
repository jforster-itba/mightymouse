#include <iostream>
#include <string>

#include "API.h"

enum Orientation {UP = 1, RIGHT, LEFT, DOWN};

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

struct Square 
{
    bool wallFront;
    bool wallRight;
    bool wallLeft;
    bool wallBack;
    char floodValue;
    bool marked;
};

struct Mouse 
{
    Orientation mouseOrientation;
    struct {
        char x;
        char y;
    };
};

void checkNeighborsAndMove(Square mazeArray[16][16], Mouse& mouse);
void floodFill(Square mazeArray[16][16]);
void actualizarParedes(Square mazeArray[16][16], Mouse& mouse);

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");

    Square mazeArray[16][16];
    Mouse mouse = {UP,{0,15}};

    for(int i = 0; i <= 15; i++)
    {
        for(int j = 0; j <= 15; j++)
        {
            mazeArray[i][j].wallFront = false;
            mazeArray[i][j].wallRight = false;
            mazeArray[i][j].wallBack = false;
            mazeArray[i][j].wallLeft = false;            
            if((i == 8 && j == 7) || (i == 7 && j == 8) || (i == 8 && j == 8) || (i == 7 && j == 7))
            {
                mazeArray[i][j].floodValue = 0;
            }else
            {
                mazeArray[i][j].floodValue = 100;
            }
        }
    }

    while (true) {
        /*if (!API::wallLeft()) {
            API::turnLeft();
        }
        while (API::wallFront()) {
            API::turnRight();
        }
        API::moveForward();*/

        actualizarParedes(mazeArray,mouse);
        floodFill(mazeArray);
        checkNeighborsAndMove(mazeArray, mouse);
        if(mazeArray[mouse.y][mouse.x].floodValue == 0)
        {
            break;
        }
    }
}

void checkNeighborsAndMove(Square mazeArray[16][16], Mouse& mouse)
{
    int floodAux = 1000;
    Orientation moveDirection = UP;
    if(mouse.y-1 >= 0 && mazeArray[mouse.y-1][mouse.x].floodValue < floodAux) // Chequea arriba
    {
        floodAux = mazeArray[mouse.y-1][mouse.x].floodValue;
    }
    if(mouse.x+1 <= 15 && mazeArray[mouse.y][mouse.x+1].floodValue < floodAux) // Chequea derecha
    {
        floodAux = mazeArray[mouse.y][mouse.x + 1].floodValue;
        moveDirection = RIGHT;
    }
    if(mouse.y+1 <= 15 && mazeArray[mouse.y+1][mouse.x].floodValue < floodAux) // Chequea abajo
    {
        floodAux = mazeArray[mouse.y+1][mouse.x].floodValue;
        moveDirection = DOWN;
    }
    if(mouse.x-1 >= 0 && mazeArray[mouse.y][mouse.x-1].floodValue < floodAux) // Chequea izquierda
    {
        floodAux = mazeArray[mouse.y][mouse.x-1].floodValue;
        moveDirection = LEFT;
    }
    switch (moveDirection)
    {
    case UP:
        switch (mouse.mouseOrientation)
        {
        case UP:
            API::moveForward();
            break;
        case RIGHT:
            API::turnLeft();
            API::moveForward();
            break;
        case DOWN:
            API::turnRight();
            API::turnRight();
            API::moveForward();
            break;
        case LEFT:
            API::turnRight();
            API::moveForward();
            break;
        default:
            break;
        }
        break;
        mouse.y--;
    case RIGHT:
        switch (mouse.mouseOrientation)
        {
        case UP:
            API::turnRight();
            API::moveForward();
            break;
        case RIGHT:
            API::moveForward();
            break;
        case DOWN:
            API::turnLeft();
            API::moveForward();
            break;
        case LEFT:
            API::turnLeft();
            API::turnLeft();
            API::moveForward();
            break;
        default:
            break;
        }
        mouse.x++;
        break;
    case DOWN:
        switch (mouse.mouseOrientation)
        {
        case UP:
            API::turnLeft();
            API::turnLeft();
            API::moveForward();
            break;
        case RIGHT:
            API::turnLeft();
            API::moveForward();
            break;
        case DOWN:
            API::moveForward();
            break;
        case LEFT:
            API::turnLeft();
            API::moveForward();
            break;
        default:
            break;
        }
        mouse.y++;
        break;
    case LEFT:
        switch (mouse.mouseOrientation)
        {
        case UP:
            API::turnLeft();
            API::moveForward();
            break;
        case RIGHT:
            API::turnLeft();
            API::turnLeft();
            API::moveForward();
            break;
        case DOWN:
            API::turnRight();
            API::moveForward();
            break;
        case LEFT:
            API::moveForward();
            break;
        default:
            break;
        }
        break;
        mouse.x--;
    default:
        break;
    }

    mouse.mouseOrientation = moveDirection;
}

void floodFill(Square mazeArray[16][16])
{
    char mov = 1;
    char n = 0;
    while(mov != 0)
    {
        mov = 0;
        for(int i = 0; i < 16; i++)
        {
            for (int j = 0; i < 16; i++)
            {
                if(mazeArray[i][j].floodValue == n)
                {
                    if(i-1 >= 0 && mazeArray[i][j].wallFront == false && mazeArray[i-1][j].floodValue > n)
                    {
                        mazeArray[i-1][j].floodValue = n+1;
                        mov = 1;
                    }
                    if(j+1 <= 15 && mazeArray[i][j].wallRight == false && mazeArray[i][j+1].floodValue > n)
                    {
                        mazeArray[i][j+1].floodValue = n+1;
                        mov = 1;
                    }
                    if(i+1 <= 15 && mazeArray[i][j].wallBack == false && mazeArray[i+1][j].floodValue > n)
                    {
                        mazeArray[i+1][j].floodValue = n+1;
                        mov = 1;
                    }
                    if(j-1 >= 0 && mazeArray[i][j].wallLeft == false && mazeArray[i][j-1].floodValue > n)
                    {
                        mazeArray[i][j-1].floodValue = n+1;
                        mov = 1;
                    }
                }
            }
        }

        n++;

    }
}

void actualizarParedes(Square mazeArray[16][16], Mouse& mouse)
{
    switch (mouse.mouseOrientation)
    {
    case UP:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallFront = true;
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallRight = true;
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallLeft = true;
        }
        break;
    case RIGHT:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallRight = true;
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallBack = true;
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallFront = true;
        }
        break;
    case DOWN:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallBack = true;
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallLeft = true;
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallRight = true;
        }
        break;
    case LEFT:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallLeft = true;   
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallFront = true;
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallBack = true; 
        }
        break;
    default:
        break;
    }
}
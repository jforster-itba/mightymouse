#include <iostream>
#include <string>
#include <stack>

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

void checkNeighborsAndMove(Square mazeArray[16][16], Mouse& mouse);
void floodFill(Square mazeArray[16][16]);
void actualizarParedes(Square mazeArray[16][16], Mouse& mouse);

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");

    Square mazeArray[16][16];
    Mouse mouse = {UP,{0,15},1};
    std::stack<Orientation> movementsStack;

    for(int i = 0; i <= 15; i++)
    {
        for(int j = 0; j <= 15; j++)
        {
            mazeArray[i][j].wallFront = false;
            mazeArray[i][j].wallRight = false;
            mazeArray[i][j].wallBack = false;
            mazeArray[i][j].wallLeft = false;
            mazeArray[i][j].frontChecked = false;
            mazeArray[i][j].rightChecked = false;
            mazeArray[i][j].backChecked = false;
            mazeArray[i][j].leftChecked = false;
            mazeArray[i][j].visited = false;
            mazeArray[i][j].visitedNum = 0;          
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

        actualizarParedes(mazeArray, mouse);
        std::cerr << std::endl;
        checkNeighborsAndMove(mazeArray, mouse);
        if(mazeArray[mouse.y][mouse.x].floodValue == 0)
        {
            break;
        }

    }

    floodFill(mazeArray);

    for(int i = 0; i <= 15; i++)
        {
            for(int j = 0; j <= 15; j++)
            {
                std::cerr << std::to_string(mazeArray[i][j].floodValue) << "\t";
            }
            std::cerr << std::endl;        
        }

    while (true) {
        bool reverseFlood = true;

        API::setColor(mouse.x, 15 - mouse.y, 'G');

        if(reverseFlood)
        {
            mazeArray[mouse.y][mouse.x].visited = true;
            if(mouse.x + 1 <= 15 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y][mouse.x + 1].floodValue 
                && !mazeArray[mouse.y][mouse.x + 1].visited && !mazeArray[mouse.y][mouse.x].wallRight 
                && mazeArray[mouse.y][mouse.x + 1].visitedNum != 0)
            {
                
                mouse.x++;
                movementsStack.push(LEFT);
            }
            else if(mouse.x - 1 >= 0 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y][mouse.x - 1].floodValue 
                && !mazeArray[mouse.y][mouse.x - 1].visited && !mazeArray[mouse.y][mouse.x].wallLeft
                && mazeArray[mouse.y][mouse.x - 1].visitedNum != 0)
            {
                mouse.x--;
                movementsStack.push(RIGHT);
            }
            else if(mouse.y + 1 <= 15 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y + 1][mouse.x].floodValue 
                && !mazeArray[mouse.y + 1][mouse.x].visited && !mazeArray[mouse.y][mouse.x].wallBack
                && mazeArray[mouse.y + 1][mouse.x].visitedNum != 0)
            {
                mouse.y++;
                movementsStack.push(UP);
            }
            else if(mouse.y - 1 >= 0 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y - 1][mouse.x].floodValue 
                && !mazeArray[mouse.y - 1][mouse.x].visited && !mazeArray[mouse.y][mouse.x].wallFront
                && mazeArray[mouse.y - 1][mouse.x].visitedNum != 0)
            {
                mouse.y--;
                movementsStack.push(DOWN);
            }
        }
        else
        {
            switch (movementsStack.top())
            {
            case UP:
                mouse.y--;
                movementsStack.pop();
                break;
            case RIGHT:
                mouse.x++;
                movementsStack.pop();
                break;
            case DOWN:
                mouse.y++;
                movementsStack.pop();
                break;
            case LEFT:
                mouse.x--;
                movementsStack.pop();
                break;
            default:
                break;
            }

            if(mouse.x + 1 <= 15 && !mazeArray[mouse.y][mouse.x].wallRight && !mazeArray[mouse.y][mouse.x + 1].visited)
            {
                reverseFlood = true;
            }
            if(mouse.x - 1 >= 0 && !mazeArray[mouse.y][mouse.x].wallLeft && !mazeArray[mouse.y][mouse.x - 1].visited)
            {
                reverseFlood = true;
            }
            if(mouse.y + 1 <= 15 && !mazeArray[mouse.y][mouse.x].wallBack && !mazeArray[mouse.y + 1][mouse.x].visited)
            {
                reverseFlood = true;
            }
            if(mouse.y - 1 >= 0 && !mazeArray[mouse.y][mouse.x].wallFront && !mazeArray[mouse.y - 1][mouse.x].visited)
            {
                reverseFlood = true;
            }
        }

        if(mazeArray[mouse.y][mouse.x].floodValue == mazeArray[15][0].floodValue && (mouse.x != 0 || mouse.y != 15))
        {
            reverseFlood = false;
        }
        else if(mazeArray[mouse.y][mouse.x].floodValue == mazeArray[15][0].floodValue && mouse.x == 0 && mouse.y == 15)
        {
            break;
        }
        //log("sigo en el loop");
    }
}

void checkNeighborsAndMove(Square mazeArray[16][16], Mouse& mouse)
{
    char floodAux = 100;
    Orientation moveDirection = RIGHT;
    char maxUnknownWalls = -1;
    int visitedMin = 1000;
    bool hasOptions = false;

    log("switch de decision de movimiento");
    switch (mouse.runNumber)
    {
    case 1:
        // Chequea arriba
        if (mouse.y - 1 >= 0 && !mazeArray[mouse.y][mouse.x].wallFront) {
            int unknownWalls =  !mazeArray[mouse.y - 1][mouse.x].frontChecked +
                                !mazeArray[mouse.y - 1][mouse.x].rightChecked +
                                !mazeArray[mouse.y - 1][mouse.x].backChecked +
                                !mazeArray[mouse.y - 1][mouse.x].leftChecked;

            if (unknownWalls > maxUnknownWalls ||
            (unknownWalls == maxUnknownWalls /*&& mazeArray[mouse.y - 1][mouse.x].floodValue <= floodAux*/ && mazeArray[mouse.y - 1][mouse.x].visitedNum <= visitedMin)) {
                floodAux = mazeArray[mouse.y - 1][mouse.x].floodValue;
                maxUnknownWalls = unknownWalls;
                moveDirection = UP;
                visitedMin = mazeArray[mouse.y - 1][mouse.x].visitedNum;
                hasOptions = true;
            }
        }

        // Chequea derecha
        if (mouse.x + 1 <= 15 && !mazeArray[mouse.y][mouse.x].wallRight) {
            int unknownWalls =  !mazeArray[mouse.y][mouse.x + 1].frontChecked +
                                !mazeArray[mouse.y][mouse.x + 1].rightChecked +
                                !mazeArray[mouse.y][mouse.x + 1].backChecked +
                                !mazeArray[mouse.y][mouse.x + 1].leftChecked;

            if (unknownWalls > maxUnknownWalls ||
            (unknownWalls == maxUnknownWalls /*&& mazeArray[mouse.y][mouse.x + 1].floodValue <= floodAux */&& mazeArray[mouse.y][mouse.x + 1].visitedNum <= visitedMin)) {
                floodAux = mazeArray[mouse.y][mouse.x + 1].floodValue;
                maxUnknownWalls = unknownWalls;
                moveDirection = RIGHT;
                visitedMin = mazeArray[mouse.y][mouse.x + 1].visitedNum;
                hasOptions = true;
            }
        }

        // Chequea abajo
        if (mouse.y + 1 <= 15 && !mazeArray[mouse.y][mouse.x].wallBack) {
            int unknownWalls =  !mazeArray[mouse.y + 1][mouse.x].frontChecked +
                                !mazeArray[mouse.y + 1][mouse.x].rightChecked +
                                !mazeArray[mouse.y + 1][mouse.x].backChecked +
                                !mazeArray[mouse.y + 1][mouse.x].leftChecked;

            if (unknownWalls > maxUnknownWalls ||
            (unknownWalls == maxUnknownWalls /*&& mazeArray[mouse.y + 1][mouse.x].floodValue <= floodAux*/ && mazeArray[mouse.y + 1][mouse.x].visitedNum <= visitedMin)) {
                floodAux = mazeArray[mouse.y + 1][mouse.x].floodValue;
                maxUnknownWalls = unknownWalls;
                moveDirection = DOWN;
                visitedMin = mazeArray[mouse.y + 1][mouse.x].visitedNum;
                hasOptions = true;
            }
        }

        // Chequea izquierda
        if (mouse.x - 1 >= 0 && !mazeArray[mouse.y][mouse.x].wallLeft) {
            int unknownWalls =  !mazeArray[mouse.y][mouse.x - 1].frontChecked +
                                !mazeArray[mouse.y][mouse.x - 1].rightChecked +
                                !mazeArray[mouse.y][mouse.x - 1].backChecked +
                                !mazeArray[mouse.y][mouse.x - 1].leftChecked;

            if (unknownWalls > maxUnknownWalls ||
            (unknownWalls == maxUnknownWalls /*&& mazeArray[mouse.y][mouse.x - 1].floodValue <= floodAux*/ && mazeArray[mouse.y][mouse.x - 1].visitedNum <= visitedMin) || 
            (!(unknownWalls >= maxUnknownWalls) && mazeArray[mouse.y][mouse.x - 1].visitedNum <= visitedMin)) {
                floodAux = mazeArray[mouse.y][mouse.x - 1].floodValue;
                maxUnknownWalls = unknownWalls;
                moveDirection = LEFT;
                visitedMin = mazeArray[mouse.y][mouse.x - 1].visitedNum;
                hasOptions = true;
            }
        }
        
        if(!hasOptions){
            if (mouse.y - 1 >= 0 && !mazeArray[mouse.y][mouse.x].wallFront && mazeArray[mouse.y - 1][mouse.x].visitedNum <= visitedMin)
            {
                moveDirection = UP;
                visitedMin = mazeArray[mouse.y - 1][mouse.x].visitedNum;
            }
            if (mouse.y + 1 <= 15 && !mazeArray[mouse.y][mouse.x].wallBack && mazeArray[mouse.y + 1][mouse.x].visitedNum <= visitedMin)
            {
                moveDirection = DOWN;
                visitedMin = mazeArray[mouse.y + 1][mouse.x].visitedNum;
            }
            if (mouse.x - 1 >= 0 && !mazeArray[mouse.y][mouse.x].wallLeft && mazeArray[mouse.y][mouse.x - 1].visitedNum <= visitedMin)
            {
                moveDirection = LEFT;
                visitedMin = mazeArray[mouse.y][mouse.x - 1].visitedNum;
            }
            if (mouse.x + 1 <= 15 && !mazeArray[mouse.y][mouse.x].wallRight && mazeArray[mouse.y][mouse.x + 1].visitedNum <= visitedMin)
            {
                moveDirection = RIGHT;
                visitedMin = mazeArray[mouse.y][mouse.x + 1].visitedNum;
            }
        }

        mazeArray[mouse.y][mouse.x].visitedNum++;

        break;
    case 2:
        if(mouse.y-1 >= 0 && mazeArray[mouse.y-1][mouse.x].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallFront == false
        && mazeArray[mouse.y][mouse.x].frontChecked == false) // Chequea arriba
        {
            floodAux = mazeArray[mouse.y-1][mouse.x].floodValue;
            //mazeArray[mouse.y][mouse.x].frontChecked == true;
        }
        if(mouse.x+1 <= 15 && mazeArray[mouse.y][mouse.x+1].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallRight == false
        && mazeArray[mouse.y][mouse.x].rightChecked == false) // Chequea derecha
        {
            floodAux = mazeArray[mouse.y][mouse.x + 1].floodValue;
            moveDirection = RIGHT;
            //mazeArray[mouse.y][mouse.x].rightChecked == true;
        }
        if(mouse.y+1 <= 15 && mazeArray[mouse.y+1][mouse.x].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallBack == false
        && mazeArray[mouse.y][mouse.x].backChecked == false) // Chequea abajo
        {
            floodAux = mazeArray[mouse.y+1][mouse.x].floodValue;
            moveDirection = DOWN;
            //mazeArray[mouse.y][mouse.x].backChecked == true;
        }
        if(mouse.x-1 >= 0 && mazeArray[mouse.y][mouse.x-1].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallLeft == false
        && mazeArray[mouse.y][mouse.x].leftChecked == false) // Chequea izquierda
        {
            floodAux = mazeArray[mouse.y][mouse.x-1].floodValue;
            moveDirection = LEFT;
            //mazeArray[mouse.y][mouse.x].leftChecked == true;
        }

        break;
    case 3:
        break;
    default:
        break;
    }
    log("salio del switch de decision del movimiento");

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
        mouse.y--;
        break;

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
            API::turnRight();
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
        mouse.x--;
        break;
    default:
        break;
    }

    mouse.mouseOrientation = moveDirection;
    log("salio del switch de movimiento mouse.x = " + std::to_string(mouse.x) + " mouse.y " + std::to_string(mouse.y));
    }

void floodFill(Square mazeArray[16][16])
{
    for(int i = 0; i <= 15; i++)
    {
        for(int j = 0; j <= 15; j++)
        {         
            if((i == 8 && j == 7) || (i == 7 && j == 8) || (i == 8 && j == 8) || (i == 7 && j == 7))
            {
                mazeArray[i][j].floodValue = 0;
            }else
            {
                mazeArray[i][j].floodValue = 100;
            }
        }
    }
    bool mov = true;
    int n = 0;
    while(mov)
    {
        mov = false;
        for(int i = 0; i <= 15; i++)
        {
            for (int j = 0; j <= 15; j++)
            {
                if(mazeArray[i][j].floodValue == n)
                {
                    if(i-1 >= 0 && !mazeArray[i][j].wallFront && mazeArray[i-1][j].floodValue > n && mazeArray[i-1][j].visitedNum != 0)
                    {
                        mazeArray[i-1][j].floodValue = n+1;
                        mov = true;
                    }
                    if(j+1 <= 15 && !mazeArray[i][j].wallRight && mazeArray[i][j+1].floodValue > n && mazeArray[i][j+1].visitedNum != 0)
                    {
                        mazeArray[i][j+1].floodValue = n+1;
                        mov = true;
                    }
                    if(i+1 <= 15 && !mazeArray[i][j].wallBack && mazeArray[i+1][j].floodValue > n && mazeArray[i+1][j].visitedNum != 0)
                    {
                        mazeArray[i+1][j].floodValue = n+1;
                        mov = true;
                    }
                    if(j-1 >= 0 && !mazeArray[i][j].wallLeft && mazeArray[i][j-1].floodValue > n && mazeArray[i][j-1].visitedNum != 0)
                    {
                        mazeArray[i][j-1].floodValue = n+1;
                        mov = true;
                    }
                }
            }
        }

        n++;

    }

    log("salio del while del floodfill");
}

void actualizarParedes(Square mazeArray[16][16], Mouse& mouse)
{

    mazeArray[mouse.y][mouse.x].frontChecked = true;
    mazeArray[mouse.y][mouse.x].rightChecked = true;
    mazeArray[mouse.y][mouse.x].backChecked = true;
    mazeArray[mouse.y][mouse.x].leftChecked = true;

    switch (mouse.mouseOrientation)
    {
    case UP:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallFront = true;
            
            // Marcamos que la pared fue checkeada
            mazeArray[mouse.y][mouse.x].frontChecked = true;
            
            if(mouse.y-1 >= 0)
            {
                mazeArray[mouse.y-1][mouse.x].wallBack = true;
                    mazeArray[mouse.y-1][mouse.x].backChecked = true;
            }

            API::setWall(mouse.x,15 - mouse.y,'n');
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallRight = true;
            mazeArray[mouse.y][mouse.x].rightChecked = true;
            
            if(mouse.x+1 <= 15)
            {
                mazeArray[mouse.y][mouse.x+1].wallLeft = true;
                mazeArray[mouse.y][mouse.x+1].leftChecked = true;
            }

            API::setWall(mouse.x,15-mouse.y,'e');
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallLeft = true;
            mazeArray[mouse.y][mouse.x].leftChecked = true;
            
            if(mouse.x - 1 >= 0)
            {
                mazeArray[mouse.y][mouse.x-1].wallRight = true;
                mazeArray[mouse.y][mouse.x-1].rightChecked = true;
            }

            API::setWall(mouse.x,15-mouse.y,'w');
        }
        break;
    case RIGHT:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallRight = true;        
            mazeArray[mouse.y][mouse.x].rightChecked = true;

            if(mouse.x + 1 <= 15)
            {
                mazeArray[mouse.y][mouse.x+1].wallLeft = true;
                mazeArray[mouse.y][mouse.x+1].leftChecked = true;
            }

            API::setWall(mouse.x,15-mouse.y,'e');
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallBack = true;
            mazeArray[mouse.y][mouse.x].backChecked = true;
            
            if(mouse.y + 1 <= 15)
            {
               mazeArray[mouse.y+1][mouse.x].frontChecked = true; 
               mazeArray[mouse.y+1][mouse.x].wallFront = true;
            }

            API::setWall(mouse.x,15-mouse.y,'s');
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallFront = true;           
            mazeArray[mouse.y][mouse.x].frontChecked = true;
            
            if(mouse.y - 1 >= 0)
            {
                mazeArray[mouse.y-1][mouse.x].wallBack = true;
                mazeArray[mouse.y-1][mouse.x].backChecked = true;
            }

            API::setWall(mouse.x,15-mouse.y,'n');
        }
        break;
    case DOWN:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallBack = true;
            mazeArray[mouse.y][mouse.x].backChecked = true;
            

            if(mouse.y + 1 <= 15)
            {
                mazeArray[mouse.y+1][mouse.x].frontChecked = true;
                mazeArray[mouse.y+1][mouse.x].wallFront = true;
            }

            API::setWall(mouse.x,15-mouse.y,'s');
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallLeft = true; 
            mazeArray[mouse.y][mouse.x].leftChecked = true;
           
            if(mouse.x - 1 >= 0){
                mazeArray[mouse.y][mouse.x-1].wallRight = true;
                mazeArray[mouse.y][mouse.x-1].rightChecked = true;
            }

            API::setWall(mouse.x,15-mouse.y,'w');
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallRight = true;
            mazeArray[mouse.y][mouse.x].rightChecked = true;

            if(mouse.x + 1 <= 15){
                mazeArray[mouse.y][mouse.x+1].wallLeft = true;
                mazeArray[mouse.y][mouse.x+1].leftChecked = true;
            }

            API::setWall(mouse.x,15-mouse.y,'e');
        }
        break;
    case LEFT:
        if(API::wallFront())
        {
            mazeArray[mouse.y][mouse.x].wallLeft = true;  
            mazeArray[mouse.y][mouse.x].leftChecked= true; 
            
            if(mouse.x - 1 >= 0){
                mazeArray[mouse.y][mouse.x-1].rightChecked = true;
                mazeArray[mouse.y][mouse.x-1].wallRight = true; 
            }
            
            API::setWall(mouse.x,15-mouse.y,'w');
        }
        if(API::wallRight())
        {
            mazeArray[mouse.y][mouse.x].wallFront = true;
            mazeArray[mouse.y][mouse.x].frontChecked = true;
            if(mouse.y - 1 >= 0){
                mazeArray[mouse.y-1][mouse.x].wallBack = true;
                mazeArray[mouse.y-1][mouse.x].backChecked = true;
            }

            API::setWall(mouse.x,15-mouse.y,'n');
        }
        if(API::wallLeft())
        {
            mazeArray[mouse.y][mouse.x].wallBack = true; 
            mazeArray[mouse.y][mouse.x].backChecked = true;
            if(mouse.y + 1 <= 15){
                mazeArray[mouse.y+1][mouse.x].wallFront = true; 
                mazeArray[mouse.y+1][mouse.x].frontChecked = true;
            }
            API::setWall(mouse.x,15-mouse.y,'s');
        }
        break;
    default:
        break;
    }

    log("fin de actualizar paredes");
}
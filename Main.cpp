#include <iostream>
#include <string>
#include <stack>
#include <chrono>

#include "API.h"
#include "Mouse&Maze.h"

#define BREAK_NUMBER 256

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

/*------------------------------Main------------------------------*/
int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");

    //Datos
    Square mazeArray[16][16];
    Mouse mouse = {UP,{0,15},1};
    std::stack<Orientation> forwardMovementsStack;
    std::stack<Orientation> reverseMovementStack;
    std::stack<Orientation> reverseMovementStackAux;
    std::stack<Orientation> movementsStack;

    bool reverseFlood = true;
    int nMovements = 0;
    char hasArrivedCenter = 0;

    auto start = std::chrono::high_resolution_clock::now();

    //Seteamos todos los datos del laberinto en su valor default para poder empezar
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
    
    //Algoritmo de exploracion
    while (true) {

        int nVisited = 0;
        
        auto now = std::chrono::high_resolution_clock::now();
        
        actualizarParedes(mazeArray, mouse);
        floodFillCenter(mazeArray,mouse);
        checkNeighborsAndMove(mazeArray, mouse, movementsStack);

        std::chrono::duration<double> elapsed = now - start;

        // Condicion para dejar de explorar: llego a la casilla objetivo
        if(mazeArray[mouse.y][mouse.x].floodValue == 0)
        {
            hasArrivedCenter = 1;
            break;
        } 
        // Condicion para dejar de explorar: 200 casillas exploradas. Lo revisa cada 10 movimientos.
        else if(nMovements%10 == 0) 
        {
            for(int i = 0; i <= 15; i++)
            {
                for(int j = 0; j <= 15; j++)
                {
                    if(mazeArray[i][j].visitedNum >= 1){
                        nVisited++;
                    }
                }
            }
            if (nVisited >= BREAK_NUMBER || elapsed.count() >= 120){
                break;
            }
        }
        // Condicion para dejar de explorar: 2 minutos pasaron desde que empezo la exploracion
        else if(elapsed.count() >= 120)
        {
            break;
        }

        nMovements++;
    }

    char mouseLastX = mouse.x;
    char mouseLastY = mouse.y;

    floodFillMouse(mazeArray, mouse);

    // Busqueda de camino de vuelta a la casilla de inicio
    while (true) {
        API::setColor(mouse.x, 15 - mouse.y, 'G');

        mazeArray[mouse.y][mouse.x].visited = true;

        // prueba para salir de callejon
        int wallsNumber = mazeArray[mouse.y][mouse.x].wallBack + mazeArray[mouse.y][mouse.x].wallFront
                        + mazeArray[mouse.y][mouse.x].wallRight + mazeArray[mouse.y][mouse.x].wallLeft;
        
        if(wallsNumber == 3)
        {
            while(wallsNumber > 1){  
                if(!forwardMovementsStack.empty())
                {
                    switch (forwardMovementsStack.top())
                    {
                    case UP:
                        API::setColor(mouse.x, 15 - mouse.y,'R');
                        mouse.y--;
                        forwardMovementsStack.pop();
                        break;
                    case RIGHT:
                        API::setColor(mouse.x, 15 - mouse.y,'R');
                        mouse.x++;
                        forwardMovementsStack.pop();
                        break;
                    case DOWN:
                        API::setColor(mouse.x, 15 - mouse.y,'R');
                        mouse.y++;
                        forwardMovementsStack.pop();
                        break;
                    case LEFT:
                        API::setColor(mouse.x, 15 - mouse.y,'R');
                        mouse.x--;
                        forwardMovementsStack.pop();
                        break;
                    default:
                        break;
                    }
                }
                wallsNumber = mazeArray[mouse.y][mouse.x].wallBack + mazeArray[mouse.y][mouse.x].wallFront
                            + mazeArray[mouse.y][mouse.x].wallRight + mazeArray[mouse.y][mouse.x].wallLeft;
            }
            // Esta en un casillero donde puede agarrar para otro lado
            reverseFlood = true;
        }
        
        if(reverseFlood)
        {
            if(mouse.x + 1 <= 15 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y][mouse.x + 1].floodValue 
                && !mazeArray[mouse.y][mouse.x + 1].visited && !mazeArray[mouse.y][mouse.x].wallRight 
                && mazeArray[mouse.y][mouse.x + 1].visitedNum != 0)
            {
                mouse.x++;
                forwardMovementsStack.push(LEFT);
            }
            else if(mouse.x - 1 >= 0 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y][mouse.x - 1].floodValue 
                && !mazeArray[mouse.y][mouse.x - 1].visited && !mazeArray[mouse.y][mouse.x].wallLeft
                && mazeArray[mouse.y][mouse.x - 1].visitedNum != 0)
            {
                mouse.x--;
                forwardMovementsStack.push(RIGHT);
            }
            else if(mouse.y + 1 <= 15 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y + 1][mouse.x].floodValue 
                && !mazeArray[mouse.y + 1][mouse.x].visited && !mazeArray[mouse.y][mouse.x].wallBack
                && mazeArray[mouse.y + 1][mouse.x].visitedNum != 0)
            {
                mouse.y++;
                forwardMovementsStack.push(UP);
            }
            else if(mouse.y - 1 >= 0 && mazeArray[mouse.y][mouse.x].floodValue < mazeArray[mouse.y - 1][mouse.x].floodValue 
                && !mazeArray[mouse.y - 1][mouse.x].visited && !mazeArray[mouse.y][mouse.x].wallFront
                && mazeArray[mouse.y - 1][mouse.x].visitedNum != 0)
            {
                mouse.y--;
                forwardMovementsStack.push(DOWN);
            }
            else   // Callejon sin salida
            {
                reverseFlood = false;
            }
        }
        else
        {
            /*
            *   Los movimientos fueron pusheados inversamente a los realizados para que el 
            *   stack contenga los movimientos que tiene que HACER para volver al INICIO.
            */
            switch (forwardMovementsStack.top())
                {
                case UP:
                    API::setColor(mouse.x, 15 - mouse.y, 'B');
                    mouse.y--;
                    forwardMovementsStack.pop();
                    break;
                case RIGHT:
                    API::setColor(mouse.x, 15 - mouse.y, 'B');
                    mouse.x++;
                    forwardMovementsStack.pop();
                    break;
                case DOWN:
                    API::setColor(mouse.x, 15 - mouse.y, 'B');
                    mouse.y++;
                    forwardMovementsStack.pop();
                    break;
                case LEFT:
                    API::setColor(mouse.x, 15 - mouse.y, 'B');
                    mouse.x--;
                    forwardMovementsStack.pop();
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

        if(mazeArray[mouse.y][mouse.x].floodValue >= mazeArray[15][0].floodValue && (mouse.x != 0 || mouse.y != 15))
        {
            // Llego a una casilla distinta de la de inicio, pero con el mismo valor de floodfill =>
            // debo retroceder
            reverseFlood = false;
        }
        else if(mazeArray[mouse.y][mouse.x].floodValue == mazeArray[15][0].floodValue && mouse.x == 0 && mouse.y == 15)
        {
            // Llego al inicio
            break;
        }
    }

    mouse.x = mouseLastX;
    mouse.y = mouseLastY;

    // Da un giro de 180 grados para volver correctamente
    switch (mouse.mouseOrientation)
    {
    case UP:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = DOWN;
        break;
    case RIGHT:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = LEFT;
        break;
    case DOWN:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = UP;
        break;
    case LEFT:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = RIGHT;
        break;
    default:
        break;
    }

    // Invertimos stack de movimientos 
    while(!forwardMovementsStack.empty())
    {
        reverseMovementStack.push(forwardMovementsStack.top());
        reverseMovementStackAux.push(forwardMovementsStack.top());
        forwardMovementsStack.pop();
    }

    // Algoritmo para volver al inicio
    // Como los movimientos se guardan para ir en direccion al centro,
    // Dependiendo la direccion del movimiento guardado, se hara el movimiento contrario.
    while (true) {

        switch (reverseMovementStack.top())
        {
        case DOWN:
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
            mouse.mouseOrientation = UP;
            
        break;

        case LEFT:
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
            mouse.mouseOrientation = RIGHT;
            break;

        case UP:
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
            mouse.mouseOrientation = DOWN;
            break;
            
        case RIGHT:
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
            mouse.mouseOrientation = LEFT;
            break;
        default:
            break;
        }

        reverseMovementStack.pop();

        if(mouse.x == 0 && mouse.y == 15)
        {
            mouse.runNumber++;
            break;
        }
    }

    // Invertimos stack de movimientos 
    while(!reverseMovementStackAux.empty())
    {
        reverseMovementStack.push(reverseMovementStackAux.top());
        reverseMovementStackAux.pop();
    }

    // Da un giro de 180 grados para volver correctamente
    switch (mouse.mouseOrientation)
    {
    case UP:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = DOWN;
        break;
    case RIGHT:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = LEFT;
        break;
    case DOWN:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = UP;
        break;
    case LEFT:
        API::turnLeft();
        API::turnLeft();
        mouse.mouseOrientation = RIGHT;
        break;
    default:
        break;
    }

    // Busqueda del camino mas rapido
    if(hasArrivedCenter)
    {
        while(true)
        {
            switch (reverseMovementStack.top())
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
                    mouse.mouseOrientation = UP;
                    
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
                    mouse.mouseOrientation = RIGHT;
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
                    mouse.mouseOrientation = DOWN;
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
                    mouse.mouseOrientation = LEFT;
                    break;
                default:
                    break;
            }
            reverseMovementStack.pop();

            if(mazeArray[mouse.y][mouse.x].floodValue == 0)
            {
                // Llego al centro
                break;
            }
        }
    }
    else
    {
        while (true) {

            actualizarParedes(mazeArray, mouse);
            floodFillCenter(mazeArray,mouse);
            checkNeighborsAndMove(mazeArray, mouse, movementsStack);
            if(mazeArray[mouse.y][mouse.x].floodValue == 0)
            {
                // Llego al inicio
                break;
            }
            API::setColor(mouse.x, 15 - mouse.y, 'Y');

        }
    }

}


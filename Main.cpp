#include <iostream>
#include <string>
#include <stack>
#include <chrono>

#include "API.h"

#define BREAK_NUMBER 256

enum Orientation {UP = 1, RIGHT, LEFT, DOWN};

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

/*------------------------------Estructuras------------------------------*/
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

        /*// chequeamos si anda el floodfill
        for(int i = 0; i <= 15; i++)
        {
            for(int j = 0; j <= 15; j++)
            {
                std::cerr << std::to_string(mazeArray[i][j].floodValue) << "\t";
            }
            std::cerr << std::endl;        
        }*/
    }

    /*for(int i = 0; i <= 15; i++)
    {
        for(int j = 0; j <= 15; j++)
        {
            std::cerr << std::to_string(mazeArray[i][j].floodValue) << "\t";
        }
        std::cerr << std::endl;        
    }*/

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
            /*log("reverseflood inicio");*/
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
                /*log("RIGHT");*/
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
            /*log("reverseflood fin");*/
        }
        else
        {
            /*log("AAAAAAAAAAAAAAA");*/
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
        /*log("fin while inicio");*/
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

    // Busqueda del camino mas rapido
    if(hasArrivedCenter)
    {
        
        switch (forwardMovementsStack.top())
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

        forwardMovementsStack.pop();
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


/**
 * @brief Algoritmo usado para mover al raton
 *
 * @param mazeArray Mapa del laberinto
 * @param mouse 
 * @param movementsStack guarda los movimientos a realizar luego por el raton
 */
void checkNeighborsAndMove(Square mazeArray[16][16], Mouse& mouse, std::stack<Orientation>& movementsStack)
{
    char floodAux = 101;
    Orientation moveDirection = RIGHT;
    char maxUnknownWalls = -1;
    int visitedMin = 1000;
    bool hasOptions = false;
    int wallsNumber;


    //log("switch de decision de movimiento");
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
            (unknownWalls == maxUnknownWalls && mazeArray[mouse.y - 1][mouse.x].floodValue <= floodAux && mazeArray[mouse.y - 1][mouse.x].visitedNum <= visitedMin)) {
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
            (unknownWalls == maxUnknownWalls && mazeArray[mouse.y][mouse.x + 1].floodValue <= floodAux && mazeArray[mouse.y][mouse.x + 1].visitedNum <= visitedMin)) {
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
            (unknownWalls == maxUnknownWalls && mazeArray[mouse.y + 1][mouse.x].floodValue <= floodAux && mazeArray[mouse.y + 1][mouse.x].visitedNum <= visitedMin)) {
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
            (unknownWalls == maxUnknownWalls && mazeArray[mouse.y][mouse.x - 1].floodValue <= floodAux && mazeArray[mouse.y][mouse.x - 1].visitedNum <= visitedMin) || 
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
    case 3:
        if(mouse.y-1 >= 0 && mazeArray[mouse.y-1][mouse.x].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallFront == false) // Chequea arriba
        {
            floodAux = mazeArray[mouse.y-1][mouse.x].floodValue;
            moveDirection = UP;
            hasOptions = true;
        }
        if(mouse.x+1 <= 15 && mazeArray[mouse.y][mouse.x+1].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallRight == false) // Chequea derecha
        {
            floodAux = mazeArray[mouse.y][mouse.x + 1].floodValue;
            moveDirection = RIGHT;
            hasOptions = true;
        }
        if(mouse.y+1 <= 15 && mazeArray[mouse.y+1][mouse.x].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallBack == false) // Chequea abajo
        {
            floodAux = mazeArray[mouse.y+1][mouse.x].floodValue;
            moveDirection = DOWN;
            hasOptions = true;
        }
        if(mouse.x-1 >= 0 && mazeArray[mouse.y][mouse.x-1].floodValue <= floodAux && mazeArray[mouse.y][mouse.x].wallLeft == false) // Chequea izquierda
        {
            floodAux = mazeArray[mouse.y][mouse.x-1].floodValue;
            moveDirection = LEFT;
            hasOptions = true;
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
    default:
        break;
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
        mouse.y--;
        movementsStack.push(DOWN);
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
        movementsStack.push(LEFT);
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
        movementsStack.push(UP);
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
        movementsStack.push(RIGHT);
        break;
    default:
        break;
    }

    mouse.mouseOrientation = moveDirection;
    //log("salio del switch de movimiento mouse.x = " + std::to_string(mouse.x) + " mouse.y " + std::to_string(mouse.y));
    }

/**
 * @brief Ejecuta el algoritmo floodFill desde el centro y sin importar por donde estuvo el raton
 *
 * @param mazeArray Mapa del laberinto
 * @param mouse 
 */
void floodFillCenter(Square mazeArray[16][16], Mouse& mouse)
{
    bool mov = true;
    int n = 0;

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

    while(mov)
    {
        mov = false;
        for(int i = 0; i <= 15; i++)
        {
            for (int j = 0; j <= 15; j++)
            {
                if(mazeArray[i][j].floodValue == n)
                {
                    if(i-1 >= 0 && !mazeArray[i][j].wallFront && mazeArray[i-1][j].floodValue > n)
                    {
                        mazeArray[i-1][j].floodValue = n+1;
                        mov = true;
                    }
                    if(j+1 <= 15 && !mazeArray[i][j].wallRight && mazeArray[i][j+1].floodValue > n)
                    {
                        mazeArray[i][j+1].floodValue = n+1;
                        mov = true;
                    }
                    if(i+1 <= 15 && !mazeArray[i][j].wallBack && mazeArray[i+1][j].floodValue > n)
                    {
                        mazeArray[i+1][j].floodValue = n+1;
                        mov = true;
                    }
                    if(j-1 >= 0 && !mazeArray[i][j].wallLeft && mazeArray[i][j-1].floodValue > n)
                    {
                        mazeArray[i][j-1].floodValue = n+1;
                        mov = true;
                    }
                }
            }
        }
        n++;
    }
    //log("salio del while del floodfill");
}

/**
 * @brief Registra las paredes que el raton va encontrando
 *
 * @param mazeArray Mapa del laberinto
 * @param mouse 
 */
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

/**
 * @brief Ejecuta el algoritmo floodFill desde la ultima posicion del raton, le importa por donde estuvo el raton
 *
 * @param mazeArray Mapa del laberinto
 * @param mouse 
 */
void floodFillMouse(Square mazeArray[16][16], Mouse& mouse)
{
    bool mov = true;
    int n = 0;

    for(int i = 0; i <= 15; i++)
    {
        for(int j = 0; j <= 15; j++)
        {         
            if(i == mouse.y && mouse.x == j)
            {
                mazeArray[i][j].floodValue = 0;
            }else
            {
                mazeArray[i][j].floodValue = 100;
            }
        }
    }

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
    
}
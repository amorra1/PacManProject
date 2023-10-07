#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "colours.h"
#include <time.h>

#define PACMAN 'P'
#define GHOST 'G'
#define DOT '.'
#define WALL 'W'
#define EMPTY ' '
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'
#define MAPFILE "C:\\Users\\Alex\\CLionProjects\\apsc143project\\map.txt"

//function to determine number of rows in map file + number of rows that will be in the arrays
int numberOfRows(){
    FILE *fmap = fopen(MAPFILE, "r");//open map file to read

    if(fmap == NULL){//output if no map file exists
        printf("Error opening game board.");
        exit(0);
    }
    int c = fgetc(fmap);//retrieve first character from map file
    int numberOfRows = 0;
    //loop to determine the number of rows in the map file
    while (c != EOF) {
        if(c == '\n'){//check if retrieved character is a newline character(that's what creates a new row)
            numberOfRows += 1;
        }
        c = fgetc(fmap);//retrieving next character
    }
    fclose(fmap);//close the map file
    return numberOfRows + 3;//rows in the arrays: 2 extra for outer walls and 1 extra for last line(doesn't have a new line character)
}

//function to determine number of columns in map file + number of columns that will be in the arrays
int numberOfColumns(){
    FILE *fmap = fopen(MAPFILE, "r");
    if(fmap == NULL){//output if no map file exists
        printf("Error opening game board.");
        exit(0);
    }
    int c = fgetc(fmap);//retrieve first character from map file
    int numberOfColumns = 0;

    while (c != '\n') {//loop until new line character is reached(columns end at new line character)
        if(isspace(c) == 0){//if character is not a white space increment # of columns
            numberOfColumns += 1;
        }
        c = fgetc(fmap);
    }
    fclose(fmap);
    return numberOfColumns + 2;//columns in arrays: 2 extra for outer walls
}

//Function to fill the dot and map arrays with the contents of the map file and outerwalls
void populateGrids(char** mapArray, char** dotArray, int numRows, int numColumns){
    FILE *fmap = fopen(MAPFILE, "r");
    printf("File opened\n");

    for(int a = 0; a < numColumns; a++){//loop to add top + bottom layer of outer walls
        mapArray[0][a] = 'W';//first row in map array
        mapArray[numRows-1][a] = 'W';//last row in map array
    }

    for(int b = 1; b < numRows-1; b++) {//nested loop to add outer side walls
        for (int c = 0; c < numColumns; c++) {
            mapArray[b][0] = 'W';//first column in map array
            mapArray[b][numColumns - 1] = 'W';//last column in map array
        }
    }

    int c = fgetc(fmap);//retrieve first character from map file
    int row = 1;
    int column = 1;

    while (c != EOF){//loop to fill the map and dot arrays
        if(isspace(c) == 0){
            if(c == '.'){//if char is a dot map array stays empty
                mapArray[row][column] = EMPTY;
            } else {//any other char goes in map array
                mapArray[row][column] = c;
            }
            if(c == '.'|| c == 'G'){//if char is a dot or ghost dot array has a dot
                dotArray[row][column] = '.';
            } else {//for any other char dot array stays empty
                dotArray[row][column] = EMPTY;
            }
            column += 1;
        }
        if(c == '\n'){//if newline char is reached
            row += 1;//row increments by 1
            column = 1;//column resets to 1
        }
        c = fgetc(fmap);
    }
    fclose(fmap);
    printf("Map initialized\n");
}

//Function to print the pacman game board to the console
void printMap(char** mapArray, char** dotArray, int numRows, int numColumns){

    //nested loop to go through the arrays and print the game board
    for(int d = 0; d < numRows; d++){
        for(int e = 0; e < numColumns; e++){
            if(mapArray[d][e] != EMPTY){//if index in map array is not empty(not a dot) print from map array
                if(mapArray[d][e] == WALL){//print walls
                    colourChange(BLUE);//make the walls 'W' blue
                    printf("%c  ", mapArray[d][e]);
                }
                if(mapArray[d][e] == PACMAN){//print pacman
                    colourChange(YELLOW);//make pacman yellow
                    printf("%c  ", mapArray[d][e]);
                }
                if(mapArray[d][e] == GHOST){//print ghosts
                    colourChange(PINK);//make the ghosts pink
                    printf("%c  ", mapArray[d][e]);
                }
            }else{//if index in map array is empty(dot) print from dot array
                colourChange(WHITE);//make the dots white
                printf("%c  ", dotArray[d][e]);//print dots
            }

        }
        printf("\n");//printing newline character to create a square game board
    }
}

//Function to check if the game has been won and all the dots have been eaten
int winCheck(char** dotArray, int numRows, int numColumns){
    int dotCounter = 0;
    //nested loop through dot array and check if there are any dots
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numColumns; j++){
            if(dotArray[i][j] == '.'){//add to dot counter if dot is found
                dotCounter += 1;
            }
        }

    }
    if(dotCounter == 0){//if number of dots that remains is zero the game has been won
        return 1;//win condition comes back true
    }
    return 0;//win condition comes back false
}

//Function to check if the game has been lost and the ghost has eaten pacman
int loseCheck(char** mapArray, int numRows, int numColumns){
    int pacmanCounter = 0;
    //nested loop to go through the map array and determine if pacman is still there
    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numColumns; j++){
            if(mapArray[i][j] == PACMAN){
                pacmanCounter += 1;
            }
        }
    }
    if(pacmanCounter == 0){//if the pacman counter remains is zero the game has been lost
        return 1;//lose condition comes back true
    }
    return 0;//lose condition comes back false
}

//Function to check if a certain index in the map array is a wall
int isWall(char** mapArray, int row, int column) {
    if (mapArray[row][column] == 'W') {
        return 1;//if the map array index is a wall, wall condition is true
    } else {
        return 0;//if the map array index is not a wall, wall condition is false
    }
}

//Function to generate a random number
int randomNumber(int upper){
    return rand() % upper;//finds random number between 1 and upper limit
}

//Function to move the ghosts
void moveGhost(char** mapArray, int numRows, int numColumns, int row, int column){

    int pacmanPosition[2] = {0, 0};//declaring an empty array to store position of pacman

    bool pacmanFound = false; //declaring bool for if pacman was found and initializing to false

    //nested loop to scan for pacman and store it's x and y coordinates in the pacmanPosition array
    for (int i = 0; i < numRows; i++) { //scan through the rows
        for (int j = 0; j < numColumns; j++) { //scan through the columns
            if (mapArray[i][j] == PACMAN) {
                pacmanFound = true; //pacmanFound is set to true
                pacmanPosition[0] = i; //stores pacman column position
                pacmanPosition[1] = j; //stored pacman row position
            }
        }
    }
    bool moved = false; //declaring bool for if the ghost is moved and initializing to false

    //code for ghost to chase pacman
    if (pacmanFound == true && row == pacmanPosition[0] && column < pacmanPosition[1]) { //check if pacman was found + to right of ghost
        if (isWall(mapArray, row, column + 1) == 0) { //check if there is a wall to right of ghost
            mapArray[row][column + 1] = GHOST;//move ghost right
            mapArray[row][column] = EMPTY; //leaving the tile behind as an empty space in mapArray
            moved = true; //moved is set to true
        }
    } else if (pacmanFound == true && row == pacmanPosition[0] && column > pacmanPosition[1]) { //check if pacman was found + to left of ghost
        if (isWall(mapArray, row, column - 1) == 0) { //checking if there is a wall to the left of ghost
            mapArray[row][column - 1] = GHOST;//move ghost left
            mapArray[row][column] = EMPTY;
            moved = true;
        }
    } else if (pacmanFound == true && column == pacmanPosition[1] && row < pacmanPosition[0]) {//check if pacman was found + below ghost
        if (isWall(mapArray, row + 1, column) == 0) {//check if there is a wall below ghost
            mapArray[row + 1][column] = GHOST;//move ghost down
            mapArray[row][column] = EMPTY;
            moved = true;
        }
    } else if (pacmanFound == true && column == pacmanPosition[1] && row > pacmanPosition[0]) {//check if pacman was found + above ghost
        if (isWall(mapArray, row - 1, column) == 0) {//check if there is a wall above ghost
            mapArray[row - 1][column] = GHOST;//move ghost up
            mapArray[row][column] = EMPTY;
            moved = true;
        }
    }

    //random ghost movement code
    while(moved == false){//while the ghost was not moved by chasing pacman, it will move randomly
        switch (randomNumber(5)) {//switch with the randomNumber function as a parameter
            case 1://up movement
                if (isWall(mapArray, row - 1, column) == 0) {
                    mapArray[row - 1][column] = GHOST;//move ghost up
                    mapArray[row][column] = EMPTY; //leaving the tile behind as an empty space in mapArray
                    moved = true; //moved is set to true
                    break;
                }
            case 2://left movement
                if (isWall(mapArray, row, column - 1) == 0) {
                    mapArray[row][column - 1] = GHOST;//move ghost left
                    mapArray[row][column] = EMPTY;
                    moved = true;
                    break;
                }
            case 3://down movement
                if (isWall(mapArray, row + 1, column) == 0) {
                    mapArray[row + 1][column] = GHOST;//move ghost down
                    mapArray[row][column] = EMPTY;
                    moved = true;
                    break;
                }
            case 4://right movement
                if (isWall(mapArray, row, column + 1) == 0) {
                    mapArray[row][column + 1] = GHOST;//move ghost right
                    mapArray[row][column] = EMPTY;
                    moved = true;
                    break;
                }
        }
    }
}

//Function to move pacman and run the flow of the game
void runGame(char** mapArray, char** dotArray, int numRows, int numColumns){

    printMap(mapArray, dotArray, numRows, numColumns);//print the game board after every move
    if(winCheck(dotArray, numRows, numColumns) == 1){//call win check
        printf("Congratulations! You win! Press any key to exit the game\n");
        fflush(stdin);//clearing the output stream
        char userInput = getch();//get user input then exit (working on a Mac so the getch() may not work)
        exit(0);
    }
    if (loseCheck(mapArray,numRows, numColumns) == 1){//call lose check
        printf("Sorry you lose. Press any key to exit the game");
        fflush(stdin);
        getch();//get user input then exit (working on a Mac so the getch() may not work)
        exit(0);
    }

    fflush(stdin);
    char userInput = getch();//getting the user input for pacman movement

    int pacmanPosition[2] = {0,0};//array to store pacman's position on game board

    //nested loop to determine the position of pacman
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numColumns; j++) {
            if(mapArray[i][j] == PACMAN){
                pacmanPosition[0] = i;//storing the column in above array
                pacmanPosition[1] = j;//storing row in above array
            }
        }
    }

    switch (userInput) {//switch case statement that takes userInput as parameter and moves pacman
        case 'w'://up movement
            if(isWall(mapArray, pacmanPosition[0] -1 , pacmanPosition[1]) == 0){//calling isWall to check if there is a wall above pacman
                if(mapArray[pacmanPosition[0] - 1][pacmanPosition[1]] != GHOST){//if there is no ghost in way move pacman up
                    mapArray[pacmanPosition[0] - 1][pacmanPosition[1]] = PACMAN;
                }
                mapArray[pacmanPosition[0]][pacmanPosition[1]] = EMPTY;//clear map array where pacman was
                dotArray[pacmanPosition[0] - 1][pacmanPosition[1]] = EMPTY;//clear dot array where pacman will move to
            }
            break;
        case 'a'://left movement
            if(isWall(mapArray, pacmanPosition[0], pacmanPosition[1]-1)== 0){//calling isWall to check if there is a wall to left of pacman
                if(mapArray[pacmanPosition[0]][pacmanPosition[1] - 1] != GHOST) {//if there is no ghost in way move pacman left
                    mapArray[pacmanPosition[0]][pacmanPosition[1] - 1] = PACMAN;
                }
                mapArray[pacmanPosition[0]][pacmanPosition[1]] = EMPTY;
                dotArray[pacmanPosition[0]][pacmanPosition[1] - 1] = EMPTY;
            }
            break;
        case 's'://down movement
            if(isWall(mapArray, pacmanPosition[0] + 1, pacmanPosition[1])== 0){//calling isWall to check if there is a wall below pacman
                if(mapArray[pacmanPosition[0] + 1][pacmanPosition[1]] != GHOST) {//if there is no ghost in way move pacman down
                    mapArray[pacmanPosition[0] + 1][pacmanPosition[1]] = PACMAN;
                }
                mapArray[pacmanPosition[0]][pacmanPosition[1]] = EMPTY;
                dotArray[pacmanPosition[0] + 1][pacmanPosition[1]] = EMPTY;
            }
            break;
        case 'd'://right movement
            if(isWall(mapArray, pacmanPosition[0], pacmanPosition[1] + 1)== 0) {//calling isWall to check if there is a wall to right of pacman
                if(mapArray[pacmanPosition[0]][pacmanPosition[1]+1] != GHOST) {//if there is no ghost in way move pacman uright
                    mapArray[pacmanPosition[0]][pacmanPosition[1]+1] = PACMAN;
                }
                mapArray[pacmanPosition[0]][pacmanPosition[1]] = EMPTY;
                dotArray[pacmanPosition[0]][pacmanPosition[1] + 1] = EMPTY;
            }
            break;
        default:
            break;
    }

    //arrays to store the ghost positions on game board
    int ghost1Position[2] = {0, 0};
    int ghost2Position[2] = {0, 0};

    int ghostCounter = 0;

    //nested loop to determine ghost positions
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numColumns; j++) {
            if (mapArray[i][j] == GHOST) {
                ghostCounter += 1;
                if (ghostCounter == 1) {
                    ghost1Position[0] = i;//storing ghost 1 column
                    ghost1Position[1] = j;//storing ghost 2 row
                }
                if (ghostCounter == 2) {
                    ghost2Position[0] = i;//storing ghost 2 row
                    ghost2Position[1] = j;//storing ghost 2 column
                }
            }
        }
    }
    if(ghostCounter == 1){//if the ghosts are on top of each other/at the same position
        ghost2Position[0] = ghost1Position[0];//set ghost 2 row to row of ghost 1
        ghost2Position[1] = ghost1Position[1];//set ghost 2 column to column of ghost 1
    }

    moveGhost(mapArray, numRows, numColumns, ghost1Position[0], ghost1Position[1]);//move ghost 1
    moveGhost(mapArray, numRows, numColumns, ghost2Position[0], ghost2Position[1]);//move ghost 2

    runGame(mapArray, dotArray, numRows, numColumns);//continue to run the game until the win / lose checks are met
}

int main() {
    //declaring variable for the number of rows and columns to be in the rays
    int numRows = numberOfRows();//calling number of rows function
    int numColumns = numberOfColumns();//calling number of columns function

    //initializing the rows in map and dot arrays
    char** mapArray = (char**)malloc(numRows * sizeof(char*));
    char** dotArray = (char**)malloc(numRows * sizeof(char*));

    //loop to initialize the columns within the dot and map arrays
    for(int i = 0; i < numRows; i++){
        mapArray[i] = (char*)malloc(numColumns * sizeof(char*));
        dotArray[i] = (char*)malloc(numColumns * sizeof(char*));
    }
    printf("Array Allocated\n");

    //calling the function to fill the dot and map arrays
    populateGrids(mapArray, dotArray, numRows, numColumns);

    //calling the function to run the game and make it playable for the user
    runGame(mapArray, dotArray, numRows, numColumns);

    //loop to free the inner components of the arrays
    for(int j = 0; j < numRows; j++){
        free(mapArray[j]);
        free(dotArray[j]);
    }

    //statements to free the entire dot and map arrays
    free(mapArray);
    free(dotArray);

    return 0;
}
//We attest to the originality of our work
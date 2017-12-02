

/////////
//GLOBALS
const int timePeriod = 100;
const int BLUE = 1;
const int RED = 2;
const int BOTH = 3;
const byte latchPin = 8; //ST_CP
const byte clockPin = 12; //SH_CP
const byte dataPin = 11;
const byte pot1 = A0;
const byte pot2 = A1;
unsigned long time;

const int rows = 6;
const int columns = 7;
//SETTING THE INITIAL BOARD STATE
int board[rows][columns] = {
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0},
};

int blue[rows];
int red[rows];

///////////
//FUNCTIONS


//THIS ARRAY CONVERTS THE 2D BOARD ARRAY TO BINARY NUMBERS THAT
//WILL BE PUSHED TO SHIFT REGISTERS
int arrayToBin(int a[], int marker, int marker2){
  //USE LSBFIRST
  int d = B11111111;
  int e = B10000000;
  for(int i=0; i<8; i++){
    if((a[i] == marker || a[i] == marker2) && i<columns){ 
      d = (d ^ (e>>i));     
    }
  }
  
  return d;
}

//UPDATES THE BLUE AND RED ARRAY WITH A BYTE REPRESENTING THE BINARY STATE OF THE ARRAY
void updatePins(){
  for(int i=0; i<rows; i++){
    blue[i] = arrayToBin(board[i], BLUE, BOTH);
    red[i] = arrayToBin(board[i], RED, BOTH);
    
  }
}

//ADD THE PLAYERS MOVE TO THE HIGHEST AVAILABLE POSITION IN THE COLUMN THEY PLAYED IN
void addToBoard(int c, int marker){
  bool changed = false;

  for(int j = 0; j < rows; j ++){
    if(board[j][c] != 0 && changed == false){
      board[j-1][c] = marker;
      changed = true;
    }
  }
  //IF NO CELL IN THE COLUMN HAS BEEN PLAYED YET, THE BOTTOM SQUARE MUST BE PLAYED
  if(!changed){
    board[5][c] = marker;
  }
}


//CLEARS THE BOARD
void flushScreen(){
  
  for(int i = 0; i < rows; i ++){
    for(int j = 0; j < columns; j ++){
     
      board[i][j] = 0;
    }
  } 
  updatePins();
  drawBoard();
}


//WHEN THE PLAYER WINS THEIR WINNING COMBINATION WILL BE PURPLE
void winLED(){

    board[0][1] = 3;
    board[1][1] = 3;
    board[2][1] = 3;

    board[0][5] = 3;
    board[1][5] = 3;
    board[2][5] = 3;

    board[4][0] = 3;
    board[5][1] = 3;
    board[5][2] = 3;
    board[5][3] = 3;
    board[5][4] = 3;
    board[5][5] = 3;
    board[4][6] = 3;
//    //THIS INDICATES THE WINNING COMBINATION IS A ROW
//    if(y2 == y1){
//      board[y1][x1] = 3;
//      board[y2][x2] = 3;
//
//      if(x1 < x2){
//        board[y1][x1 + 1] = 3;
//        board[y1][x1 + 2] = 3;
//      }
//      else{
//        board[y1][x2 + 1] = 3;
//        board[y1][x2 + 2] = 3;
//        
//      }
//    }
//    //THIS INDICATES THE WINNING COMBINATION IS A COLUMN
//    else if(x2 == x1){
//      board[y1][x1] = 3;
//      board[y2][x2] = 3;
//
//      if(y1 < y2){
//        board[y1 + 1][x1] = 3;
//        board[y1 + 2][x1] = 3;
//      }
//      else{
//        board[y2 + 1][x2] = 3;
//        board[y2 + 2][x2] = 3;
//        
//      }
//    }
//    //THIS INDICATES THE WINNING COMBINATION IS A DIAGONAL
//    else{
//      board[y1][x1] = 3;
//      board[y2][x2] = 3;
//
//      
//      //THE FOLLOWING IS CHECKING FOR DIFFERENT POSSIBLE TYPES OF DIAGONALS
//      
//      if(x1 < x2){
//        if(y1 < y2){
//          board[x1 + 1][y1 + 1] = 3;
//          board[x1 + 2][y1 + 2] = 3;
//        }
//
//        else{
//          board[x1 + 1][y1 -1] = 3;
//          board[x1 + 2][y1 - 2] = 3;
//        }
//      }
//      else{
//        if(y2 < y1){
//          board[x2 + 1][y2 + 1] = 3;
//          board[x2 + 2][y2 + 2] = 3;
//        }
//
//        else{
//          board[x2 + 1][y2 -1] = 3;
//          board[x2 + 2][y2 - 2] = 3;
//        }
//      }
//    }
}


//PUSHES BINARY VALUES TO THE SHIFT REGISTERS TO DISPLAY THE BOARD ON THE MATRIX
void drawBoard(){
  int gnd;
  
  for(int i=0; i<rows; i++){
    
    gnd = B10000000 >> i;
    gnd = gnd ^ B00000000;
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, gnd);
    shiftOut(dataPin, clockPin, LSBFIRST, red[i]);
    shiftOut(dataPin, clockPin, LSBFIRST, blue[i]);
    digitalWrite(latchPin, HIGH);
    delay(1);
  }
}
//INITIAL SETUP
void setup() {
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  updatePins();
  }

//MAIN LOOP
void loop() {
  //ONLY BEGIN AFTER THE FIRST MOVE
  
  if(Serial.available() > 0){
    drawBoard();
    int x = Serial.parseInt();  
    drawBoard();
    //THESE TWO NUMBERS ARE HIGHER THAN ANY POSSIBLE MOVE SO THEY INDICATE SPECIAL CASES
    
    //IF X IS 63 THE FOLLOWING TWO NUMBERS WILL BE THE EDGE COORDINATES OF THE WINNING CONNECTION
    if(x == 63){

      for(int i = 0; i < 500; i++){
        drawBoard();
      }
      flushScreen();
      winLED();
      updatePins();
      drawBoard();
      /*
      while(Serial.available() == 0){
        
      }
      
      int c1 = Serial.parseInt();

      if(c1 % 10 != 0 || (int)c1/10 != 0){
        c1 = c1 - 1;
      }
      while(Serial.available() == 0){
        
      }
      int c2 = Serial.parseInt();
      if(c2 % 10 != 0 || (int)c2/10 != 0){
        c2 = c2 - 1;
      }
      winLED((int)(c1 / 10),(int)(c1 % 10),(int)(c2 / 10),(int)(c2 % 10));
      */
      updatePins();
      for(int i = 0; i < 500; i++){
        drawBoard();
      }
      flushScreen();
    }

    //IF THE CODE RECIEVES A 64 THEN THE BOARD HAS TO BE RESET
    //else if(x == 64){
    //  flushScreen();
    //}

    //THIS IS A PLAYER OR AI MAKING A MOVE
    else{
      int y = x /10;
      int z = x %10;

      addToBoard(y,z);
      updatePins();
      drawBoard();
      Serial.println(board[5][6]);
      
    }
  }
  
  //DRAW THE BOARD
  drawBoard();
}

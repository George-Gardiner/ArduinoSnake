/*
Requires Analog Stick and 8X8 pixel array modules
*/

#include "LedControl.h"

enum direction {
  Up,
  Down,
  Left,
  Right,
  None
};


LedControl lc = LedControl(12, 10, 11, 1);
int joyStickPinX = A5;
int joyStickPinY = A4;
int snake[64][2] = { { 3, 3 }, { 3, 4 }, { 3, 5 } };
int length = 3;
int tailInd = 0;  // next dot in the snake to be moved
int apple[2] = { 5, 5 };
int speed = 500;


enum direction getJoyStickIn() {
  int inX = analogRead(joyStickPinX) - 512;
  int inY = analogRead(joyStickPinY) - 512;

  if (inX < 20 && inX > -20 && inY < 20 && inY > -20) {
    return None;
  }
  if (inX < inY) {
    if (inX + inY < 0) {
      return Down;
    }
    return Right;
  }
  if (inX + inY < 0) {
    return Left;
  }
  return Up;
}


enum direction dir = Right;


void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 15);
  lc.clearDisplay(0);

  randomSeed(analogRead(A0));

  placeApple();

  pinMode(joyStickPinX, INPUT);
  pinMode(joyStickPinY, INPUT);
  snake[0][0] = 3;
  snake[0][1] = 3;
  snake[1][0] = 3;
  snake[1][1] = 4;
  snake[2][0] = 3;
  snake[2][1] = 5;
}

void gameOver() {  // temporary
  while (true) {
    delay(1000000);
  }
}

void placeApple() {
  int lastPos[2];
  lastPos[0] = apple[0];
  lastPos[1] = apple[1];
  while (true) {
    apple[0] = random(0, 8);
    apple[1] = random(0, 8);
    bool works = true;
    if (apple[0] == lastPos[0] && apple[1] == lastPos[1]) {
      works = false;
    }
    for (int i = 0; i < length; i++) {
      if (snake[i][0] == apple[0] && snake[i][1] == apple[1]) {
        works = false;
      }
    }
    if (works) {
      return;
    }
  }
}

void loop() {
  // setup
  int headInd = (tailInd + length - 1) % length;

  //handle input
  enum direction inp = getJoyStickIn();
  if (inp != None) {
    dir = inp;
  }



  // collisions
  int nextPos[2];
  nextPos[0] = snake[headInd][0];
  nextPos[1] = snake[headInd][1];
  if (dir == Up) {
    nextPos[1]++;
  } else if (dir == Down) {
    nextPos[1]--;
  } else if (dir == Left) {
    nextPos[0]--;
  } else if (dir == Right) {
    nextPos[0]++;
  }


  // border
  if (nextPos[0] < 0 || nextPos[1] < 0 || nextPos[0] > 7 || nextPos[1] > 7) {
    gameOver();
  }

  // self collision
  for (int i = 0; i < length; i++) {
    if (nextPos[0] == snake[i][0] && nextPos[1] == snake[i][1]) {
      gameOver();
    }
  }

  // apple collision
  if (nextPos[0] == apple[0] && nextPos[1] == apple[1]) {
    //speed up
    speed *= 0.9;

    // extend snake
    // move all elements after and including the tail right one
    for (int i = length-1; i >= tailInd; i--) {
      snake[i+1][0] = snake[i][0];
      snake[i+1][1] = snake[i][1];
    }


    // insert head
    snake[tailInd][0] = apple[0];
    snake[tailInd][1] = apple[1];
    tailInd++;
    length++;
    headInd = (tailInd + length - 1) % length;
    

    // move apple
    placeApple();
  } else {
    // move snake
    snake[tailInd][0] = nextPos[0];
    snake[tailInd][1] = nextPos[1];
    tailInd = ++tailInd % length;
  }

  // draw
  lc.clearDisplay(0);
  for (int i = 0; i < length; i++) {
    lc.setLed(0, snake[i][0], snake[i][1], true);
  }

  // draw apple
  lc.setLed(0, apple[0], apple[1], true);

  delay(speed);
}

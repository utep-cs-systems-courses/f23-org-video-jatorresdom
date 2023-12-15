#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include <stdio.h>
#include <stdlib.h>
#include "switches.h"

// WARNING: LCD DISPLAY USES P1.0.  Do not touch!!! 

#define LED BIT6		/* note that bit zero req'd for display */
#define NUM_SWITCHES 4

//Constants
#define width 120
#define height 160
#define MAX_TAIL_LENGTH 35

//Redraw Screen
short redrawScreen = 1;
char scoreString[20];

//Array to hold the state of each switch
int* states;

// Game Values
int gameState = 1;
int gameOver,snakex,snakey,fruitx,fruity,flag,score,snake2x,snake2y,tailLength;
int highScore = 0;

struct Point {

  int tailx;
  int taily;

};

struct Point tail[MAX_TAIL_LENGTH];

int randomNumber(){
  return 5 * (2 + rand() % 19);
}
void fruitValues(){
 labe1:
  fruitx = randomNumber();
  if (fruitx == 0)
    goto labe1;
 labe2:
  fruity = randomNumber();
  if (fruity == 0)
    goto labe2;
}

void setup(){  
  gameOver = 0;
  flag = 4;
  score = 0;
  // Stores height and width
  snake2x = 0;
  snake2y = 0;
  snakex = (width / 2) - 5;
  snakey = (height / 2) - 5;
  fruitValues();
  // Initialize the tail
  tail[0].tailx = snakex;
  tail[0].taily = snakey;
  tailLength = 1;
  
}

void updateTail() {

  for (int i = tailLength - 1; i > 0; i--) {
    tail[i] = tail[i - 1];
  }
  tail[0].tailx = snakex;
  tail[0].taily = snakey;
}

void growTail() {
  if (tailLength < MAX_TAIL_LENGTH) {
    tailLength++;
  }
}

void direction(){
  if(states[0]){
    flag = 1;
  }
  else if(states[1]){
    flag = 2;
  }
  else if(states[2]){
    flag = 3;
  }
  else if(states[3]){
    flag = 4;
  }
  set_switches_states();

  switch(flag){
  case 1:
    snakey -= 5;
    snake2y += 5;
    break;
  case 2:
    snakex -= 5;
    snake2x += 5;
    break;
  case 3:
    snakex += 5;
    snake2x -= 5;
    break;
  case 4:
    snakey += 5;
    snake2y -= 5;
    break;
  }
}

void drawSnake() {
  // Draw the head
  fillRectangle(snakex, snakey, 5, 5, COLOR_GREEN);
  // Draw the tail
  for (int i = 0; i < tailLength; i++) {
    fillRectangle(tail[i].tailx, tail[i].taily, 5, 5, COLOR_GREEN);
  }
}

void updateGame(){
  clearScreen(COLOR_BLACK);
  drawSnake();
  fillRectangle(fruitx,fruity,5,5,COLOR_GREEN);
}

void addScore(){
  score++;
}

int checkCollision() {

  // Check collision with walls
  if(snakey > (height - 5) || snakex > (width - 5) || snakey < 0 || snakex < 0 || score == 30){
    return 1;
  }

  // Check collision with the tail
  for (int i = 1; i < tailLength; i++) {
    if (snakex == tail[i].tailx && snakey == tail[i].taily) {
      return 1;  // Collision detected
    }

  }

  return 0;  // No collision

}

void wdt_c_handler(){
  static int secondCount = 0;
  states = get_switches_states();

  if(gameState == 1){
    secondCount++;
    if(secondCount > 250){
      if(states[0] || states[1] || states[2] || states[3]){
	set_switches_states();
	gameState = 2;
	redrawScreen = 1;
	P1OUT |= LED;
	and_sr(0x10);
      }
    }
  }
  else if(gameState == 2){
    secondCount++;
    if(secondCount > 25){
      secondCount = 0;
      updateGame();
      direction();
      updateTail();
      if(snakex == fruitx && snakey == fruity){
	fruitValues();
	addScore();
	growTail();
      }
      gameOver = checkCollision();
      if(gameOver){
	set_switches_states();
	secondCount = 0;
      }
    }
  }
  else if(gameState == 3){
    secondCount++;
    if(secondCount > 250){ 
      if(states[0] || states[1] || states[2] || states[3]){
	gameState = 1;
	redrawScreen = 1;
	secondCount = 0;
	set_switches_states();
	setup();
	P1OUT |= LED;
	and_sr(0x10);
      }
    }
  }
  
}

// Function to display the start screen

void displayStartScreen() {

  // Coordinates for the the middle screen
  u_char x = screenWidth / 2 - 30; // Adjust X as needed
  u_char y = screenHeight / 2;     // Center Y
  // Draw the title and image
  draw_ascii();
  drawString5x7(x, screenHeight - 16, "Snake Game", COLOR_GREEN, COLOR_BLACK);;
  // Draw the start prompt
  y += 10; // Move down for the next text
  x -= 15;
  drawString5x7(x, screenHeight - 8, "Press Any Button", COLOR_WHITE, COLOR_BLACK);
}

// Function to display the start screen

void displayEndScreen() {

  // Coordinates for the the middle screen
  u_char x = screenWidth / 2 - 30; // Adjust X as needed
  u_char y = screenHeight / 2;     // Center Y
  // Draw the title and image
  draw_ascii();
  if(score > highScore){
    highScore = score;
  }
  if(score == 30){
    drawString5x7(x, screenHeight - 48, "YOU WON",COLOR_GREEN, COLOR_BLACK);
  }
  else{
    drawString5x7(x, screenHeight - 48, "GAME OVER",COLOR_GREEN, COLOR_BLACK);
  }
    sprintf(scoreString, "Score: %d", score);
  drawString5x7(x, screenHeight - 16, scoreString , COLOR_WHITE, COLOR_BLACK);;
  // Draw the start prompt
  y += 10; // Move down for the next text
  x -= 15;
  sprintf(scoreString, "HighScore: %d", highScore);
  drawString5x7(x, screenHeight - 8, scoreString, COLOR_WHITE, COLOR_BLACK);
}


void main(){
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  switch_init();
  configureClocks();
  lcd_init();
  setup();
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  while (1) {/* forever */
    switch(gameState) {
    case 1:
      if(redrawScreen){
	score = 0;
	clearScreen(COLOR_BLACK);
	displayStartScreen();
	redrawScreen = 0;
      }
      P1OUT &= ~LED;
      or_sr(0x10);
      break;
    case 2:
      if(gameOver){
	gameState  = 3;
	redrawScreen = 1;
      }
      break;
    case 3:
      if(redrawScreen){
	clearScreen(COLOR_BLACK);
	displayEndScreen();
	redrawScreen = 0;
      }
      P1OUT &= ~LED;
      or_sr(0x10);
      break;
    }
  }

}



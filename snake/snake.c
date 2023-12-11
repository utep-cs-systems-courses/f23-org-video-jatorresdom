#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include <stdio.h>
#include "switches.h"

// WARNING: LCD DISPLAY USES P1.0.  Do not touch!!! 

#define LED BIT6		/* note that bit zero req'd for display */
#define NUM_SWITCHES 4

//Array to hold the state of each switch
int* states;

// Game Values
int gameState = 1;
int score = 0;
int highScore = 0;
int gameOver = 0;
short redrawScreen = 1;
void wdt_c_handler(){
  static int secondCount = 0;
  static int fps = 0;
  secondCount++;
  states = get_switches_states();

  if(gameState == 1){
    if(secondCount > 250){
      if(states[0]){
  	gameState = 3;
	redrawScreen = 1;
	
      }
    }
  }
  
  //testing score and high score
  
  else if(gameState == 3){
    fps++;
    if(fps > 250){
      fps = 0;
      score += 1;
      redrawScreen = 1;
    }
  }
  set_switches_states();

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
  char scoreString[50];
  char highScoreString[50];
  if(score > highScore){
    highScore = score;
  }
  sprintf(scoreString, "Score: %d", score);
  sprintf(highScoreString, "HighScore: %d", highScore);
  drawString5x7(x, screenHeight - 48, "GAME OVER",COLOR_GREEN, COLOR_BLACK);
  drawString5x7(x, screenHeight - 16, scoreString , COLOR_WHITE, COLOR_BLACK);;
  // Draw the start prompt
  y += 10; // Move down for the next text
  x -= 15;
  drawString5x7(x, screenHeight - 8, highScoreString , COLOR_WHITE, COLOR_BLACK);
}


void main(){
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  switch_init();
  configureClocks();
  lcd_init();
 
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
      break;
    case 2:
      //      if(gameOver){
      //gameState  = 3;
      //redrawScreen = 1;
      //}
      break;
    case 3:
      if(redrawScreen){
	clearScreen(COLOR_BLACK);
	displayEndScreen();
	redrawScreen = 0;
      }
      break;
    default:
      break;
    }
  }

}



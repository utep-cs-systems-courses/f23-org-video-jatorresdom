#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

// WARNING: LCD DISPLAY USES P1.0.  Do not touch!!! 

#define LED BIT6		/* note that bit zero req'd for display */

#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8
#define SWITCHES 15

short redrawScreen = 1;

static char 
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  /* update switch interrupt to detect changes from current buttons */
  P2IES |= (p2val & SWITCHES);	/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);	/* if switch down, sense up */
  return p2val;
}

void 
switch_init()			/* setup switch */
{  
  P2REN |= SWITCHES;		/* enables resistors for switches */
  P2IE |= SWITCHES;		/* enable interrupts from switches */
  P2OUT |= SWITCHES;		/* pull-ups for switches */
  P2DIR &= ~SWITCHES;		/* set switches' bits for input */
  switch_update_interrupt_sense();
}

int switches = 0;

void
switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;
}


void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}


void wdt_c_handler(){
  static int secondCount = 0;

    secondCount ++;
   
}

// Function to display the start screen

void displayStartScreen() {

  // Coordinates for the the middle screen

  u_char x = screenWidth / 2 - 30; // Adjust X as needed
  u_char y = screenHeight / 2;     // Center Y

  //Testing for snake drawing

  

  // Draw the title

  drawString5x7(x, 0, "Snake Game", COLOR_GREEN, COLOR_BLACK);

  fillRectangle(x - 16, y - 56, x + 64 , y + 32, COLOR_RED);
  
  // Draw the start prompt

  y += 10; // Move down for the next text
  x -= 15;
  drawString5x7(x, screenHeight - 8, "Press Any Button", COLOR_WHITE, COLOR_BLACK);

}


void main()
{
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
 
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLACK);
  displayStartScreen();
  //while (1) {			/* forever */
  //if (redrawScreen) {
  //  redrawScreen = 0;
  //  update_shape();
  //}
  //P1OUT &= ~LED;	/* led off */
  //or_sr(0x10);	/**< CPU OFF */
  //P1OUT |= LED;	/* led on */
  //}
}




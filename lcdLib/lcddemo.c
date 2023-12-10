/** \file lcddemo.c
 *  \brief A simple demo that draws a string and square
 */

#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

f1(){
  
  for(int i = 0; i < 150; i++)
    for(int j = 0; j < 150; j++)
    drawPixel(i,j,COLOR_PINK);
}
f2(){
  for(int i = 0; i < 125; i++)
    drawPixel(i,125-i,COLOR_BLUE);
}



/** Initializes everything, clears the screen, draws "hello" and a square */
int main()
{
  configureClocks();
  lcd_init();
  u_char width = screenWidth, height = screenHeight;

  clearScreen(COLOR_BLACK);

  //drawString5x7(20,20, "hello", COLOR_GREEN, COLOR_RED);

  // fillRectangle(30,30, 60, 60, COLOR_ORANGE);

  f1();
  f2();
}

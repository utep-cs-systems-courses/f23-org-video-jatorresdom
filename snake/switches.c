#include <msp430.h>
#include "switches.h"
#define NUM_SWITCHES 4  // Total number of switches


// Array to hold the state of each switch
int switch_states[NUM_SWITCHES] = {0};



void switch_init() {
  P2REN |= TOP_SWITCH;
  P2IE |= TOP_SWITCH;
  P2OUT |= TOP_SWITCH;
  P2DIR &= ~TOP_SWITCH;
}



void switch_interrupt_handler() {
  
  char p2val = P2IN;
  P2IES |= (p2val & TOP_SWITCH);
  P2IES &= (p2val | ~TOP_SWITCH);

}

int* get_switches_states(){

  return switch_states; 

}

void set_switches_states(){
  
  switch_states[0] = 0;
  switch_states[1] = 0;
  switch_states[2] = 0;
  switch_states[3] = 0;
}

void __interrupt_vec(PORT2_VECTOR) Port_2(){
  
  if(P2IFG & SW1){
    P2IFG &= ~SW1;
    switch_states[0] = 1;
    switch_interrupt_handler();
  }
  else if(P2IFG & SW2){
    P2IFG &= ~SW2;
    switch_states[1] = 1;
    switch_interrupt_handler();
  }
  else if(P2IFG & SW3){
    P2IFG &= ~SW3;
    switch_states[2] = 1;
    switch_interrupt_handler();
  }
  else if(P2IFG & SW4){
    P2IFG &= ~SW4;
    switch_states[3] = 1;
    switch_interrupt_handler();
  }

}

#include <SPI.h>
#include "RF24.h"
#include "printf.h"

unsigned short curr,prev;

// button setup
const unsigned short nButtons = 12;
const unsigned short clock = 7; // blue
const unsigned short latch = 6; // yellow
const unsigned short data = 5; // red
// ground -> black
// 5v -> white


// names of buttons
const char* name[] = {"B",
		      "Y",
		      "Select",
		      "Start",
		      "Up",
		      "Down",
		      "Left",
		      "Right",
		      "A",
		      "X",
		      "L",
		      "R"};


unsigned short getButtons(const unsigned short nButtons,
			  const unsigned short clock,
			  const unsigned short latch,
			  const unsigned short data);

unsigned short getButtons(const unsigned short nButtons,
			  const unsigned short clock,
			  const unsigned short latch,
			  const unsigned short data){
  // refresh data
  digitalWrite(latch,HIGH);
  digitalWrite(latch,LOW);

  unsigned short i;
  unsigned short buttons = 0;
  unsigned short swap = 1;
  for(i = 0; i < nButtons; ++i){
    // switch to next button
    digitalWrite(clock,LOW);

    // check if pressed
    if(digitalRead(data) == LOW){
      // toggle bit if read
      buttons |= swap;
    }
    swap <<= 1;

    digitalWrite(clock,HIGH);
  }
  return buttons;
}


void setup(void){
  Serial.begin(115200);
  printf_begin();

  pinMode(clock,OUTPUT); // clock (blue)
  pinMode(latch,OUTPUT); // latch (yellow)
  pinMode(data,INPUT); // data (red)

  // get initial state
  curr = getButtons(nButtons,clock,latch,data);
  prev = curr;
}



void loop(void){
  curr = getButtons(nButtons,clock,latch,data);
  if(curr != prev){
    // if different, print changes
    showDiff(curr,prev);
    prev = curr;
  }
}



void showDiff(const unsigned short curr,
	      const unsigned short prev){
  // bitwise XOR
  unsigned short diff = curr ^ prev;

  // print buttons that changed state
  unsigned short i;
  for(i = 0; i < nButtons; ++i){
    if(diff & (1 << i))
      if(curr & (1 << i))
	printf("% 8s: pressed\n\r",name[i]);
      else
	printf("% 8s: released\n\r",name[i]);
  }
}

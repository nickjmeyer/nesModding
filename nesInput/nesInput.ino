#include <SPI.h>
#include "printf.h"


const unsigned short nButtons = 12;

const unsigned short clock = 7;
const unsigned short latch = 6;
const unsigned short data = 5;


unsigned short prev, curr;


unsigned short readController(const unsignes short nButtons,
			      const unsigned short clock,
			      const unsigned short latch,
			      const unsigned short data);

unsigned short readController(const unsigned short nButtons,
			      const unsigned short clock,
			      const unsigned short latch,
			      const unsigned short data){
  digitalWrite(latch,HIGH);
  /* delay(100); */
  digitalWrite(latch,LOW);
  unsigned short i;
  unsigned short buttons = 0;
  unsigned short swap = 1;
  for(i = 0; i < nButtons; ++i){
    digitalWrite(clock,LOW);
    /* delay(10); */
    if(digitalRead(data) == LOW){
      buttons |= swap;
    }
    swap <<= 1;

    digitalWrite(clock,HIGH);
  }
  return buttons;
}



void setup(void){
  Serial.begin(57600);
  printf_begin();

  pinMode(clock,OUTPUT); // clock (blue)
  pinMode(latch,OUTPUT); // latch (yellow)
  pinMode(data,INPUT); // data (red)

  // ground (black)
  // 5v (white)

  prev = curr = 0;
}



void loop(void){
  unsigned short curr = readController(nButtons,clock,latch,data);
  if(curr != prev){
    printf("%hu\n\r",curr);
    prev = curr;
  }
}
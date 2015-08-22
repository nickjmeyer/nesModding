#include <SPI.h>
#include "printf.h"


typedef unsigned int uint;

const uint nButtons = 12;


void setup(void){
  Serial.begin(57600);
  printf_begin();

  pinMode(7,OUTPUT); // clock (blue)
  pinMode(6,OUTPUT); // latch (yellow)
  pinMode(5,INPUT); // data (red)

  // ground (black)
  // 5v (white)
}



void loop(void){
  digitalWrite(6,HIGH);
  /* delay(100); */
  digitalWrite(6,LOW);
  uint i;
  printf("[");
  for(i = 0; i < nButtons; ++i){
    digitalWrite(7,LOW);
    /* delay(10); */
    printf("%d",digitalRead(5));

    digitalWrite(7,HIGH);
    if((i+1) < nButtons)
      printf(", ");
  }
  printf("]\n\r");
}
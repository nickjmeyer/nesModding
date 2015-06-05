#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// button
const unsigned short nButtons = 2;
const unsigned short buttonPin[] = {6,7};

unsigned curr,prev;


unsigned short getButtons(const unsigned short nButtons,
			  const unsigned short * const buttonPin);

unsigned short getButtons(const unsigned short nButtons,
			  const unsigned short * const buttonPin){
  // write each button to a bit
  unsigned short i,buttons = 0,state;
  for(i = 0; i < nButtons; ++i){
    state = digitalRead(buttonPin[i]);
    if(state == HIGH){
      buttons |= (1u << i);
    }
  }
  return buttons;
}

void setup(void){
  Serial.begin(57600);
  printf_begin();

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(sizeof(unsigned short));

  radio.openWritingPipe(pipes[0]);

  unsigned short i;
  for(i = 0; i < nButtons; ++i)
    pinMode(buttonPin[i],INPUT);
  
  curr = prev = 0;
}



void loop(void){
  curr = getButtons(nButtons,buttonPin);
  
  if(curr != prev){
    printf("Buttons: %u\n\r",curr);
  
    bool ok = radio.write( &curr, sizeof(unsigned short) );
    prev = curr;
    
    if (ok)
      printf("sent.\n\r");
    else
      printf("failed.\n\r");

    // have to cycle listening otherwise communication fails
    radio.startListening();
    radio.stopListening();
  }
}

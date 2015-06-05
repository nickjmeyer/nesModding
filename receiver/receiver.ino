#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const unsigned short nButtons = 2;
const unsigned short keys[] = {216, // left
			       215}; // right
const char* name[] = {"LEFT","RIGHT"};

unsigned short curr = 0,prev = 0;

void keyMap(const unsigned short curr, const unsigned short prev,
	    const unsigned short nButtons){
  // map the button changes to keyboard commands
  
  unsigned short i,change;
  change = curr ^ prev;
  for(i = 0; i < nButtons; ++i){
    if((change & (1u << i)) && (curr & (1u << i))){
      printf("Press %s\n\r",name[i]);
      Keyboard.press(keys[i]);
    }
    else if(change & (1u << i)){
      printf("Release %s\n\r",name[i]);
      Keyboard.release(keys[i]);
    }
  }
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

  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();

  Keyboard.begin();

  curr = prev = 0;
}

void loop(void){
  // if there is data ready
  if ( radio.available() ){
    // Dump the payloads until we've gotten everything
    unsigned short got_buttons;
    bool done = false;
    while (!done){
      // Fetch the payload, and see if this was the last one.
      done = radio.read( &got_buttons, sizeof(unsigned short) );
      curr = got_buttons;

      keyMap(curr,prev,nButtons);
      prev = curr;

      printf("Got payload %u.\n\r",got_buttons);
    }
  }
}

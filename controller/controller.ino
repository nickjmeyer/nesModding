#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(3,4);


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

unsigned short curr,prev;

// button setup
/* const unsigned short nButtons = 12; */
/* const unsigned short clock = 7; // blue */
/* const unsigned short latch = 6; // yellow */
/* const unsigned short data = 5; // red */

// button pins
const unsigned short nButtons = 1;
const unsigned short buttonPins[3] = {6};

// ground -> black
// 5v -> white


unsigned short getButtons(const unsigned short nButtons);
			  /* const unsigned short clock, */
			  /* const unsigned short latch, */
			  /* const unsigned short data); */

unsigned short getButtons(const unsigned short nButtons){
			  /* const unsigned short clock, */
			  /* const unsigned short latch, */
			  /* const unsigned short data){ */
  /* digitalWrite(latch,HIGH); */
  /* delay(100); */
  /* digitalWrite(latch,LOW); */
  unsigned short i;
  unsigned short buttons = 0;
  unsigned short swap = 1;
  for(i = 0; i < nButtons; ++i){
    /* digitalWrite(clock,LOW); */
    /* delay(10); */
    if(digitalRead(buttonPins[i]) == LOW){
      buttons |= swap;
    }
    swap <<= 1;

    /* digitalWrite(clock,HIGH); */
  }
  return buttons;
}


void setup(void){
  Serial.begin(115200);
  printf_begin();

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();

  /* pinMode(clock,OUTPUT); // clock (blue) */
  /* pinMode(latch,OUTPUT); // latch (yellow) */
  /* pinMode(data,INPUT); // data (red) */

  int i;
  for(i = 0; i < nButtons; ++i){
    pinMode(buttonPins[i],INPUT_PULLUP);
  }

  curr = getButtons(nButtons);/* ,clock,latch,data); */
  prev = -1;
}

void loop(void){
  curr = getButtons(nButtons);/* ,clock,latch,data); */
  if(curr != prev){

    radio.stopListening();

    // Send the buttons
    printf("Now sending %hu...",curr);
    bool ok = radio.write( &curr, sizeof(unsigned short) );

    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    unsigned long wait = 300;
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > wait )
	timeout = true;

    // Describe the results
    if ( timeout ){
      printf("Failed, response timed out.\n\r");
      prev = curr;
    }
    else{
      // Grab the response, compare, and send to debugging spew
      radio.read( &prev, sizeof(unsigned short) );

      // Spew it
      printf("Got response %hu.\n\r",prev);
    }
  }
}

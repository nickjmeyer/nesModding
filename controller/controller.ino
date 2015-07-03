#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

unsigned short curr,prev;

// button
const unsigned short nButtons = 3;
const unsigned short buttonPin[] = {5,6,7};


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

  unsigned short i;
  for(i = 0; i < nButtons; ++i)
    pinMode(buttonPin[i],INPUT);

  curr = getButtons(nButtons,buttonPin);
  prev = -1;
}

void loop(void){
  curr = getButtons(nButtons,buttonPin);
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
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
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

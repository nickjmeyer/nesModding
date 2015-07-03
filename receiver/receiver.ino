#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

const int dryRunPin = 7;

bool isDryRun = false;


// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

unsigned short curr,prev;

const unsigned short nButtons = 3;
const unsigned short keys[] = {216, // left
			       215, // right
			       176}; // RETURN

const char* name[] = {"LEFT","RIGHT","RETURN"};

void keyMap(const unsigned short curr, const unsigned short prev){
  // map the button changes to keyboard commands

  isDryRun = !digitalRead(dryRunPin);

  unsigned short i,change;
  change = curr ^ prev;
  for(i = 0; i < nButtons; ++i){
    if((change & (1u << i)) && (curr & (1u << i))){
      printf("Press %s\n\r",name[i]);
      if(!isDryRun)
	Keyboard.press(keys[i]);
    }
    else if(change & (1u << i)){
      printf("Release %s\n\r",name[i]);
      if(!isDryRun)
	Keyboard.release(keys[i]);
    }
  }
}


void setup(void){
  // determine if dry run or not
  pinMode(dryRunPin,INPUT);
  digitalWrite(dryRunPin,INPUT);
  delay(20);

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

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();

  curr = 0;
  prev = -1;
}

void loop(void){
  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  // if there is data ready
  if (radio.available()){
    prev = curr;
    // Dump the payloads until we've gotten everything
    bool done = false;
    while (!done){
      // Fetch the payload, and see if this was the last one.
      done = radio.read( &curr, sizeof(unsigned short) );

      // Spew it
      printf("Got payload %hu...",curr);

      // Delay just a little bit to let the other unit
      // make the transition to receiver
      delay(20);
    }

    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( &curr, sizeof(unsigned short) );
    printf("Sent response.\n\r");

    keyMap(curr,prev);

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}

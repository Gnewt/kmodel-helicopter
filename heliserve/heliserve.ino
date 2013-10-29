#include <SoftwareSerial.h>

#define IR_PIN 8
#define COMMAND_LEN 32

// Start at throttle = 0
uint32_t currentCommand = 0x00080000;
uint32_t throttle = 0;

// For incoming serial bytes
byte c;

// To store the commands in our serial state machine
char commandArray[4];
int commandIndex;

void setup() {
  Serial.begin(19200);
  pinMode(IR_PIN, OUTPUT);
  
  delay(1000);
  // Tell the copter "hi" before we start sending
  // more commands
  initializeCopter(0);
  delay(10);
  initializeCopter(0);
  delay(10);
  initializeCopter(0);
  delay(10);
  
}

void loop() {
  // Send our current command ever time loop() runs, then check
  // for serial data
  delay(10);
  sendIRPacketBinary(calculateChecksum(currentCommand));
  delay(10);

  if (Serial.available()) {
    handleSerial();
  }
}

void handleSerial() {
  // Sending a 'preamble' byte of 0xFF resets the
  // state machine, avoiding issues with sync. Commands
  // are 4 bytes but sending 5 bytes in total avoids problems.
  //
  // The preamble byte is NOT required.

  c = Serial.read();
  if (c == 0xFF) {
    commandIndex = 0;
  }
  else {
    // Store incoming serial data to our command array
    // until it's full, then execute the command
    commandArray[commandIndex] = c;
    if (++commandIndex == 4) {
      commandIndex = 0;
      doSerialCommand();
    }
  }
}

void doSerialCommand() {
  // Our first byte is the command type
  int command = commandArray[0];
  // Second is unused. Third and fourth are the data.
  // Most significant byte first.
  int data = (commandArray[2] << 8) + commandArray[3];
  
  switch(command) {
    case 0x01: // throttle
    {
      setThrottle(data);
      break;
    }
    case 0x02: // re-pair
    {
      initializeCopter(data);
      break;
    }
    case 0x03: // pitch
    {
      setPitch(data);
      break;
    }
    case 0x04: // yaw
    {
      setYaw(data);
      break;
    }
  }
  
  Serial.println(currentCommand); // useful for debug purposes
}  

void setThrottle(uint32_t throttle) {
  if (throttle > 127) {
    throttle = 127;
  }
  throttle = throttle << 25;
  currentCommand = currentCommand & 0x1ffffff;
  currentCommand = currentCommand | throttle;
}

void setPitch(uint32_t pitch) {
  //currentCommand = currentCommand & 0xfffc3fff;
  currentCommand = currentCommand & 0xfff03fff; // zero all 6 bytes of pitch command
  if (pitch == 1) {
    pitch = 0xb8000; // mask for forward pitch
    currentCommand = currentCommand | pitch;
  }
  else if (pitch == 2) {
    pitch = 0x38000; // mask for backward pitch
    currentCommand = currentCommand | pitch;
  }
}

void setYaw(uint32_t yaw) {
  // avoid this for now -- not fully implemented
  currentCommand = 0xfd080000;
}
  

void initializeCopter(int channel) {
  // Channel A = 0; B = 1; C = 3
  uint32_t startCommand = 0x2080000;
  // Set the channel bits
  startCommand = startCommand | (channel << 4);
  startCommand = calculateChecksum(startCommand);
  
  sendIRPacketBinary(startCommand);
}

uint32_t calculateChecksum(uint32_t command) {
  // Pass this a full-length (32-bit) command
  // returns the full checksummed command.
  // The checksum itself is a binary addition
  // of all the nibbles of the command,
  // truncated to 4 bits.
  
  // Let's first ensure that the checksum bytes are zero
  // or else our calculated checksum will not be correct
  command = command & 0xffffff00;
  
  // Checksum is the last 4 bytes of the sum
  // of all the preceding nibbles
  int checksum = 0;
  // work backwards through the command to get the nibbles
  for (int i = 0; i < (32 / 4); i++) {
    checksum = checksum + (command >> (4 * i) & 0x0f);
  }

  command = command + (checksum & 0xf);
  return command;
}

// Preamble, one, and zero functions to make life easy

void preamble() {
  pulseIR(2000);
  delayMicroseconds(400);
}

void one() {
  pulseIR(1200);
  delayMicroseconds(400);
}

void zero() {
  pulseIR(400);
  delayMicroseconds(400);
}

// All the dirty work

void sendIRPacketBinary(uint32_t command) {
  preamble();
  for (int i = 1; i <= COMMAND_LEN; i++) {
    if (bitRead(command, COMMAND_LEN - i) == 1) {
      one();
    }
    else {
      zero();
    }
  }
}

void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
   digitalWrite(IR_PIN, HIGH); 
   delayMicroseconds(8); 
   digitalWrite(IR_PIN, LOW);
   delayMicroseconds(8); 
 
   microsecs -= 30; // counted with a scope, change if you have problems
  }
 
  sei();  // this turns them back on
}
  

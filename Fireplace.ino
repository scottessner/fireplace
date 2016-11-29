#include <IRremote.h>

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 5;    // the number of the pushbutton pin

// Variables will change:
int switchState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int remoteState = HIGH;     // state of the command from the remote

//Remote codes for specific numbers
const int c0 = 0xB5;
const int c1 = 0x79;
const int c2 = 0x7D;
const int c3 = 0x7B;
const int c4 = 0x71;
const int c5 = 0x75;
const int c6 = 0xB9;
const int c7 = 0xBD;
const int c8 = 0xBB;
const int c9 = 0xB1;
int currentChar = -1;

const int passcode[] = {1,0,9,0,4};
const int codeLength = 5;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

long remoteTime = 0;
long remoteTarget = 3000;
long remoteTol = 2000;
int lastCommand = 0;
int thisCommand = 0;
int currentCodePosition = 0;
long lastCommandTime = 0;
int commandHandled = LOW;

int RECV_PIN = 11; 
int RelayPin = 12;
int LEDPin = 13;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(LEDPin, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(LEDPin, HIGH);
  digitalWrite(RelayPin, HIGH);
  pinMode(buttonPin, INPUT_PULLUP);

  buttonState = digitalRead(buttonPin);
  lastButtonState = buttonState;
  remoteState = buttonState;
}

void loop() {
  //Get IR Code if one is available
  if (irrecv.decode(&results)) {
    //Serial.println(results.value, HEX);

    //Store the current command and
    thisCommand = results.value;
    lastCommandTime = millis();
    
    //Act if this is a new code
    if(lastCommand != thisCommand){
      //Store this command to watch for repeats
      lastCommand = thisCommand;

      //Get the value for the current code
      switch(thisCommand & 0xFF){
        case c0:
          currentChar = 0;
          break;
        case c1:
          currentChar = 1;
          break;
        case c2:
          currentChar = 2;
          break;
        case c3:
          currentChar = 3;
          break;
        case c4:
          currentChar = 4;
          break;
        case c5:
          currentChar = 5;
          break;
        case c6:
          currentChar = 6;
          break;
        case c7:
          currentChar = 7;
          break;
        case c8:
          currentChar = 8;
          break;
        case c9:
          currentChar = 9;
          break;
      }
      //Serial.println(currentChar);
        //If this is the correct value, look for the next number in the code
        if(currentChar == passcode[currentCodePosition]){
          //If this is the last number in the code, change the state of the remote
          Serial.print(currentChar);
          Serial.print("(");
          Serial.print(currentCodePosition);
          Serial.print(") ");
          if(++currentCodePosition == codeLength){
            remoteState = !remoteState;  
            Serial.println(" Success!");          
          }
        }else{
          //If an incorrect button was pressed, start over
          currentCodePosition = 0;
          Serial.println(" Fail");
        }
    }
    irrecv.resume(); // Receive the next value
  }else{
      //If we don't get a code for a while, start over
      if(millis() - lastCommandTime > remoteTol){
        currentCodePosition = 0;
        //Serial.println(" Timed Out");
      }
    }
  

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        switchState = !switchState;
      }
    }
  }
  lastButtonState = reading;
  
  if (buttonState ^ remoteState) {
    digitalWrite(LEDPin, LOW);
    digitalWrite(RelayPin, LOW);
    //Serial.println("Turned off relay");
  }else{
    digitalWrite(LEDPin, HIGH);
    digitalWrite(RelayPin, HIGH);
    //Serial.println("Turned on relay");
  }

  //Serial.print("Switch: ");
  //Serial.print(buttonState);
  //Serial.print(" Remote: ");
  //Serial.println(remoteState);
}




void dump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");

  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC - Address: ");
    Serial.print(results->address, HEX);
    Serial.print(" Value: ");
  }
  else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print("Decoded AIWA RC T501: ");
  }
  else if (results->decode_type == WHYNTER) {
    Serial.print("Decoded Whynter: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  //Serial.print("Raw (");
  //Serial.print(count, DEC);
  //Serial.print("): ");

  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}

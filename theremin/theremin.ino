////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAKE MAGAZINE - MIDI LASER THEREMIN FOR ARDUINO v1.0
// Stephen Hobley 2008
// www.stephenhobley.com
//
// Edited by: Josiah Witt 2013
// www.josiahwitt.com
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connect the Pitch sensor to A0 and the Laser sensor to D2
// This example covers sending MIDI note on and off data, along with pitchbend controller information
// It shows how to read an analog sensor, 'linearize' and scale the value, then convert into a MIDI message
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// #defines: ////////////////////////////////////////////////////////////////////////////////////////////
#define MIDICMD_NOTEON 0x90 // MIDI command (Note On, Channel 0)
//#define MIDICMD_CTRL   0xb0 // Controller message - channel 0
#define MIDICMD_PB     0xe0 // Pitchbend message  - channel 0
#define MIDI_ROOT_NOTE 60   // Root note for the theremin

#define PITCHINPUT 0        // Connect the GP2D12 sensor output to this analog pin 
#define LASERINPUT 2        // Connect the laser detector to this pin
#define LEDPIN     13       // Pin that the LED is connected to

#define SENSORSCALE 500.0    // Floating point maximum range for sensor - tweak this to find the best range 

// Timing control
#define LOOPDELAY 10

// Variables: ////////////////////////////////////////////////////////////////////////////////////////////
byte lowerBits; // Pitchbend value LSB
byte upperBits; // Pitchbend value MSB

int iCounter = 0; // Counter used to reduce the sample rate
byte noteOnToggle = HIGH; // Used to hold the state of the beam

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function takes the value from the GP2 sensor and calculates the MIDI controller value from it
// this is a bit slow as it uses floating point math
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessAnalogValue(byte i)
{
  // get a value for the GP sensor on pin i
  float _x =  read_gp2d12_range_float(i);
  
  // Truncate at 70
  if (_x > SENSORSCALE)
    _x = SENSORSCALE;
    
  // 0 - 16383 is the full 14 bit pitchbend range
  int _converted = (int)(_x/SENSORSCALE * 16383.0);

  // Convert this 14 bit range value to LSB and MSB bytes
  lowerBits = (byte)(_converted & 0x7F);
  _converted >>= 7;
  upperBits = (byte)(_converted & 0x7F);

  // Now output the message
  SendMIDI(MIDICMD_PB, lowerBits, upperBits); 
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Reads the GP2 sensor and returns the value in cms - floating point version - provided by Javier Valencia 2008
/////////////////////////////////////////////////////////////////////////////////////////////////////////
float read_gp2d12_range_float(byte pin) 
{
  int tmp;
  tmp = analogRead(pin);
    if (tmp < 3)
      return -1.0; // invalid value
    return (6787.0 /((float)tmp - 3.0)) - 4.0;
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plays a MIDI note. 
//  Format of MIDI note on 
//  1<xxx><yyyy> - xxx is 001 (Note On), yyyy is the channel number - in our case 0000 - which is channel 0
//  0<xxxxxxx> - note number 0-127
//  0<yyyyyyy> - note velocity 0-127 - instead of sending a note off, you can send 0 in this field to silence a note
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void SendMIDI(char cmd, char data1, char data2) 
{
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP FUNCTION
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  // Set MIDI baud rate:
  Serial.begin(31250);

  pinMode(LEDPIN, OUTPUT);      // sets the digital pin 13 as output for testing purposes
  pinMode(LASERINPUT, INPUT);        // sets pin 2 to receive note on data
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  {
    if (digitalRead(LASERINPUT) != noteOnToggle)
    {
      if (noteOnToggle == HIGH)
      {
        // Send the root note
        SendMIDI(MIDICMD_NOTEON, MIDI_ROOT_NOTE, 127);

        // Turn on the LED for debugging
        //digitalWrite(LEDPIN, HIGH);
      }
      else
      {
        // NOTE OFF
        SendMIDI(MIDICMD_NOTEON, MIDI_ROOT_NOTE, 0); // Silence the note

        // Turn off the LED
        //digitalWrite(LEDPIN, LOW);
        
      }
     // Finally flip the state of the playarray
     noteOnToggle = !noteOnToggle;
    }
    else if (noteOnToggle == LOW)
    {
      
      // Note: the update rate of the sensor is 25Hz - so we only need to read about once every 40ms - let's make it 30ms (3xLOOPDELAY) to be safe.
      // If it is LOW then read analog pin and send out a pitchbend message
      if (iCounter == 3)
      {
        ProcessAnalogValue(PITCHINPUT);
        iCounter = 0;
      }
      iCounter++;
    }
  }
 
 // Pause processing for 1/100 of a second - adjust this value to alter the response.
 delay(LOOPDELAY);
}


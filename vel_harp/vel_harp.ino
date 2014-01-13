/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAKE MAGAZINE - LASER HARP FOR ARDUINO
// Stephen Hobley 2008
// www.stephenhobley.com
//
// Edited by: Josiah Witt 2013
// www.josiahwitt.com
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Variables: ///////////////////////////////////////////////////////////////////////////////////////////
#define BEAMCOUNT 8         // Number of Beams 
#define MIDICMD_NOTEON 0x90 // MIDI command (Note On, Channel 0)
/#define MIDICMD_CTRL   0xb0 // Controller message - channel 0
//#define MIDICMD_PB     0xe0 // Pitchbend message  - channel 0
//#define MIDI_ROOT_NOTE 60   // Root note for the theremin

#define LOOPDELAY 10
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA STRUCTURES
// These structures govern the behaviour of each of the 6 laser beams in harp mode
// access any of these arrays in the main polling loop <array>[i]
/////////////////////////////////////////////////////////////////////////////////////////////////////////
byte pinarray[BEAMCOUNT]         = {2,3,4,5,6,7,8,9}; // Digital pins to read for laser beam breaks
byte analogarray[BEAMCOUNT]      = {0,1,2,3,4,5,6,7}; // Analog pins to read for continuous data
byte notearray[BEAMCOUNT]        = {64,63,62,60,59,58,57,56}; // MIDI note array - these are the notes that will play for each beam
byte playarray[BEAMCOUNT]        = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH}; // The state of all beams to start with

#define SENSORSCALE 1000.0    // Floating point maximum range for sensor - tweak this to find the best range 

int iCounter = 0; // Counter used to reduce the sample rate
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function takes the value from the GP2 sensor and calculates the MIDI controller value from it
// this is a bit slow as it uses floating point math
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int ProcessAnalogValue(byte i)
{
  // get a value for the GP sensor on pin i
  float _x =  read_gp2d12_range_float(i);
  
  // Truncate at 1000
  if (_x > 1000.0)
    _x = 1000.0;
  
  // 0 - 127 is the full velocity range
  int _converted = (int)(_x/SENSORSCALE * 127.0);
  
  // Now reverse the scale - louder closer to sensor
  //_converted = 127 - _converted;
  
  return _converted;
  
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
  //Serial.begin(9600);

  //pinMode(13, OUTPUT);      // sets the digital pin as output
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // Loop through 6 inputs and find one that is different to how it was before
  // if is is LOW, then note on, HIGH then note off..
  for (int i= 0; i < BEAMCOUNT; i++)
  {
    // 1 Loop through 6 inputs and find one that is different to how it was before
    if (digitalRead(pinarray[i]) != playarray[i])
    {
      if (playarray[i] == HIGH)
      {
        // NOTE_ON

        SendMIDI(MIDICMD_NOTEON, notearray[i], ProcessAnalogValue(analogarray[i]));
      }
      else
      {
        // NOTE OFF

        // Sound the MIDI note
        SendMIDI(MIDICMD_NOTEON, notearray[i], 0); // Silence the note
      }
     // Finally flip the state of the playarray
     playarray[i] = !playarray[i];
    }
  }
 
 // Pause processing for 1/100 of a second - adjust this value to alter the response of the harp.
 delay(LOOPDELAY);
}


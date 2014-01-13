/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAKE MAGAZINE - MIDI TEST CODE
// Stephen Hobley 2008
// www.stephenhobley.com
//
// Edited by: Josiah Witt 2013
// www.josiahwitt.com
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Variables: ///////////////////////////////////////////////////////////////////////////////////////////
#define MIDICMD_NOTEON 0x90 // MIDI command (Note On, Channel 0)

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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // Loop through 6 inputs and find one that is different to how it was before
  // if is is LOW, then note on, HIGH then note off..
  for (int i= 60; i < 70; i++)
  {
    // NOTE_ON
    SendMIDI(MIDICMD_NOTEON, i, 127);
    delay(250);
   
    // NOTE OFF
    SendMIDI(MIDICMD_NOTEON, i, 0); // Silence the note
  }
}


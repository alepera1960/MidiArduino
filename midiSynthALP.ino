/****************************************************************************************** 
 * This is an example for the Music Shield SEED VS1053 Codec Breakout
 * 
 * 
 * 
 * Written by Antonio Le Pera 
 * BSD license, all text above must be included in any redistribution
 * Arduino Uno
 * Bottone per program change su pin PD3 e massa
 * Midi in da tastiera su pin RX PD0
 ********************************************************************************************/
// VS1103b MIDI Synth
// The implemented midi messages here. Some like tempo are used in file mode only. 

#include <SD.h>
#include <SPI.h>
#include <Arduino.h>

#include <MusicPlayer.h>
#include <pins_config.h>
#include <vs10xx.h>

#include <MIDI.h>

#define btnFunc 3  // D7 Select Function

MIDI_CREATE_DEFAULT_INSTANCE();

int note;
int velocity;
int instrument;
boolean AnyBtnPress = false;

void setup() {
  player.beginInMidiFmt();
  midiWriteData(0xB0, 0x07, 127);
  instrument=0;
  midiWriteData(0xB0, 0, 0x00); //Default bank GM1
  midiWriteData(0xC0, instrument, 0);    //Set instrument number. 0xC0 is a 1 data byte command
  pinMode(btnFunc, INPUT_PULLUP);

  MIDI.begin(MIDI_CHANNEL_OMNI); 
  initIOForLED();
}

void loop()
{
    if (digitalRead(btnFunc) == LOW) {
        while (digitalRead(btnFunc) == LOW) {
            //mentre il pulsante rimane premuto non eseguire nulla
        } 
    
    //il pulsante è stato rilasciato e proseguo con l'esecuzione
        if (instrument<128)  instrument = instrument + 1; 
            else instrument = 0; 
        midiWriteData(0xC0, instrument, 0);    //Set instrument number. 0xC0 is a 1 data byte command 
    } //chiudo if

    
    if (MIDI.read())                    // If we have received a message
    {
    switch(MIDI.getType()) {
            case midi::NoteOn:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        midiNoteOn(0, note, velocity);
        GREEN_LED_ON();
        break;

      case midi::NoteOff:
        note = MIDI.getData1();
        midiNoteOff(0, note, 127);
        GREEN_LED_OFF();
        break;
    }
    }
}


/*
 **@ function:midiWriteData
 **@ usage:Write data to Midi Player
 **@ input:
 **@ output:none
*/
void midiWriteData(byte cmd, byte high, byte low)
{
  while(!digitalRead(VS_DREQ));
  digitalWrite(VS_XDCS, LOW);
  midiSendByte(cmd);
  if((cmd & 0xF0) <= 0xB0 || (cmd & 0xF0) >= 0xE0)
  {
    midiSendByte(high);
    midiSendByte(low);
  }
  else
  {
    midiSendByte(high);
  }
  digitalWrite(VS_XDCS, HIGH);
}


void midiNoteOn(byte channel, byte note, byte rate)
{
  midiWriteData((0x90 | channel), note, rate);
}

void midiNoteOff(byte channel, byte note, byte rate)
{
  midiWriteData((0x80 | channel), note, rate);
}

void midiSendByte(byte data)
{
  SPI.transfer(0x00);
  SPI.transfer(data);
}

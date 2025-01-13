#include <FlexIO_t4.h>
#include <FlexSerial.h>
#include <Wire.h>
#include <MIDI.h>
#include <Bounce.h>
#include <math.h>
#include "Adafruit_Trellis.h"
#define NUMTRELLIS 8				// **** SET # OF TRELLISES HERE
#define numKeys (NUMTRELLIS * 16)
Adafruit_Trellis matrix[NUMTRELLIS] = {
	Adafruit_Trellis(), Adafruit_Trellis(),
	Adafruit_Trellis(), Adafruit_Trellis(),
	Adafruit_Trellis(), Adafruit_Trellis(),
	Adafruit_Trellis(), Adafruit_Trellis()
	};
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(
	&matrix[0], &matrix[1], &matrix[2], &matrix[3],
	&matrix[4], &matrix[5], &matrix[6], &matrix[7]
);
const byte START = 250;				//MIDI BeatClock
const byte CONTINUE = 251;			//MIDI BeatClock
const byte STOP = 252;				//MIDI BeatClock
const byte CLOCK = 248;				//MIDI BeatClock
const byte noteSendOn = 144;		//MIDI 1001 0000 -> Status byte note on / Channel 1
const byte noteSendOff = 128;		//MIDI 1000 0000 -> Status byte note off / Channel 1
const byte velocityZero = 0;		//MIDI 0000 0000 -> Data byte For note on with 0 velocity
bool function = LOW;				//Is funktion view active?	
bool setPattern1 = LOW;				//View: to which 1-8 row should the pattern be set?
bool setPattern2 = LOW;				//View: choose the pattern
byte patternEdit1;					//The row for which the pattern is to be set
byte patternEdit2;					//The choosen pattern
byte setMute;						//View: set left mute and right solo
bool mute[8][16];
bool noteTriplet[16];
const byte tripletStep[2] = {15,11};
byte startBlinkSleep = 50;
byte beatClockCounter = 0;
byte steps[16];
const byte pinPlay = 23;
Bounce bouncePlay = Bounce(pinPlay, 300);
const byte pinFunction = 12;
Bounce bounceFunction = Bounce(pinFunction, 300);
byte eightRows;
byte pagePlay[16];
byte pageView[16];
EXTMEM bool pageActive[16][256];
byte pageNext;
byte pagePre;
byte pageFirst;
byte pageLast;
EXTMEM byte pageChange[256][256];
byte channelView = 0;
bool channelActive[16];
byte channelPlay1;
byte channelPlay2;
bool longNotes[16];
byte channelCount = 16;
EXTMEM byte notePitch[16][256][8][16];
EXTMEM byte noteVolume[16][256][8][16];
bool volumeVolume1 = LOW;
bool volumeVolume2 = LOW;
EXTMEM byte noteEnd[16][256][8][16];
EXTMEM byte noteStart[16][256][8][16];
byte startNotes[8] = {49,42,45,43,41,39,38,36};
EXTMEM bool softwareMatrix[16][256][128];
bool noteStop[16][128];
bool noteStopFunction[16][128];
EXTMEM bool noteOnTrigger[16][256][128];
EXTMEM bool noteOffTrigger[16][256][128];
EXTMEM bool noteOnOff[16][256][8][16];
EXTMEM bool startNote[16][256][8];
bool setLongNotes = LOW;
byte pageNextB;
bool setRange1 = LOW;
bool setRange2 = LOW;
bool setAll = LOW;
bool set8 = LOW;
bool setPage = LOW;
bool setChannel = LOW;
bool setPitch = LOW;
bool setVolume = LOW;
bool setNoteStart = LOW;
bool setNoteEnd = LOW;
byte noteEdit = 0;
byte noteOnVolume[128];
unsigned long trellisTime = 0;				 // letzter Zeitwert bei dem der Ausgangzustand wechselte.
unsigned long trellisDebounce = 30;	 // Entprellzeit
unsigned long clockTime = 0;
unsigned long clockDebounce = 200;
unsigned long usbTime = 0;
unsigned long usbDebounce = 1;
byte usbTransfer[5];
int BPM = 120;
bool setBPM = LOW;
unsigned int latency[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int latencyPlay[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool setLatency = LOW;
bool playLater[16][8];
byte playLaterNotePitch[16][8];
byte playLaternoteVolume[16][8];
unsigned long playLaterMillis[16][8];
bool playLaterOff[16][8];
byte playLaterOffNote[16][8];
unsigned long playLaterOffMillis[16][8];
byte noteLength[16];
unsigned long makeClock1 = 0;
unsigned long makeClock2 = 0;
unsigned long clockCurrent;
float clockIntervall = 60.0/BPM*1000*1000/24.0;
unsigned long clockCounter = 0;
byte clock2[16];
bool clockStart = LOW;
bool isClock = LOW;
bool isPlay = LOW;
bool isInternal = HIGH;
const byte funtionMatrix1[128] = { 0, 1, 2, 3,16,17,18,19, 32, 33, 34, 35, 48, 49, 50, 51,
								   4, 5, 6 ,7,20,21,22,23, 36, 37, 38, 39, 52, 53, 54, 55,
								   8, 9,10,11,24,25,26,27, 40, 41, 42, 43, 56, 57, 58, 59,
								  12,13,14,15,28,29,30,31, 44, 45, 46, 47, 60, 61, 62, 63,
								  64,65,66,67,80,81,82,83, 96, 97, 98, 99,112,113,114,115,
								  68,69,70,71,84,85,86,87,100,101,102,103,116,117,118,119,
								  72,73,74,75,88,89,90,91,104,105,106,107,120,121,122,123,
								  76,77,78,79,92,93,94,95,108,109,110,111,124,125,126,127};
byte funtionMatrix2[128];
const byte hardwareMatrix[8][16] = {{ 0, 1, 2, 3,16,17,18,19, 32, 33, 34, 35, 48, 49, 50, 51},
									{ 4, 5, 6 ,7,20,21,22,23, 36, 37, 38, 39, 52, 53, 54, 55},
									{ 8, 9,10,11,24,25,26,27, 40, 41, 42, 43, 56, 57, 58, 59},
									{12,13,14,15,28,29,30,31, 44, 45, 46, 47, 60, 61, 62, 63},
									{64,65,66,67,80,81,82,83, 96, 97, 98, 99,112,113,114,115},
									{68,69,70,71,84,85,86,87,100,101,102,103,116,117,118,119},
									{72,73,74,75,88,89,90,91,104,105,106,107,120,121,122,123},
									{76,77,78,79,92,93,94,95,108,109,110,111,124,125,126,127}};
bool copyPage = LOW;
bool copyPageView[256];
bool copyPageChannel[16];
byte liveLowerNote[16];
bool playLive = LOW;
bool playLiveTest = LOW;
const byte liveNotes[128] = { 50,50,50,50,50, 1, 3,50, 6, 8,10,50,50,50,50,50,
							  50,50,50,50, 0, 2, 4, 5, 7, 9,11,12,50,50,50,50,
							  50,50,50,50,50,13,15,50,18,20,22,50,50,50,50,50,
							  50,50,50,50,12,14,16,17,19,21,23,24,50,50,50,50,
							  50,50,50,50,50,25,27,50,30,32,34,50,50,50,50,50,
							  50,50,50,50,24,26,28,29,31,33,35,36,50,50,50,50,
							  50,50,50,50,50,37,39,50,42,44,46,50,50,50,50,50,
							  50,50,50,50,36,38,40,41,43,45,47,48,50,50,50,50};
bool freeLiveNotes = HIGH;
byte liveIsPressed[128];
bool liveIsUnPressed[128];
byte MIDIByte;
byte statusByte;
byte dataByte1;
byte dataByte2;
bool monoStepRecord = LOW;
bool chordStepRecord = LOW;
byte stepRecordStep = 0;
bool mooveIn = HIGH;
bool mooveInChannel[16];
int mooveInCounter[16];
FlexSerial MIDI01(-1, 9);
FlexSerial MIDI02(-1, 11);
FlexSerial MIDI03(-1, 6);
FlexSerial MIDI04(-1, 8);
FlexSerial MIDI05(-1, 10);
FlexSerial MIDI06(-1, 21);
FlexSerial MIDI07(-1, 17);
FlexSerial MIDI08(-1, 15);
FlexSerial MIDI09(-1, 41);
FlexSerial MIDI10(-1, 39);
FlexSerial MIDI11(-1, 37);
FlexSerial MIDI12(-1, 35);
FlexSerial MIDI13(-1, 33);
FlexSerial MIDI14(-1, 26);
FlexSerial MIDI15(-1, 27);
FlexSerial MIDI16(-1, 32);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIInput); //PIN 0 (RX1)
//int led = 13;
//bool ledcheck;


void setup(){
	CCM_CS1CDR &= ~( CCM_CS1CDR_FLEXIO2_CLK_PRED( 7 ) );
	CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PRED( 0b11 );
	pinMode(pinFunction, INPUT_PULLUP);
	pinMode(pinPlay, INPUT_PULLUP);
	MIDIInput.begin(MIDI_CHANNEL_OMNI);
	//pinMode(led, OUTPUT);
	MIDI01.begin(31250);
	MIDI02.begin(31250);
	MIDI03.begin(31250);
	MIDI04.begin(31250);
	MIDI05.begin(31250);
	MIDI06.begin(31250);
	MIDI07.begin(31250);
	MIDI08.begin(31250);
	MIDI09.begin(31250);
	MIDI10.begin(31250);
	MIDI11.begin(31250);
	MIDI12.begin(31250);
	MIDI13.begin(31250);
	MIDI14.begin(31250);
	MIDI15.begin(31250);
	MIDI16.begin(31250);
	for (byte c=0; c<128; c++){
		funtionMatrix2[funtionMatrix1[c]] = c;
	}
	
	for (byte c=0; c<128; c++){
		for (byte x=0; x<16; x++){
			noteStop[x][c] = LOW;
			noteStopFunction[x][c] = LOW;
			for (byte y=0; y<channelCount; y++){
				softwareMatrix[y][x][c] = LOW;
			}
		}
	}
	for (byte c=0; c<4; c++){
		usbTransfer[c] = 0;
	}
	for (byte z=0; z<16; z++){
		longNotes[z] = LOW;
		channelActive[z] = HIGH;
		noteLength[z] = 12;
		steps[z] = 15;
		clock2[z] = 40;
		noteTriplet[z] = LOW;
		copyPageChannel[z] = LOW;
		liveLowerNote[z] = 36;
		mooveInChannel[z] = LOW;
		pagePlay[z] = 255;
		pageView[z] = 0;
		for (byte c=0; c<8; c++){
			mute[c][z] = LOW;
			for (int x=0; x<256; x++){
				startNote[z][x][c] = HIGH;
				for (byte y=0; y<channelCount; y++){
					notePitch[y][x][c][z] = startNotes[c];
					noteVolume[y][x][c][z] = 100;
					noteEnd[y][x][c][z] = 1;
					noteStart[y][x][c][z] = 0;
					noteOnOff[y][x][c][z] = LOW;
					if (x == 0){
						pageActive[y][x] = HIGH;
					} else {
						pageActive[y][x] = LOW;
					}
				}
			}
		}
	}
	for (int z=0; z<256; z++){
		copyPageView[z] = LOW;
		if (z + 1 == 256){
			pageChange[0][z] = 0;
		} else {
			pageChange[0][z] = z + 1;
		}
		for (int c=1; c<256; c++){
			if (pageChange[c-1][z] == 255){
				pageChange[c][z] = 0;
			} else {
				pageChange[c][z] = pageChange[c-1][z] + 1;
			}
		}
	}
	trellis.begin(0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77);
	usbMIDI.setHandleRealTimeSystem(usbBeatClock);
	trellisTime = millis();
	for (byte c=0;c<128;c++){
		trellis.setLED(funtionMatrix1[c]);
		trellis.writeDisplay();
		delay(5);
	}
	for (byte c=0;c<128;c++){
		trellis.clrLED(funtionMatrix1[c]);
		trellis.writeDisplay();
		delay(5);
	}
}

void loop(){
	//if (isPlay == LOW) { //Für Timing Probleme bitte zweiten Arduino die MIDI Beat Clock machen lassen.
	if (millis() - trellisTime > trellisDebounce){
		if (trellis.readSwitches()){
			for (byte z=0; z<numKeys; z++){
				if (trellis.justPressed(z)){
					if (function == LOW){ //Set Step
						if (softwareMatrix[channelView][pageView[channelView]][z]==LOW){
							softwareMatrix[channelView][pageView[channelView]][z] = HIGH;
							noteOnTrigger[channelView][pageView[channelView]][z] = HIGH;
							noteOffTrigger[channelView][pageView[channelView]][z] = HIGH;
							trellis.setLED(z);
							trellis.writeDisplay();
						} else {
							softwareMatrix[channelView][pageView[channelView]][z] = LOW;
							noteOnTrigger[channelView][pageView[channelView]][z] = LOW;
							noteOffTrigger[channelView][pageView[channelView]][z] = LOW;
							trellis.clrLED(z);
							trellis.writeDisplay();
						}
					} else if (setRange1 == HIGH){
						for (byte c=0; c<128; c++){
							trellis.clrLED(c);
						}
						switch (z) {
							case 124:
								for (byte c=0; c<128; c++){
									trellis.setLED(funtionMatrix1[c]);
								}
								setAll = HIGH;
								setRange1 = LOW;
								setRange2 = HIGH;
								break;
							case 125:
								for (byte c=0; c<8; c++){
									trellis.setLED(hardwareMatrix[c][0]);
								}
								set8 = HIGH;
								setRange1 = LOW;
								setRange2 = HIGH;
								break;
							case 126:
								if (setPitch == HIGH){
									for (byte c=0; c<=notePitch[channelView][pageView[channelView]][0][0]; c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								} else if (setVolume == HIGH){
									for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][0][0]; c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								} else if (setNoteStart == HIGH){
									for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								} else if (setNoteEnd == HIGH){
									for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								}
								setPage = HIGH;
								setRange1 = LOW;
								break;
							case 127:
								if (setPitch == HIGH){
									for (byte c=0; c<=notePitch[channelView][pageView[channelView]][0][0]; c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								} else if (setVolume == HIGH){
									for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][0][0]; c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								} else if (setNoteStart == HIGH){
									for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								} else if (setNoteEnd == HIGH){
									for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
										trellis.setLED(funtionMatrix1[c]);
									}
								}
								setChannel = HIGH;
								setRange1 = LOW;
								break;
						}
						trellis.writeDisplay();
					} else if (setRange2 == HIGH && setAll == HIGH){
						for (byte c=0; c<128; c++){
							trellis.clrLED(c);
						}
						noteEdit = funtionMatrix2[z];
						if (setPitch == HIGH){
							for (byte c=0; c<=notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setVolume == HIGH){
							for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setNoteStart == HIGH){
							for (byte c=noteStart[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setNoteEnd == HIGH){
							for (byte c=noteStart[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
						setRange2 = LOW;
					} else if (setRange2 == HIGH && set8 == HIGH){
						for (byte c=0; c<128; c++){
							trellis.clrLED(c);
						}
						switch (z) {
							case 0:
								noteEdit = 0;
								break;
							case 4:
								noteEdit = 1;
								break;
							case 8:
								noteEdit = 2;
								break;
							case 12:
								noteEdit = 3;
								break;
							case 64:
								noteEdit = 4;
								break;
							case 68:
								noteEdit = 5;
								break;
							case 72:
								noteEdit = 6;
								break;
							case 76:
								noteEdit = 7;
								break;
						}
						if (setPitch == HIGH){
							for (byte c=0; c<=notePitch[channelView][pageView[channelView]][noteEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setVolume == HIGH){
							for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][noteEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setNoteStart == HIGH){
							for (byte c=noteStart[channelView][pageView[channelView]][noteEdit][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit][0];c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setNoteEnd == HIGH){
							for (byte c=noteStart[channelView][pageView[channelView]][noteEdit][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit][0];c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
						setRange2 = LOW;
					} else if (setPitch == HIGH){ //change the notePitch
						if (setAll == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
							notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16] = funtionMatrix2[z];
							noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (set8 == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],channelView);
							for (int c=pageView[channelView]; c<256; c++){
								if (pageActive[channelView][c] == HIGH){
									for (byte x=0; x<16; x++){
										notePitch[channelView][c][noteEdit][x] = funtionMatrix2[z];
									}
								}
							}
							noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],noteVolume[channelView][pageView[channelView]][noteEdit][0],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit][0]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=notePitch[channelView][pageView[channelView]][noteEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setPage == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
							for (int c=pageView[channelView]; c<256; c++){
								if (pageActive[channelView][c] == HIGH){
									for (byte y=0; y<8; y++){
										for (byte x=0; x<16; x++){
											notePitch[channelView][c][y][x] = funtionMatrix2[z];
										}
									}
								}
							}
							noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=notePitch[channelView][pageView[channelView]][0][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setChannel == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
							for (int c=0; c<256; c++){
								for (byte y=0; y<8; y++){
									for (byte x=0; x<16; x++){
										notePitch[channelView][c][y][x] = funtionMatrix2[z];
									}
								}
							}
							noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=notePitch[channelView][pageView[channelView]][0][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
					} else if (setVolume == HIGH){ //change the notePitch
						if (setAll == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
							noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16] = funtionMatrix2[z];
							noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (set8 == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],channelView);
							for (int c=pageView[channelView]; c<256; c++){
								if (pageActive[channelView][c] == HIGH){
									for (byte x=0; x<16; x++){
										noteVolume[channelView][c][noteEdit][x] = funtionMatrix2[z];
									}
								}
							}
							noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],noteVolume[channelView][pageView[channelView]][noteEdit][0],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit][0]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][noteEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setPage == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
							for (int c=pageView[channelView]; c<256; c++){
								if (pageActive[channelView][c] == HIGH){
									for (byte y=0; y<8; y++){
										for (byte x=0; x<16; x++){
											noteVolume[channelView][c][y][x] = funtionMatrix2[z];
										}
									}
								}
							}
							noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][0][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						} else if (setChannel == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
							for (int c=0; c<256; c++){
								for (byte y=0; y<8; y++){
									for (byte x=0; x<16; x++){
										noteVolume[channelView][c][y][x] = funtionMatrix2[z];
									}
								}
							}
							noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
							noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][0][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
					} else if (setPattern1 == HIGH){ //to which 1-8 row should the pattern be set
						if (z == 0 || z == 4 || z == 8 || z == 12 || z == 64 || z == 68 || z == 72 || z == 76){
							for (byte c=0; c<128; c++){
									trellis.clrLED(c);
							}
							switch (z) {
								case 0:
									patternEdit1 = 0;
									break;
								case 4:
									patternEdit1 = 1;
									break;
								case 8:
									patternEdit1 = 2;
									break;
								case 12:
									patternEdit1 = 3;
									break;
								case 64:
									patternEdit1 = 4;
									break;
								case 68:
									patternEdit1 = 5;
									break;
								case 72:
									patternEdit1 = 6;
									break;
								case 76:
									patternEdit1 = 7;
									break;
							}
							for (byte c=0; c<16; c++){
								for (byte x=0; x<8; x++){
									if (c % (x+1) == 0){
										trellis.setLED(hardwareMatrix[x][c]);
									}
								}
							}
							trellis.writeDisplay();
							setPattern1 = LOW;
							setPattern2 = HIGH;
						}
					} else if (setPattern2 == HIGH){ //Set 1-8 pattern
						if (z == 0 || z == 4 || z == 8 || z == 12 || z == 64 || z == 68 || z == 72 || z == 76){
							switch (z) {
								case 0:
									patternEdit2 = 1;
									break;
								case 4:
									patternEdit2 = 2;
									break;
								case 8:
									patternEdit2 = 3;
									break;
								case 12:
									patternEdit2 = 4;
									break;
								case 64:
									patternEdit2 = 5;
									break;
								case 68:
									patternEdit2 = 6;
									break;
								case 72:
									patternEdit2 = 7;
									break;
								case 76:
									patternEdit2 = 8;
									break;
							}
							for (int c=pageView[channelView]; c<256; c++){
								if (pageActive[channelView][c] == HIGH){
									for (byte y=0; y<16; y++){
										if (y % patternEdit2 == 0){
											softwareMatrix[channelView][c][hardwareMatrix[patternEdit1][y]] = HIGH;
											noteOnTrigger[channelView][c][hardwareMatrix[patternEdit1][y]] = HIGH;
											noteOffTrigger[channelView][c][hardwareMatrix[patternEdit1][y]] = HIGH;
										} else {
											softwareMatrix[channelView][c][hardwareMatrix[patternEdit1][y]] = LOW;
											noteOnTrigger[channelView][c][hardwareMatrix[patternEdit1][y]] = LOW;
											noteOffTrigger[channelView][c][hardwareMatrix[patternEdit1][y]] = LOW;
										}
									}
								}
							}
							trellis.writeDisplay();
						}
					} else if (setMute == HIGH){ //Set Mute
						mute[funtionMatrix2[z]/16][funtionMatrix2[z]%16] = !mute[funtionMatrix2[z]/16][funtionMatrix2[z]%16];
						//mute[8][16];
						for (byte c=0; c<128; c++){
							trellis.clrLED(c);
						}
						for (byte c=0; c<8; c++){
							for (byte y=0; y<16; y++){
								if (mute[c][y] == LOW){
									trellis.setLED(hardwareMatrix[c][y]);
								}
							}
						}
						trellis.writeDisplay();
					} else if (setBPM == HIGH){
						BPM = funtionMatrix2[z] + 57;
						clockIntervall = 60.0/BPM*1000*1000/24.0;
						for (byte c=0; c<128; c++){
							if (c < BPM - 56){
								trellis.setLED(funtionMatrix1[c]);
							} else {
								trellis.clrLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
					} else if (setLatency == HIGH){
						latency[channelView] = funtionMatrix2[z];
						for (byte c=0; c<128; c++){
							if (c <= latency[channelView]){
								trellis.setLED(funtionMatrix1[c]);
							} else {
								trellis.clrLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
					} else if (setLongNotes == HIGH){
						if (softwareMatrix[channelView][pageView[channelView]][z] == HIGH){
							noteOnOff[channelView][pageView[channelView]][funtionMatrix2[z]/16][funtionMatrix2[z]%16] = !noteOnOff[channelView][pageView[channelView]][funtionMatrix2[z]/16][funtionMatrix2[z]%16];
							if (noteOnOff[channelView][pageView[channelView]][funtionMatrix2[z]/16][funtionMatrix2[z]%16] == LOW){
								noteOnTrigger[channelView][pageView[channelView]][z] = HIGH;
								noteOffTrigger[channelView][pageView[channelView]][z] = HIGH;
							}
						}
						for (int c=0; c<256; c++){
							if (pageActive[channelView][pageChange[c][pageView[channelView]]] == HIGH){
								pageNextB = pageChange[c][pageView[channelView]];
								break;
							}
						}
						for (byte c=0; c<8; c++){
							for (byte y=0; y<15; y++){
								if (noteOnOff[channelView][pageView[channelView]][c][y] == HIGH && noteOnOff[channelView][pageView[channelView]][c][pageChange[0][y]] == HIGH){
									noteOffTrigger[channelView][pageView[channelView]][hardwareMatrix[c][y]] = LOW;
									noteOnTrigger[channelView][pageView[channelView]][hardwareMatrix[c][pageChange[0][y]]] = LOW;
								}
							}
							if (noteOnOff[channelView][pageView[channelView]][c][15] == HIGH && noteOnOff[channelView][pageNextB][c][0] == HIGH){
								//noteOnVoid(100,100,0);
								noteOffTrigger[channelView][pageView[channelView]][hardwareMatrix[c][15]] = LOW;
								noteOnTrigger[channelView][pageNextB][hardwareMatrix[c][0]] = LOW;
							}
						}
						for (byte c=0; c<8; c++){
							for (byte y=0; y<15; y++){
								if (noteOnOff[channelView][pageView[channelView]][c][y] == HIGH && noteOnOff[channelView][pageView[channelView]][c][pageChange[0][y]] == LOW){
									noteOffTrigger[channelView][pageView[channelView]][hardwareMatrix[c][y]] = HIGH;
								} else if (noteOnOff[channelView][pageView[channelView]][c][y] == LOW && noteOnOff[channelView][pageView[channelView]][c][pageChange[0][y]] == HIGH){
									noteOnTrigger[channelView][pageView[channelView]][hardwareMatrix[c][pageChange[0][y]]] = HIGH;
								}
							}
							if (noteOnOff[channelView][pageView[channelView]][c][15] == HIGH && noteOnOff[channelView][pageNextB][c][0] == LOW){
								noteOffTrigger[channelView][pageView[channelView]][hardwareMatrix[c][15]] = HIGH;
							} else if (noteOnOff[channelView][pageView[channelView]][c][15] == LOW && noteOnOff[channelView][pageNextB][c][0] == HIGH){
								noteOnTrigger[channelView][pageNextB][hardwareMatrix[c][0]] = HIGH;
							}
						}
						for (byte c=0; c<8; c++){
							for (byte y=0; y<16; y++){
								if (noteOnOff[channelView][pageView[channelView]][c][y] == HIGH){
									trellis.setLED(hardwareMatrix[c][y]);
								} else {
									trellis.clrLED(hardwareMatrix[c][y]);
								}
							}
						}
						trellis.writeDisplay();
					} else if (copyPage == HIGH){
						if(funtionMatrix2[z] > 31 && funtionMatrix2[z] < 48){
							if(copyPageView[funtionMatrix2[z]-32] == LOW){
								copyPageView[funtionMatrix2[z]-32] = HIGH;
								trellis.setLED(z);
							} else {
								copyPageView[funtionMatrix2[z]-32] = LOW;
								trellis.clrLED(z);
							}
							trellis.writeDisplay();
						}
						if(funtionMatrix2[z] > 95 && funtionMatrix2[z] < 96+channelCount){
							if(copyPageChannel[funtionMatrix2[z]-96] == LOW){
								copyPageChannel[funtionMatrix2[z]-96] = HIGH;
								trellis.setLED(z);
							} else {
								copyPageChannel[funtionMatrix2[z]-96] = LOW;
								trellis.clrLED(z);
							}
							trellis.writeDisplay();
						}
						if(z == 76){
							for (byte c=0; c<16; c++){
								if(copyPageView[c] == HIGH){
									for (byte y=0; y<channelCount; y++){
										if(copyPageChannel[y] == HIGH){
											for (byte x=0; x<128; x++){
												softwareMatrix[y][c][x] = softwareMatrix[channelView][pageView[channelView]][x];
												noteOnTrigger[y][c][x] = noteOnTrigger[channelView][pageView[channelView]][x];
												noteOffTrigger[y][c][x] = noteOffTrigger[channelView][pageView[channelView]][x];
												
											}
											for (byte x=0; x<8; x++){
												for (byte a=0; a<16; a++){
													notePitch[y][c][x][a] = notePitch[channelView][pageView[channelView]][x][a];
													noteVolume[y][c][x][a] = noteVolume[channelView][pageView[channelView]][x][a];
													noteStart[y][c][x][a] = noteStart[channelView][pageView[channelView]][x][a];
													noteEnd[y][c][x][a] = noteEnd[channelView][pageView[channelView]][x][a];
												}
											}
										}
									}
								}
							}
							for (byte c=0; c<16; c++){
								copyPageView[c] = LOW;
							}
							for (byte y=0; y<channelCount; y++){
								copyPageChannel[y] = LOW;
							}
							copyPage = LOW;
							functionView();
						}
					} else if (playLive == HIGH){
						for (byte c=0; c<8; c++){
							if((z == hardwareMatrix[c][0] || z == hardwareMatrix[c][1]) && liveLowerNote[channelView] > 0){
								liveLowerNote[channelView] = liveLowerNote[channelView] - 12;
								playLiveView();
							} else if((z == hardwareMatrix[c][14] || z == hardwareMatrix[c][15]) && liveLowerNote[channelView] < 84){
								liveLowerNote[channelView] = liveLowerNote[channelView] + 12;
								playLiveView();
							}
						}
					} else if (setNoteStart == HIGH){ //change the noteStart
						if (setAll == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]){
								noteStart[channelView][pageView[channelView]][noteEdit/16][noteEdit%16] = funtionMatrix2[z];
								for (byte c=0; c<128; c++){
											trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							}
						} else if (set8 == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit][0]){
								for (int c=pageView[channelView]; c<256; c++){
									if (pageActive[channelView][c] == HIGH){
										for (byte x=0; x<16; x++){
											noteStart[channelView][c][noteEdit][x] = funtionMatrix2[z];
										}
									}
								}
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][noteEdit][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit][0];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							}
						} else if (setPage == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0]){
								for (int c=pageView[channelView]; c<256; c++){
									if (pageActive[channelView][c] == HIGH){
										for (byte y=0; y<8; y++){
											for (byte x=0; x<16; x++){
												noteStart[channelView][c][y][x] = funtionMatrix2[z];
											}
										}
									}
								}
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							}
						} else if (setChannel == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0]){
								for (int c=0; c<256; c++){
									for (byte y=0; y<8; y++){
										for (byte x=0; x<16; x++){
											noteStart[channelView][c][y][x] = funtionMatrix2[z];
										}
									}
								}
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							}
						}
						trellis.writeDisplay();
					} else if (setNoteEnd == HIGH){ //change the noteEnd
						if (setAll == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView] && funtionMatrix2[z] > noteStart[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]){
								noteEnd[channelView][pageView[channelView]][noteEdit/16][noteEdit%16] = noteLength[channelView]-funtionMatrix2[z];
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit/16][noteEdit%16];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							} 
						} else if (set8 == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView] && funtionMatrix2[z] > noteStart[channelView][pageView[channelView]][noteEdit][0]){
								for (int c=pageView[channelView]; c<256; c++){
									if (pageActive[channelView][c] == HIGH){
										for (byte x=0; x<16; x++){
											noteEnd[channelView][c][noteEdit][x] = noteLength[channelView]-funtionMatrix2[z];
										}
									}
								}
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][noteEdit][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][noteEdit][0];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							}
						} else if (setPage == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView] && funtionMatrix2[z] > noteStart[channelView][pageView[channelView]][0][0]){
								for (int c=pageView[channelView]; c<256; c++){
									if (pageActive[channelView][c] == HIGH){
										for (byte y=0; y<8; y++){
											for (byte x=0; x<16; x++){
												noteEnd[channelView][c][y][x] = noteLength[channelView]-funtionMatrix2[z];
											}
										}
									}
								}
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							}
						} else if (setChannel == HIGH){
							if (funtionMatrix2[z] < noteLength[channelView] && funtionMatrix2[z] > noteStart[channelView][pageView[channelView]][0][0]){
								for (int c=0; c<256; c++){
									for (byte y=0; y<8; y++){
										for (byte x=0; x<16; x++){
											noteEnd[channelView][c][y][x] = noteLength[channelView]-funtionMatrix2[z];
										}
									}
								}
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=noteStart[channelView][pageView[channelView]][0][0];c<=noteLength[channelView]-noteEnd[channelView][pageView[channelView]][0][0];c++){
									trellis.setLED(funtionMatrix1[c]);
								}
							}
						}
						trellis.writeDisplay();
					} else {
						switch (z) {
							case 0: //Set notePitch
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=12; c<16; c++){
										trellis.setLED(hardwareMatrix[7][c]);
									}
									trellis.writeDisplay();
									setRange1 = HIGH;
									setPitch = HIGH;
								}
								break;
							case 1: //Set noteVolume
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=12; c<16; c++){
										trellis.setLED(hardwareMatrix[7][c]);
									}
									trellis.writeDisplay();
									setRange1 = HIGH;
									setVolume = HIGH;
								}
								break;
							case 17: //Set Mute
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=0; c<8; c++){
									for (byte y=0; y<16; y++){
										if (mute[c][y] == LOW){
											trellis.setLED(hardwareMatrix[c][y]);
										}
									}
								}
								trellis.writeDisplay();
								setMute = HIGH;
								break;
							case 18: //Set Note Value to 4
								if (isPlay == LOW){
									if (noteTriplet[channelView] == LOW){
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													/*noteStart[channelView][a][b][c] = round(1/float(noteLength[channelView])*float(noteStart[channelView][a][b][c])*24);
													noteEnd[channelView][a][b][c] = round(1/float(noteLength[channelView])*float(noteEnd[channelView][a][b][c])*24);*/
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,23);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,24);
												}
											}
										}
										noteLength[channelView] = 24;
									} else {
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,15);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,16);
												}
											}
										}
										noteLength[channelView] = 16;
									}
									functionView();
								}
								break;
							case 19: //Set Note Value to 8
								if (isPlay == LOW){
									if (noteTriplet[channelView] == LOW){
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,11);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,12);
												}
											}
										}
										noteLength[channelView] = 12;
									} else {
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,7);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,8);
												}
											}
										}
										noteLength[channelView] = 8;
									}
									functionView();
								}
								break;
							case 32: //Set Note Value to 16
								if (isPlay == LOW){
									if (noteTriplet[channelView] == LOW){
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,5);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,6);
												}
											}
										}
										noteLength[channelView] = 6;
									} else {
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,3);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,4);
												}
											}
										}
										noteLength[channelView] = 4;
									}
									functionView();
								}
								break;
							case 33: //Set Note Value to 32
								if (isPlay == LOW){
									if (noteTriplet[channelView] == LOW){
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,2);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,3);
												}
											}
										}
										noteLength[channelView] = 3;
									} else {
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,1);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,2);
												}
											}
										}
										noteLength[channelView] = 2;
									}
									functionView();
								}
								break;
							case 34: //Invert Triplets
								if (isPlay == LOW){
									if (noteTriplet[channelView] == LOW){
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													/*noteStart[channelView][a][b][c] = round(1/float(noteLength[channelView])*float(noteStart[channelView][a][b][c])*(noteLength[channelView]*2/3));
													noteEnd[channelView][a][b][c] = round(1/float(noteLength[channelView])*float(noteEnd[channelView][a][b][c])*(noteLength[channelView]*2/3));*/
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,noteLength[channelView]*2/3-1);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,noteLength[channelView]*2/3);
												}
											}
										}
										noteLength[channelView] = noteLength[channelView]*2/3;
										noteTriplet[channelView] = HIGH;
										steps[channelView] = tripletStep[noteTriplet[channelView]];
									} else {
										for (byte a=0; a<16; a++){
											for (byte b=0; b<8; b++){
												for (byte c=0; c<16; c++){
													noteStart[channelView][a][b][c] = map(noteStart[channelView][a][b][c],0,noteLength[channelView]-1,0,noteLength[channelView]*3/2-1);
													noteEnd[channelView][a][b][c] = map(noteEnd[channelView][a][b][c],1,noteLength[channelView],1,noteLength[channelView]*3/2);
												}
											}
										}
										noteLength[channelView] = noteLength[channelView]*3/2;
										noteTriplet[channelView] = LOW;
									}
									functionView();
								}
								break;
							case 35: // Invert Clock Source
								isInternal = !isInternal;
								functionView();
								break;
							case 48: // Set BPM
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										if (c < BPM - 56){
											trellis.setLED(funtionMatrix1[c]);
										} else {
											trellis.clrLED(funtionMatrix1[c]);
										}
									}
									trellis.writeDisplay();
									setBPM = HIGH;
								}
								break;
							case 49: // Set Latenz
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										if (c <= latency[channelView]){
											trellis.setLED(funtionMatrix1[c]);
										} else {
											trellis.clrLED(funtionMatrix1[c]);
										}
									}
									trellis.writeDisplay();
									setLatency = HIGH;
								}
								break;
							case 50: // Set LongNotes
								for (byte c=0; c<8; c++){
									for (byte y=0; y<16; y++){
										if (noteOnOff[channelView][pageView[channelView]][c][y] == HIGH){
											trellis.setLED(hardwareMatrix[c][y]);
										} else {
											trellis.clrLED(hardwareMatrix[c][y]);
										}
									}
								}
								trellis.writeDisplay();
								setLongNotes = HIGH;
								break;
							case 51: // Invert LongNotes
								longNotes[channelView] = !longNotes[channelView];
								functionView();
								break;
							case 4: //Set pageView = 0
								setPageView1(0);
								break;
							case 5: //Set pageView = 1
								setPageView1(1);
								break;
							case 6: //Set pageView = 2
								setPageView1(2);
								break;
							case 7: //Set pageView = 3
								setPageView1(3);
								break;
							case 20: //Set pageView = 4
								setPageView1(4);
								break;
							case 21: //Set pageView = 5
								setPageView1(5);
								break;
							case 22: //Set pageView = 6
								setPageView1(6);
								break;
							case 23: //Set pageView = 7
								setPageView1(7);
								break;
							case 36: //Set pageView = 8
								setPageView1(8);
								break;
							case 37: //Set pageView = 9
								setPageView1(9);
								break;
							case 38: //Set pageView = 10
								setPageView1(10);
								break;
							case 39: //Set pageView = 11
								setPageView1(11);
								break;
							case 52: //Set pageView = 12
								setPageView1(12);
								break;
							case 53: //Set pageView = 13
								setPageView1(13);
								break;
							case 54: //Set pageView = 14
								setPageView1(14);
								break;
							case 55: //Set pageView = 15
								setPageView1(15);
								break;
							case 8: //Invert pageActive 0
								setPageActive1(0);
								break;
							case 9: //Invert pageActive 1
								setPageActive1(1);
								break;
							case 10: //Invert pageActive 2
								setPageActive1(2);
								break;
							case 11: //Invert pageActive 3
								setPageActive1(3);
								break;
							case 24: //Invert pageActive 4
								setPageActive1(4);
								break;
							case 25: //Invert pageActive 5
								setPageActive1(5);
								break;
							case 26: //Invert pageActive 6
								setPageActive1(6);
								break;
							case 27: //Invert pageActive 7
								setPageActive1(7);
								break;
							case 40: //Invert pageActive 8
								setPageActive1(8);
								break;
							case 41: //Invert pageActive 9
								setPageActive1(9);
								break;
							case 42: //Invert pageActive 10
								setPageActive1(10);
								break;
							case 43: //Invert pageActive 11
								setPageActive1(11);
								break;
							case 56: //Invert pageActive 12
								setPageActive1(12);
								break;
							case 57: //Invert pageActive 13
								setPageActive1(13);
								break;
							case 58: //Invert pageActive 14
								setPageActive1(14);
								break;
							case 59: //Invert pageActive 15
								setPageActive1(15);
								break;
							case 12: //Set pageView = 0
								setPageView2(0);
								break;
							case 13: //Set pageView = 1
								setPageView2(16);
								break;
							case 14: //Set pageView = 2
								setPageView2(32);
								break;
							case 15: //Set pageView = 3
								setPageView2(48);
								break;
							case 28: //Set pageView = 4
								setPageView2(64);
								break;
							case 29: //Set pageView = 5
								setPageView2(80);
								break;
							case 30: //Set pageView = 6
								setPageView2(96);
								break;
							case 31: //Set pageView = 7
								setPageView2(112);
								break;
							case 44: //Set pageView = 8
								setPageView2(128);
								break;
							case 45: //Set pageView = 9
								setPageView2(144);
								break;
							case 46: //Set pageView = 10
								setPageView2(160);
								break;
							case 47: //Set pageView = 176
								setPageView2(176);
								break;
							case 60: //Set pageView = 192
								setPageView2(192);
								break;
							case 61: //Set pageView = 208
								setPageView2(208);
								break;
							case 62: //Set pageView = 224
								setPageView2(224);
								break;
							case 63: //Set pageView = 240
								setPageView2(240);
								break;
							case 64: //Set pageView = 0
								setPageActive2(0);
								break;
							case 65: //Set pageView = 1
								setPageActive2(16);
								break;
							case 66: //Set pageView = 2
								setPageActive2(32);
								break;
							case 67: //Set pageView = 3
								setPageActive2(48);
								break;
							case 80: //Set pageView = 4
								setPageActive2(64);
								break;
							case 81: //Set pageView = 5
								setPageActive2(80);
								break;
							case 82: //Set pageView = 6
								setPageActive2(96);
								break;
							case 83: //Set pageView = 7
								setPageActive2(112);
								break;
							case 96: //Set pageView = 8
								setPageActive2(128);
								break;
							case 97: //Set pageView = 9
								setPageActive2(144);
								break;
							case 98: //Set pageView = 10
								setPageActive2(160);
								break;
							case 99: //Set pageView = 176
								setPageActive2(176);
								break;
							case 112: //Set pageView = 192
								setPageActive2(192);
								break;
							case 113: //Set pageView = 208
								setPageActive2(208);
								break;
							case 114: //Set pageView = 224
								setPageActive2(224);
								break;
							case 115: //Set pageView = 240
								setPageActive2(240);
								break;
							case 68: //Set channelView = 0
								channelView = 0;
								functionView();
								break;
							case 69: //Set channelView = 1
								channelView = 1;
								functionView();
								break;
							case 70: //Set channelView = 2
								channelView = 2;
								functionView();
								break;
							case 71: //Set channelView = 3
								channelView = 3;
								functionView();
								break;
							case 84: //Set channelView = 4
								channelView = 4;
								functionView();
								break;
							case 85: //Set channelView = 5
								channelView = 5;
								functionView();
								break;
							case 86: //Set channelView = 6
								channelView = 6;
								functionView();
								break;
							case 87: //Set channelView = 7
								channelView = 7;
								functionView();
								break;
							case 100: //Set channelView = 8
								channelView = 8;
								functionView();
								break;
							case 101: //Set channelView = 9
								channelView = 9;
								functionView();
								break;
							case 102: //Set channelView = 10
								channelView = 10;
								functionView();
								break;
							case 103: //Set channelView = 11
								channelView = 11;
								functionView();
								break;
							case 116: //Set channelView = 12
								channelView = 12;
								functionView();
								break;
							case 117: //Set channelView = 13
								channelView = 13;
								functionView();
								break;
							case 118: //Set channelView = 14
								channelView = 14;
								functionView();
								break;
							case 119: //Set channelView = 15
								channelView = 15;
								functionView();
								break;
							case 72: //Invert Channel Active 0
								setChannelActive(0);
								break;
							case 73: //Invert Channel Active 1
								setChannelActive(1);
								break;
							case 74: //Invert Channel Active 2
								setChannelActive(2);
								break;
							case 75: //Invert Channel Active 3
								setChannelActive(3);
								break;
							case 88: //Invert Channel Active 4
								setChannelActive(4);
								break;
							case 89: //Invert Channel Active 5
								setChannelActive(5);
								break;
							case 90: //Invert Channel Active 6
								setChannelActive(6);
								break;
							case 91: //Invert Channel Active 7
								setChannelActive(7);
								break;
							case 104: //Invert Channel Active 8
								setChannelActive(8);
								break;
							case 105: //Invert Channel Active 9
								setChannelActive(9);
								break;
							case 106: //Invert Channel Active 10
								setChannelActive(10);
								break;
							case 107: //Invert Channel Active 11
								setChannelActive(11);
								break;
							case 120: //Invert Channel Active 12
								setChannelActive(12);
								break;
							case 121: //Invert Channel Active 13
								setChannelActive(13);
								break;
							case 122: //Invert Channel Active 14
								setChannelActive(14);
								break;
							case 123: //Invert Channel Active 15
								setChannelActive(15);
								break;
							case 76: //Copy
								for (byte x=0; x<16; x++){
									trellis.clrLED(hardwareMatrix[0][x]);
									trellis.clrLED(hardwareMatrix[2][x]);
									trellis.clrLED(hardwareMatrix[4][x]);
								}
								for (byte x=0; x<channelCount; x++){
									trellis.clrLED(hardwareMatrix[6][x]);
								}
								for (byte x=1; x<16; x++){
									trellis.clrLED(hardwareMatrix[7][x]);
								}
								trellis.writeDisplay();
								copyPage = HIGH;
								break;
							case 77: // Play Live
								playLiveView();
								playLiveTest = HIGH;
								playLive = HIGH;
								break;
							case 78: // Record Live
								playLiveView();
								playLive = HIGH;
								break;
							case 79: // Mono Step Record
								if (isPlay == LOW){
									if (monoStepRecord == HIGH){
										stepRecordStep++;
										if (stepRecordStep == 16){
											stepRecordStep = 0;
											for (int i=0; i<256; i++){
												if (pageActive[channelView][pageChange[i][pageView[channelView]]] == HIGH) {
													pageView[channelView] = pageChange[i][pageView[channelView]];
													break;
												}
											}
										}
									} else {
										monoStepRecord = HIGH;
									}
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte x=0; x<=stepRecordStep; x++){
										if (softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[0][x]] == HIGH || x==stepRecordStep){
											trellis.setLED(hardwareMatrix[0][x]);
										}
									}
									trellis.setLED(hardwareMatrix[7][3]);
									trellis.writeDisplay();
								}
								break;
							case 92: // Chord Step Record
								if (isPlay == LOW){
									if (chordStepRecord == HIGH){
										stepRecordStep++;
										if (stepRecordStep == 16){
											stepRecordStep = 0;
											for (int i=0; i<256; i++){
												if (pageActive[channelView][pageChange[i][pageView[channelView]]] == HIGH) {
													pageView[channelView] = pageChange[i][pageView[channelView]];
													break;
												}
											}
										}
									} else {
										chordStepRecord = HIGH;
									}
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte x=0; x<=stepRecordStep; x++){
										if (softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[0][x]] == HIGH || x==stepRecordStep){
											trellis.setLED(hardwareMatrix[0][x]);
										}
									}
									trellis.setLED(hardwareMatrix[7][4]);
									trellis.writeDisplay();
								}
								break;
							case 93: // Set Note Start
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=12; c<16; c++){
										trellis.setLED(hardwareMatrix[7][c]);
									}
									trellis.writeDisplay();
									setRange1 = HIGH;
									setNoteStart = HIGH;
								}
								break;
							case 94: // Set Note End
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=12; c<16; c++){
										trellis.setLED(hardwareMatrix[7][c]);
									}
									trellis.writeDisplay();
									setRange1 = HIGH;
									setNoteEnd = HIGH;
								}
								break;
							case 95: // Set mooveIn
								mooveIn = !mooveIn;
								functionView();
								break;
							case 108: //Set Pattern
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=0; c<8; c++){
									trellis.setLED(hardwareMatrix[c][0]);
								}
								trellis.writeDisplay();
								setPattern1 = HIGH;
								break;
						}
					}
				}
				if (playLive == HIGH){
					if (trellis.justPressed(z)){
						if (liveNotes[funtionMatrix2[z]] != 50){
							noteOnVoid(liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView], 100, channelView);
							if (isPlay == HIGH && playLiveTest == LOW){
								liveIsPressed[liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView]] = 100;
								for (eightRows=0; eightRows<8; eightRows++){
									if (notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] == liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView] && softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == HIGH){
										freeLiveNotes = LOW;
									}
								}
								if (freeLiveNotes == HIGH){
									for (eightRows=0; eightRows<8; eightRows++){
										if (softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == LOW){
											noteStart[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = (beatClockCounter + noteLength[channelView] - 1) % noteLength[channelView];
											noteOnTrigger[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
											softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
											notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView];
											noteVolume[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = 100;
											noteOnVolume[liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView]] = 100;
											break;
										}
									}
								}
							}
						}
					}
					if (trellis.justReleased(z)){
						if (liveNotes[funtionMatrix2[z]] != 50){
							noteOffVoid(liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView], channelView);
							if (isPlay == HIGH && playLiveTest == LOW){
								//liveIsUnPressed[liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView]] = HIGH;
								liveIsPressed[liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView]] = 0;
								for (eightRows=0; eightRows<8; eightRows++){
									if (notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] == liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView] && softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == HIGH){
										freeLiveNotes = LOW;
									}
								}
								if (freeLiveNotes == HIGH){
									for (eightRows=0; eightRows<8; eightRows++){
										if (softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == LOW){
											softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
											notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView];
											noteVolume[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = noteOnVolume[liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView]];
											noteOnVolume[liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView]] = 0;
											break;
										}
									}
								}
								freeLiveNotes = HIGH;
								for (eightRows=0; eightRows<8; eightRows++){
									if(notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] == liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView] && softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == HIGH){
										noteEnd[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = clock2[channelView] + noteLength[channelView] - beatClockCounter;
										noteOffTrigger[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
									}
								}
							}
						}
					}
				}
			}
		}
		trellisTime = millis();
	}
	if (bounceFunction.update()){
		if (bounceFunction.fallingEdge()){
			if (function == LOW){
				function = HIGH;
				functionView();
			} else {
				for (byte i=0; i<128; i++){
					for (byte x=0; x<16; x++){
						if (noteStopFunction[x][i] == HIGH){
							noteOffVoid(i,x);
						}
					}
					if (softwareMatrix[channelView][pageView[channelView]][i] == LOW){
						trellis.clrLED(i);
					} else {
						trellis.setLED(i);
					}
				}
				trellis.writeDisplay();
				function = LOW;
				setRange1 = LOW;
				setRange2 = LOW;
				setAll = LOW;
				set8 = LOW;
				setPage = LOW;
				setChannel = LOW;
				setPitch = LOW;
				setVolume = LOW;
				setNoteStart = LOW;
				setNoteEnd = LOW;
				setPattern1 = LOW;
				setPattern2 = LOW;
				setMute = LOW;
				setBPM = LOW;
				setLatency = LOW;
				setLongNotes = LOW;
				copyPage = LOW;
				playLive = LOW;
				playLiveTest = LOW;
				monoStepRecord = LOW;
				chordStepRecord = LOW;
				stepRecordStep = 0;
			}
		}
	}
	if (isPlay == HIGH){
		for (byte i=0; i<channelCount; i++){
			for (byte y=0; y<8; y++){
				if (playLater[i][y] == HIGH) {
					if (millis() - playLaterMillis[i][y] > max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))-latency[i]){
						noteOnVoid(playLaterNotePitch[i][y], playLaternoteVolume[i][y], i);
						playLater[i][y] = LOW;
					}
				}
				if (playLaterOff[i][y] == HIGH) {
					if (millis() - playLaterOffMillis[i][y] > max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))-latency[i]){
						noteOffVoid(playLaterOffNote[i][y], i);
						playLaterOff[i][y] = LOW;
					}
				}
			}
		}
	}
	if (isInternal == HIGH && isPlay == HIGH) {
		if (micros() >= makeClock2){
			clockCounter++;
			makeClock2 = makeClock1 + (clockIntervall * clockCounter);
			beatClock(CLOCK);
		}
	}
	if (isInternal == LOW && isClock == HIGH) {
		if((millis() - clockTime > clockDebounce)){ //If the external clock stopped
			stopPlay();
		}
	}
	if (bouncePlay.update() && isInternal == HIGH && setPitch == LOW && setVolume == LOW && setBPM == LOW && setLatency == LOW && setLongNotes == LOW && monoStepRecord == LOW && chordStepRecord == LOW && setNoteStart == LOW && setNoteEnd == LOW){
		if (bouncePlay.fallingEdge()){
			//digitalWrite(led, LOW);
			makeClock1 = micros();
			isPlay = !isPlay;
			if (isPlay == LOW){
				stopPlay();
				//digitalWrite(led, LOW);
			} else {
				makeClock1 = micros();
				clockCounter++;
				makeClock2 = makeClock1 + (clockIntervall * clockCounter);
				beatClock(CLOCK);
			}
		}
	}
	for (byte c=0; c<3; c++){
		if (usbTransfer[0] == 1 && millis() - usbTime > usbDebounce && usbTransfer[1] == 0){
			usbMIDI.sendNoteOff(usbTransfer[3], 0, usbTransfer[2]);
			usbTransfer[0] = 0;
		} else if (usbTransfer[0] == 1 && millis() - usbTime > usbDebounce && usbTransfer[1] == 1){
			usbMIDI.sendNoteOn(usbTransfer[3], usbTransfer[4], usbTransfer[2]);
			usbTransfer[0] = 0;
		}
	}
	usbMIDI.read();
	if (MIDIInput.read()) {                    // Is there a MIDI message incoming ?
		MIDIByte = MIDIInput.getType();
		switch (MIDIByte) {
			case midi::NoteOn:
				dataByte1 = MIDIInput.getData1();
				dataByte2 = MIDIInput.getData2();
				if (chordStepRecord == LOW && setPitch == LOW && setVolume == LOW){
					noteOnVoid(dataByte1, dataByte2, channelView);
				}
				if (isPlay == HIGH && playLiveTest == LOW && playLive == HIGH){
					liveIsPressed[dataByte1] = dataByte2;
					for (eightRows=0; eightRows<8; eightRows++){
						if (notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] == dataByte1 && softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == HIGH){
							freeLiveNotes = LOW;
						}
					}
					if (freeLiveNotes == HIGH){
						for (eightRows=0; eightRows<8; eightRows++){
							if (softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == LOW){
								noteStart[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = (beatClockCounter + noteLength[channelView] - 1) % noteLength[channelView];
								noteOnTrigger[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
								softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
								notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = dataByte1;
								noteVolume[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = dataByte2;
								noteOnVolume[dataByte1] = dataByte2;
								break;
							}
						}
					}
					freeLiveNotes = HIGH;
					//liveRecordPressed(dataByte1, dataByte2);
				}
				if (setPitch == HIGH && set8 == HIGH && setRange1 == LOW && setRange2 == LOW){
					noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],channelView);
					for (int c=pageView[channelView]; c<256; c++){
						if (pageActive[channelView][c] == HIGH){
							for (byte x=0; x<16; x++){
									notePitch[channelView][c][noteEdit][x] = dataByte1;
							}
						}
					}
					noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],noteVolume[channelView][pageView[channelView]][noteEdit][0],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit][0]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=notePitch[channelView][pageView[channelView]][noteEdit][0]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					
					trellis.writeDisplay();
				}
				if (setPitch == HIGH && setAll == HIGH && setRange1 == LOW && setRange2 == LOW){ //change the notePitch
					noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
					notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16] = dataByte1;
					noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					
					trellis.writeDisplay();
				}
				if (setPitch == HIGH && setPage == HIGH && setRange1 == LOW && setRange2 == LOW){ //change the noteVolume
					noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
					for (int c=pageView[channelView]; c<256; c++){
						if (pageActive[channelView][c] == HIGH){
							for (byte y=0; y<8; y++){
								for (byte x=0; x<16; x++){
									notePitch[channelView][c][y][x] = dataByte1;
								}
							}
						}
					}
					noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=notePitch[channelView][pageView[channelView]][0][0]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					trellis.writeDisplay();
				}
				if (setPitch == HIGH && setChannel == HIGH && setRange1 == LOW && setRange2 == LOW){ //change the noteVolume
					noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
					for (int c=0; c<256; c++){
						for (byte y=0; y<8; y++){
							for (byte x=0; x<16; x++){
								notePitch[channelView][c][y][x] = dataByte1;
							}
						}
					}
					noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=notePitch[channelView][pageView[channelView]][0][0]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					trellis.writeDisplay();
				}
				if (setVolume == HIGH && set8 == HIGH && setRange1 == LOW && setRange2 == LOW){ //change the noteVolume
					noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],channelView);
					for (int c=pageView[channelView]; c<256; c++){
						if (pageActive[channelView][c] == HIGH){
							for (byte x=0; x<16; x++){
								noteVolume[channelView][c][noteEdit][x] = dataByte1;
							}
						}
					}
					noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit][0],noteVolume[channelView][pageView[channelView]][noteEdit][0],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit][0]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][noteEdit][0]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					
					trellis.writeDisplay();
				}
				if (setVolume == HIGH && setAll == HIGH && setRange1 == LOW && setRange2 == LOW){ //change the noteVolume
					noteOffVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
					noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16] = dataByte1;
					noteOnVoid(notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][noteEdit/16][noteEdit%16]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					trellis.writeDisplay();
				}
				if (setVolume == HIGH && setPage == HIGH && setRange1 == LOW && setRange2 == LOW){ //change the noteVolume
					noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
					for (int c=pageView[channelView]; c<256; c++){
						if (pageActive[channelView][c] == HIGH){
							for (byte y=0; y<8; y++){
								for (byte x=0; x<16; x++){
									noteVolume[channelView][c][y][x] = dataByte1;
								}
							}
						}
					}
					noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][0][0]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					trellis.writeDisplay();
				}
				if (setVolume == HIGH && setChannel == HIGH && setRange1 == LOW && setRange2 == LOW){ //change the noteVolume
					noteOffVoid(notePitch[channelView][pageView[channelView]][0][0],channelView);
					for (int c=0; c<256; c++){
						for (byte y=0; y<8; y++){
							for (byte x=0; x<16; x++){
								noteVolume[channelView][c][y][x] = dataByte1;
							}
						}
					}
					noteOnVoid(notePitch[channelView][pageView[channelView]][0][0],noteVolume[channelView][pageView[channelView]][0][0],channelView);
					noteStopFunction[channelView][notePitch[channelView][pageView[channelView]][0][0]] = HIGH;
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte c=0; c<=noteVolume[channelView][pageView[channelView]][0][0]; c++){
						trellis.setLED(funtionMatrix1[c]);
					}
					trellis.writeDisplay();
				}
				if (monoStepRecord == HIGH){
					for (eightRows=0; eightRows<8; eightRows++){
						if (softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] == LOW){
							noteOnTrigger[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] = HIGH;
							noteOffTrigger[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] = HIGH;
							softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] = HIGH;
							notePitch[channelView][pageView[channelView]][eightRows][stepRecordStep] = dataByte1;
							noteVolume[channelView][pageView[channelView]][eightRows][stepRecordStep] = dataByte2;
							break;
						}
					}
					stepRecordStep++;
					if (stepRecordStep == 16){
						stepRecordStep = 0;
						for (int i=0; i<256; i++){
							if (pageActive[channelView][pageChange[i][pageView[channelView]]] == HIGH) {
								pageView[channelView] = pageChange[i][pageView[channelView]];
								break;
							}
						}
					}
					for (byte c=0; c<128; c++){
						trellis.clrLED(c);
					}
					for (byte x=0; x<=stepRecordStep; x++){
						if (softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[0][x]] == HIGH || x==stepRecordStep){
							trellis.setLED(hardwareMatrix[0][x]);
						}
					}
					trellis.setLED(hardwareMatrix[7][3]);
					trellis.writeDisplay();
				}
				if (chordStepRecord == HIGH){
					for (eightRows=0; eightRows<8; eightRows++){
						if (softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] == LOW){
							noteOnTrigger[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] = HIGH;
							noteOffTrigger[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] = HIGH;
							softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] = HIGH;
							notePitch[channelView][pageView[channelView]][eightRows][stepRecordStep] = dataByte1;
							noteVolume[channelView][pageView[channelView]][eightRows][stepRecordStep] = dataByte2;
							break;
						}
					}
					for (eightRows=0; eightRows<8; eightRows++){
						if (softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] == HIGH){
							noteOnVoid(notePitch[channelView][pageView[channelView]][eightRows][stepRecordStep],noteVolume[channelView][pageView[channelView]][eightRows][stepRecordStep],channelView);
						}
					}
				}
				break;
			case midi::NoteOff:
				dataByte1 = MIDIInput.getData1();
				dataByte2 = MIDIInput.getData2();
				if (chordStepRecord == LOW && setPitch == LOW && setVolume == LOW){
					noteOffVoid(dataByte1, channelView);
				}
				if (isPlay == HIGH && playLiveTest == LOW && playLive == HIGH){
					liveIsPressed[dataByte1] = 0;
					//liveIsUnPressed[dataByte1] = HIGH;
					for (eightRows=0; eightRows<8; eightRows++){
						if (notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] == dataByte1 && softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == HIGH){
							freeLiveNotes = LOW;
						}
					}
					if (freeLiveNotes == HIGH){
						for (eightRows=0; eightRows<8; eightRows++){
							if (softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == LOW){
								softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
								notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = dataByte1;
								noteVolume[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = noteOnVolume[dataByte1];
								noteOnVolume[dataByte1] = 0;
								break;
							}
						}
					}
					freeLiveNotes = HIGH;
					for (eightRows=0; eightRows<8; eightRows++){
						if(notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] == dataByte1 && softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == HIGH){
							noteEnd[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = clock2[channelView] + noteLength[channelView] - beatClockCounter;
							noteOffTrigger[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
						}
					}
					//liveRecordUnPressed(dataByte1);
				}
				if (chordStepRecord == HIGH){
					for (eightRows=0; eightRows<8; eightRows++){
						if (softwareMatrix[channelView][pageView[channelView]][hardwareMatrix[eightRows][stepRecordStep]] == HIGH){
							noteOffVoid(notePitch[channelView][pageView[channelView]][eightRows][stepRecordStep],channelView);
						}
					}
				}
				break;
		}
	}
}

void usbBeatClock(byte realtimebyte){
	if (isInternal == LOW && setPitch == LOW && setVolume == LOW && setBPM == LOW && setLatency == LOW && setLongNotes == LOW && monoStepRecord == LOW && chordStepRecord == LOW && setNoteStart == LOW && setNoteEnd == LOW){
		beatClock(realtimebyte);
	}
}

void beatClock(byte realtimebyte){
	//if(realtimebyte == START || realtimebyte == CONTINUE){beatClockCounter = 0;}
	if(realtimebyte == CLOCK){
		isClock = HIGH;
		clockTime = millis();
		beatClockCounter++;
		channelPlay2 = 0;
		for(byte i=0; i<16; i++) latencyPlay[i] = latency[i] + 1;
		if (beatClockCounter == 49){beatClockCounter = 1;}		 //Eine Viertelnote ist vorbei
		for (channelPlay1 = 0; channelPlay1<channelCount; channelPlay1++){
			if (latencyPlay[channelPlay1] == max(latencyPlay[0], max(latencyPlay[1], max(latencyPlay[2], max(latencyPlay[3], max(latencyPlay[4], max(latencyPlay[5], max(latencyPlay[6], max(latencyPlay[7], max(latencyPlay[8], max(latencyPlay[9], max(latencyPlay[10], max(latencyPlay[11], max(latencyPlay[12], max(latencyPlay[13], max(latencyPlay[14], latencyPlay[15])))))))))))))))) {
				mooveInCounter[channelPlay1]++;
				if (mooveInCounter[channelPlay1] == noteLength[channelPlay1] * (tripletStep[noteTriplet[channelPlay1]]+1)){
					mooveInCounter[channelPlay1] = 0;
					if (mooveInChannel[channelPlay1] == HIGH){
						mooveInChannel[channelPlay1] = LOW;
					}
				}
				beatClock2(channelPlay1);
				latencyPlay[channelPlay1] = 0;
				channelPlay2++;
				channelPlay1 = -1;
			}
			if (channelPlay2 == channelCount) {
				break;
			}
		}
	}
}

void beatClock2(byte channelPlay){
	if (steps[channelPlay] == tripletStep[noteTriplet[channelPlay]] && beatClockCounter == 1){
		if (mooveIn == HIGH && channelActive[channelPlay] == LOW){
			
		} else {
			if (mooveInChannel[channelPlay] == HIGH){
				mooveInChannel[channelPlay] = LOW;
			}
			for (int i=0; i<256; i++){
				if (pageActive[channelPlay][pageChange[i][pagePlay[channelPlay]]] == HIGH) {
					if (pagePlay[channelPlay] == pageView[channelPlay]){
						pageView[channelPlay] = pageChange[i][pageView[channelPlay]];
					}
					pagePlay[channelPlay] = pageChange[i][pagePlay[channelPlay]];
					break;
				}
			}
			if (channelPlay == channelView && function == HIGH && setPattern1 == LOW && setPattern2 == LOW && setMute == LOW && copyPage == LOW && playLive == LOW && playLiveTest == LOW){
				for (byte i=0; i<16; i++){
					trellis.clrLED(hardwareMatrix[1][i]);
					trellis.clrLED(hardwareMatrix[2][i]);
					trellis.clrLED(hardwareMatrix[3][i]);
					trellis.clrLED(hardwareMatrix[4][i]);
				}
				for (int x=0; x<256; x++){
					if (pageView[channelView] == x){
						trellis.setLED(hardwareMatrix[1][x%16]);
						trellis.setLED(hardwareMatrix[3][x/16]);
					}
					if (pageActive[channelView][x]==HIGH){
						if (pageView[channelView]/16 == x/16){
							trellis.setLED(hardwareMatrix[2][x%16]);
						}
						trellis.setLED(hardwareMatrix[4][x/16]);
					}
				}
				trellis.writeDisplay();
			}
		}
	}
	for (byte i=0; i<16; i++){
		if (pageActive[channelPlay][pageChange[i][pagePlay[channelPlay]]] == HIGH){
			pageNext = i;
			break;
		}
	}
	for (int i=pagePlay[channelPlay] - 1; i<256 && i>=0; i--){
		if (pageActive[channelPlay][i] == HIGH){
			pagePre = i;
			break;
		}
	}
	for (int i=0; i<256; i++){
		if (pageActive[channelPlay][i] == HIGH){
			pageFirst = i;
			break;
		}
	}
	for (int i=15; i<256 && i>=0; i--){
		if (pageActive[channelPlay][i] == HIGH){
			pageLast = i;
			break;
		}
	}
	if ((beatClockCounter + noteLength[channelPlay] - 1) % noteLength[channelPlay] == 0){
		clock2[channelPlay] = beatClockCounter;
		if (steps[channelPlay] < tripletStep[noteTriplet[channelPlay]]){
			steps[channelPlay]++;
		} else {
			steps[channelPlay] = 0;
		}
		if (function == LOW && channelPlay == channelView && pagePlay[channelPlay] == pageView[channelPlay]){
			for (byte i=0; i<128; i++){
				if (softwareMatrix[channelView][pageView[channelView]][i] == LOW){
					trellis.clrLED(i);
				} else {
					trellis.setLED(i);
				}
			}
			for (eightRows=0; eightRows<8; eightRows++){
				if (softwareMatrix[channelPlay][pagePlay[channelPlay]][hardwareMatrix[eightRows][steps[channelPlay]]] == HIGH){
					trellis.clrLED(hardwareMatrix[eightRows][steps[channelPlay]]);
				} else {
					trellis.setLED(hardwareMatrix[eightRows][steps[channelPlay]]);
				}
			}
			trellis.writeDisplay();
		}
	}
	if (channelActive[channelPlay] == HIGH && mooveInChannel[channelPlay] == LOW){
		for (eightRows=0; eightRows<8; eightRows++){
			if (pageActive[channelPlay][pagePlay[channelPlay]] == HIGH && mute[eightRows][channelPlay] == LOW){
				if (softwareMatrix[channelPlay][pagePlay[channelPlay]][hardwareMatrix[eightRows][steps[channelPlay]]] == HIGH && (noteOnTrigger[channelPlay][pagePlay[channelPlay]][hardwareMatrix[eightRows][steps[channelPlay]]] == HIGH || startNote[channelPlay][pagePlay[channelPlay]][eightRows] == HIGH)){	
					if ((beatClockCounter + noteLength[channelPlay] - 1) % noteLength[channelPlay] == noteStart[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]){
						if (longNotes[channelPlay] == LOW){
							if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
								noteOnVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								startNote[channelPlay][pagePlay[channelPlay]][eightRows] = LOW;
							} else {
								playLater[channelPlay][eightRows] = HIGH;
								playLaterMillis[channelPlay][eightRows] = millis();
								playLaterNotePitch[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								playLaternoteVolume[channelPlay][eightRows] = noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
							}
						} else if ((softwareMatrix[channelPlay][pagePlay[channelPlay]][hardwareMatrix[eightRows][steps[channelPlay]-1]] == LOW || notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]-1] != notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]) && steps[channelPlay] > 0){
							if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
								noteOnVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								startNote[channelPlay][pagePlay[channelPlay]][eightRows] = LOW;
							} else {
								playLater[channelPlay][eightRows] = HIGH;
								playLaterMillis[channelPlay][eightRows] = millis();
								playLaterNotePitch[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								playLaternoteVolume[channelPlay][eightRows] = noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
							}
						} else if ((softwareMatrix[channelPlay][pagePre][hardwareMatrix[eightRows][15]] == LOW || notePitch[channelPlay][pagePre][eightRows][15] != notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]) && steps[channelPlay] == 0 && pagePlay[channelPlay] > pageFirst){
							if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
								noteOnVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								startNote[channelPlay][pagePlay[channelPlay]][eightRows] = LOW;
							} else {
								playLater[channelPlay][eightRows] = HIGH;
								playLaterMillis[channelPlay][eightRows] = millis();
								playLaterNotePitch[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								playLaternoteVolume[channelPlay][eightRows] = noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
							}
						} else if ((softwareMatrix[channelPlay][15][hardwareMatrix[eightRows][pageLast]] == LOW || notePitch[channelPlay][pageLast][eightRows][15] != notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]) && steps[channelPlay] == 0 && pagePlay[channelPlay] == pageFirst){
							if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
								noteOnVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								startNote[channelPlay][pagePlay[channelPlay]][eightRows] = LOW;
							} else {
								playLater[channelPlay][eightRows] = HIGH;
								playLaterMillis[channelPlay][eightRows] = millis();
								playLaterNotePitch[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								playLaternoteVolume[channelPlay][eightRows] = noteVolume[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
							}
						}
					}
				}
			}
		}
	}
	if (channelActive[channelPlay] == HIGH && mooveInChannel[channelPlay] == LOW){
		if(beatClockCounter == clock2[channelPlay] + noteLength[channelPlay] - 1){
			freeLiveNotes = HIGH;
			for (byte i=0; i<128; i++){
				if (liveIsPressed[i] > 0){
					liveRecord(i, liveIsPressed[i]);
				}
			}
		}
	}
	for (eightRows=0; eightRows<8; eightRows++){
		if (pageActive[channelPlay][pagePlay[channelPlay]] == HIGH){
			if (mute[eightRows][channelPlay] == LOW || noteStop[channelPlay][notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]] == HIGH){
				if (softwareMatrix[channelPlay][pagePlay[channelPlay]][hardwareMatrix[eightRows][steps[channelPlay]]] == HIGH && noteOffTrigger[channelPlay][pagePlay[channelPlay]][hardwareMatrix[eightRows][steps[channelPlay]]] == HIGH){
					if (beatClockCounter == clock2[channelPlay] + noteLength[channelPlay] - noteEnd[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]){
						if (longNotes[channelPlay] == LOW){
								if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
									noteOffVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][eightRows] = HIGH;
									playLaterOffMillis[channelPlay][eightRows] = millis();
									playLaterOffNote[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								}
						} else if (channelPlay == channelView && liveIsPressed[notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]]] > 0){
							
						} else {
							if ((softwareMatrix[channelPlay][pagePlay[channelPlay]][hardwareMatrix[eightRows][steps[channelPlay]+1]] == LOW || notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]+1] != notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]) && steps[channelPlay] < 15){
								if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
									noteOffVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][eightRows] = HIGH;
									playLaterOffMillis[channelPlay][eightRows] = millis();
									playLaterOffNote[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								}
							} else if ((softwareMatrix[channelPlay][pageNext][hardwareMatrix[eightRows][0]] == LOW || notePitch[channelPlay][pageNext][eightRows][0] != notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]) && steps[channelPlay] == 15 && pagePlay[channelPlay] < pageLast){
								if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
									noteOffVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][eightRows] = HIGH;
									playLaterOffMillis[channelPlay][eightRows] = millis();
									playLaterOffNote[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								}
							} else if ((softwareMatrix[channelPlay][0][hardwareMatrix[eightRows][pageFirst]] == LOW || notePitch[channelPlay][pageFirst][eightRows][0] != notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]]) && steps[channelPlay] == 15 && pagePlay[channelPlay] == pageLast){
								if (latency[channelPlay] == max(latency[0], max(latency[1], max(latency[2], max(latency[3], max(latency[4], max(latency[5], max(latency[6], max(latency[7], max(latency[8], max(latency[9], max(latency[10], max(latency[11], max(latency[12], max(latency[13], max(latency[14], latency[15])))))))))))))))){
									noteOffVoid(notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][eightRows] = HIGH;
									playLaterOffMillis[channelPlay][eightRows] = millis();
									playLaterOffNote[channelPlay][eightRows] = notePitch[channelPlay][pagePlay[channelPlay]][eightRows][steps[channelPlay]];
								}
							}
						}
					}
				}
			}
		}
	}
}
void functionView(){
	for (byte c=0; c<128; c++){
		trellis.clrLED(c);
	}
	trellis.setLED(hardwareMatrix[0][0]);
	trellis.setLED(hardwareMatrix[0][1]);
	trellis.setLED(hardwareMatrix[0][5]);
	if (noteLength[channelView] == 24 || noteLength[channelView] == 16){
		trellis.setLED(hardwareMatrix[0][6]);
	} else if (noteLength[channelView] == 12 || noteLength[channelView] == 8){
		trellis.setLED(hardwareMatrix[0][7]);
	} else if (noteLength[channelView] == 6 || noteLength[channelView] == 4){
		trellis.setLED(hardwareMatrix[0][8]);
	} else if (noteLength[channelView] == 3 || noteLength[channelView] == 2){
		trellis.setLED(hardwareMatrix[0][9]);
	}
	if (noteTriplet[channelView] == HIGH){
		trellis.setLED(hardwareMatrix[0][10]);
	}
	if (isInternal == HIGH){
		trellis.setLED(hardwareMatrix[0][11]);
	}
	trellis.setLED(hardwareMatrix[0][12]);
	trellis.setLED(hardwareMatrix[0][13]);
	trellis.setLED(hardwareMatrix[0][14]);
	if (longNotes[channelView] == HIGH){
		trellis.setLED(hardwareMatrix[0][15]);
	}
	for (int x=0; x<256; x++){
		if (pageView[channelView] == x){
			trellis.setLED(hardwareMatrix[1][x%16]);
			trellis.setLED(hardwareMatrix[3][x/16]);
		}
	}
	for (int x=0; x<256; x++){
		if (pageActive[channelView][x]==HIGH){
			if (pageView[channelView]/16 == x/16){
				trellis.setLED(hardwareMatrix[2][x%16]);
			}
			trellis.setLED(hardwareMatrix[4][x/16]);
		}
	}
	for (byte x=0; x<channelCount; x++){
		if (channelView == x){
			trellis.setLED(hardwareMatrix[5][x]);
		}
		if (channelActive[x] == HIGH){
			trellis.setLED(hardwareMatrix[6][x]);
		}
	}
	trellis.setLED(hardwareMatrix[7][0]);
	trellis.setLED(hardwareMatrix[7][1]);
	trellis.setLED(hardwareMatrix[7][2]);
	trellis.setLED(hardwareMatrix[7][3]);
	trellis.setLED(hardwareMatrix[7][4]);
	trellis.setLED(hardwareMatrix[7][5]);
	trellis.setLED(hardwareMatrix[7][6]);
	if (mooveIn == HIGH){
		trellis.setLED(hardwareMatrix[7][7]);
	}
	trellis.setLED(hardwareMatrix[7][8]);
	trellis.writeDisplay();
}
void noteOffVoid(byte valueNoteOff,byte channelNoteOff){
	if (millis() - usbTime > usbDebounce){
		usbMIDI.sendNoteOff(valueNoteOff, 0, channelNoteOff+1);
		usbTime = millis();
	} else {
		usbTransfer[0] = 1;
		usbTransfer[1] = 0;
		usbTransfer[2] = channelNoteOff+1;
		usbTransfer[3] = valueNoteOff;
		usbTransfer[4] = 0;
	}
	switch (channelNoteOff) {
		case 0:
			MIDI01.write(noteSendOn);
			MIDI01.write(valueNoteOff);
			MIDI01.write(velocityZero);
			
			MIDI01.write(noteSendOff);
			MIDI01.write(valueNoteOff);
			MIDI01.write(velocityZero);
			break;
		case 1:
			MIDI02.write(noteSendOn);
			MIDI02.write(valueNoteOff);
			MIDI02.write(velocityZero);
			
			MIDI02.write(noteSendOff);
			MIDI02.write(valueNoteOff);
			MIDI02.write(velocityZero);
			break;
		case 2:
			MIDI03.write(noteSendOn);
			MIDI03.write(valueNoteOff);
			MIDI03.write(velocityZero);
			
			MIDI03.write(noteSendOff);
			MIDI03.write(valueNoteOff);
			MIDI03.write(velocityZero);
			break;
		case 3:
			MIDI04.write(noteSendOn);
			MIDI04.write(valueNoteOff);
			MIDI04.write(velocityZero);
			
			MIDI04.write(noteSendOff);
			MIDI04.write(valueNoteOff);
			MIDI04.write(velocityZero);
			break;
		case 4:
			MIDI05.write(noteSendOn);
			MIDI05.write(valueNoteOff);
			MIDI05.write(velocityZero);
			
			MIDI05.write(noteSendOff);
			MIDI05.write(valueNoteOff);
			MIDI05.write(velocityZero);
			break;
		case 5:
			MIDI06.write(noteSendOn);
			MIDI06.write(valueNoteOff);
			MIDI06.write(velocityZero);
			
			MIDI06.write(noteSendOff);
			MIDI06.write(valueNoteOff);
			MIDI06.write(velocityZero);
			break;
		case 6:
			MIDI07.write(noteSendOn);
			MIDI07.write(valueNoteOff);
			MIDI07.write(velocityZero);
			
			MIDI07.write(noteSendOff);
			MIDI07.write(valueNoteOff);
			MIDI07.write(velocityZero);
			break;
		case 7:
			MIDI08.write(noteSendOn);
			MIDI08.write(valueNoteOff);
			MIDI08.write(velocityZero);
			
			MIDI08.write(noteSendOff);
			MIDI08.write(valueNoteOff);
			MIDI08.write(velocityZero);
			break;
		case 8:
			MIDI09.write(noteSendOn);
			MIDI09.write(valueNoteOff);
			MIDI09.write(velocityZero);
			
			MIDI09.write(noteSendOff);
			MIDI09.write(valueNoteOff);
			MIDI09.write(velocityZero);
			break;
		case 9:
			MIDI10.write(noteSendOn);
			MIDI10.write(valueNoteOff);
			MIDI10.write(velocityZero);
			
			MIDI10.write(noteSendOff);
			MIDI10.write(valueNoteOff);
			MIDI10.write(velocityZero);
			break;
		case 10:
			MIDI11.write(noteSendOn);
			MIDI11.write(valueNoteOff);
			MIDI11.write(velocityZero);
			
			MIDI11.write(noteSendOff);
			MIDI11.write(valueNoteOff);
			MIDI11.write(velocityZero);
			break;
		case 11:
			MIDI12.write(noteSendOn);
			MIDI12.write(valueNoteOff);
			MIDI12.write(velocityZero);

			MIDI12.write(noteSendOff);
			MIDI12.write(valueNoteOff);
			MIDI12.write(velocityZero);
			break;
		case 12:
			MIDI13.write(noteSendOn);
			MIDI13.write(valueNoteOff);
			MIDI13.write(velocityZero);
			
			MIDI13.write(noteSendOff);
			MIDI13.write(valueNoteOff);
			MIDI13.write(velocityZero);
			break;
		case 13:
			MIDI14.write(noteSendOn);
			MIDI14.write(valueNoteOff);
			MIDI14.write(velocityZero);
			
			MIDI14.write(noteSendOff);
			MIDI14.write(valueNoteOff);
			MIDI14.write(velocityZero);
			break;
		case 14:
			MIDI15.write(noteSendOn);
			MIDI15.write(valueNoteOff);
			MIDI15.write(velocityZero);
			
			MIDI15.write(noteSendOff);
			MIDI15.write(valueNoteOff);
			MIDI15.write(velocityZero);
			break;
		case 15:
			MIDI16.write(noteSendOn);
			MIDI16.write(valueNoteOff);
			MIDI16.write(velocityZero);
			
			MIDI16.write(noteSendOff);
			MIDI16.write(valueNoteOff);
			MIDI16.write(velocityZero);
			break;
	}
	noteStopFunction[channelNoteOff][valueNoteOff] = LOW;
	noteStop[channelNoteOff][valueNoteOff] = LOW;
}
void noteOnVoid(byte valueNoteOn, byte volumeNoteOn, byte channelNoteOn){
	//delay (latency[channelNoteOn]);
	if (millis() - usbTime > usbDebounce){
		usbMIDI.sendNoteOn(valueNoteOn, volumeNoteOn, channelNoteOn+1);
		usbTime = millis();
	} else {
		usbTransfer[0] = 1;
		usbTransfer[1] = 1;
		usbTransfer[2] = channelNoteOn+1;
		usbTransfer[3] = valueNoteOn;
		usbTransfer[4] = volumeNoteOn;
	}
	switch (channelNoteOn) {
		case 0:
			MIDI01.write(noteSendOn);
			MIDI01.write(valueNoteOn);
			MIDI01.write(volumeNoteOn);
			break;
		case 1:
			MIDI02.write(noteSendOn);
			MIDI02.write(valueNoteOn);
			MIDI02.write(volumeNoteOn);
			break;
		case 2:
			MIDI03.write(noteSendOn);
			MIDI03.write(valueNoteOn);
			MIDI03.write(volumeNoteOn);
			break;
		case 3:
			MIDI04.write(noteSendOn);
			MIDI04.write(valueNoteOn);
			MIDI04.write(volumeNoteOn);
			break;
		case 4:
			MIDI05.write(noteSendOn);
			MIDI05.write(valueNoteOn);
			MIDI05.write(volumeNoteOn);
			break;
		case 5:
			MIDI06.write(noteSendOn);
			MIDI06.write(valueNoteOn);
			MIDI06.write(volumeNoteOn);
			break;
		case 6:
			MIDI07.write(noteSendOn);
			MIDI07.write(valueNoteOn);
			MIDI07.write(volumeNoteOn);
			break;
		case 7:
			MIDI08.write(noteSendOn);
			MIDI08.write(valueNoteOn);
			MIDI08.write(volumeNoteOn);
			break;
		case 8:
			MIDI09.write(noteSendOn);
			MIDI09.write(valueNoteOn);
			MIDI09.write(volumeNoteOn);
			break;
		case 9:
			MIDI10.write(noteSendOn);
			MIDI10.write(valueNoteOn);
			MIDI10.write(volumeNoteOn);
			break;
		case 10:
			MIDI11.write(noteSendOn);
			MIDI11.write(valueNoteOn);
			MIDI11.write(volumeNoteOn);
			break;
		case 11:
			MIDI12.write(noteSendOn);
			MIDI12.write(valueNoteOn);
			MIDI12.write(volumeNoteOn);
			break;
		case 12:
			MIDI13.write(noteSendOn);
			MIDI13.write(valueNoteOn);
			MIDI13.write(volumeNoteOn);
			break;
		case 13:
			MIDI14.write(noteSendOn);
			MIDI14.write(valueNoteOn);
			MIDI14.write(volumeNoteOn);
			break;
		case 14:
			MIDI15.write(noteSendOn);
			MIDI15.write(valueNoteOn);
			MIDI15.write(volumeNoteOn);
			break;
		case 15:
			MIDI16.write(noteSendOn);
			MIDI16.write(valueNoteOn);
			MIDI16.write(volumeNoteOn);
			break;
	}
	noteStop[channelNoteOn][valueNoteOn] = HIGH;
}
void stopPlay(){
	if (function == LOW){
		if (pagePlay[channelView] == pageView[channelView]){
			for (byte i=0; i<128; i++){	 //Nimm den Strich weg
				if (softwareMatrix[channelView][pagePlay[channelView]][i] == LOW){
					trellis.clrLED(i);
				} else {
					trellis.setLED(i);
				}
			}
			trellis.writeDisplay();
		}
	}
	for (byte x=0; x<16; x++){
		steps[x] = tripletStep[noteTriplet[x]];
		for (byte y=0; y<128; y++){
			if (noteStop[x][y] == HIGH){
				noteOffVoid(y,x);
			}
		}
		for (byte y=0; y<8; y++){
			playLater[x][y] = LOW;
			playLaterOff[x][y] = LOW;
			for (int i=0; i<256; i++){
				startNote[x][i][y] = HIGH;
			}
		}
	}
	clockCounter = 0;
	isClock = LOW;
	clockStart = HIGH;
	beatClockCounter = 0;
	for (byte x=0; x<channelCount; x++){
		pagePlay[x] = 255;
		mooveInCounter[x] = 0;
	}
}

void setPageView1(byte x){
	for (byte c=0; c<channelCount; c++){
		if (pageActive[c][pageView[c]/16*16+x]==HIGH || channelView==c || pageActive[channelView][pageView[c]/16*16+x]==LOW) {
			pageView[c] = pageView[c]/16*16+x;
		}
	}
	functionView();
}

void setPageView2(byte x){
	for (byte c=0; c<channelCount; c++){
		if (pageActive[c][pageView[c]%16+x]==HIGH || channelView==c || pageActive[channelView][pageView[c]%16+x]==LOW) {
			pageView[c] = pageView[c]%16+x;
		}
	}
	functionView();
}

void setPageActive1(byte x){
	pageActive[channelView][pageView[channelView]/16*16+x] = !pageActive[channelView][pageView[channelView]/16*16+x];
	functionView();
}

void setPageActive2(byte x){
	if (pageActive[channelView][x]==LOW && 
		pageActive[channelView][x+1]==LOW && 
		pageActive[channelView][x+2]==LOW && 
		pageActive[channelView][x+3]==LOW && 
		pageActive[channelView][x+4]==LOW && 
		pageActive[channelView][x+5]==LOW && 
		pageActive[channelView][x+6]==LOW && 
		pageActive[channelView][x+7]==LOW && 
		pageActive[channelView][x+8]==LOW && 
		pageActive[channelView][x+9]==LOW && 
		pageActive[channelView][x+10]==LOW && 
		pageActive[channelView][x+11]==LOW && 
		pageActive[channelView][x+12]==LOW && 
		pageActive[channelView][x+13]==LOW && 
		pageActive[channelView][x+14]==LOW && 
		pageActive[channelView][x+15]==LOW && 
		pageActive[channelView][x+16]==LOW){
		pageActive[channelView][x] = !pageActive[channelView][x];
	} else {
		for (byte c=x; c<=x+16; c++){
			pageActive[channelView][c] = LOW;
		}
	}
	functionView();
}

void playLiveView(){
	for (byte c=0; c<128; c++){
		trellis.clrLED(funtionMatrix1[c]);
	}
	for (byte c=0; c<8; c++){
		if(liveLowerNote[channelView] > 0){
			trellis.setLED(hardwareMatrix[c][0]);
			trellis.setLED(hardwareMatrix[c][1]);
		}
		if(liveLowerNote[channelView] < 84){
			trellis.setLED(hardwareMatrix[c][14]);
			trellis.setLED(hardwareMatrix[c][15]);
		}
		if(c % 2 != 0){ //White Keys
			trellis.setLED(hardwareMatrix[c][4]);
			trellis.setLED(hardwareMatrix[c][5]);
			trellis.setLED(hardwareMatrix[c][6]);
			trellis.setLED(hardwareMatrix[c][7]);
			trellis.setLED(hardwareMatrix[c][8]);
			if (liveLowerNote[channelView] == 84 && c > 5){
				trellis.clrLED(hardwareMatrix[c][9]);
				trellis.clrLED(hardwareMatrix[c][10]);
				trellis.clrLED(hardwareMatrix[c][11]);
			} else {
				trellis.setLED(hardwareMatrix[c][9]);
				trellis.setLED(hardwareMatrix[c][10]);
				trellis.setLED(hardwareMatrix[c][11]);
			}
		} else { //Black Keys
			trellis.setLED(hardwareMatrix[c][5]);
			trellis.setLED(hardwareMatrix[c][6]);
			trellis.setLED(hardwareMatrix[c][8]);
			if (liveLowerNote[channelView] == 84 && c > 5){
				trellis.clrLED(hardwareMatrix[c][9]);
				trellis.clrLED(hardwareMatrix[c][10]);
			} else {
				trellis.setLED(hardwareMatrix[c][9]);
				trellis.setLED(hardwareMatrix[c][10]);
			}
		}
	}
	trellis.writeDisplay();
}
void liveRecord(byte liveRecordNote, byte liveRecordVolume){
	for (eightRows=0; eightRows<8; eightRows++){
		if (notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] == liveRecordNote && softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == HIGH){
			freeLiveNotes = LOW;
		}
	}
	if (freeLiveNotes == HIGH){
		for (eightRows=0; eightRows<8; eightRows++){
			if (softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] == LOW){
				softwareMatrix[channelView][pagePlay[channelView]][hardwareMatrix[eightRows][steps[channelView]]] = HIGH;
				notePitch[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = liveRecordNote;
				noteVolume[channelView][pagePlay[channelView]][eightRows][steps[channelView]] = liveRecordVolume;
				break;
			}
		}
	}
	freeLiveNotes = HIGH;
}
void setChannelActive(byte channel){
	channelActive[channel] = !channelActive[channel];
	if (isPlay == HIGH){
		if (channelActive[channel] == LOW){
			for (byte y=0; y<128; y++){
				if (noteStop[channel][y] == HIGH){
					noteOffVoid(y,channel);
				}
			}
		} else if (mooveIn == HIGH) {
			mooveInChannel[channel] = HIGH;
		}
	}
	functionView();
}

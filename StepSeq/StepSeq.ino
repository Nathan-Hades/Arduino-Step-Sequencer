#include <FlexIO_t4.h>
#include <FlexSerial.h>
#include <Wire.h>
#include <MIDI.h>
#include <Bounce.h>
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
const byte START = 250;
const byte CONTINUE = 251;
const byte STOP = 252;
const byte CLOCK = 248;
const byte channel = 1;
const byte noteSendOn = 144;
const byte noteSendOff = 128;
const byte velocityZero = 0;
bool function = LOW;
byte pattern[16][16][8];
bool setPattern1 = LOW;
bool setPattern2 = LOW;
byte patternEdit1;
byte patternEdit2;
bool noteMute[16][8];
bool autoMute[16][8];
byte muteEdit;
byte setMute;
bool noteSolo[16][8];
byte soloEdit;
byte setSolo;
byte noteStep = 4;
bool noteTriplet[16];
byte warten = 200;
byte zaehlerClock = 0;
byte steps[16];
const byte pinPlay = 23;
Bounce bouncePlay = Bounce(pinPlay, 100);
const byte pinFunction = 12;
Bounce bounceFunction = Bounce(pinFunction, 100);
byte achtNoten;
byte ansichtPlay[16];
byte ansichtView[16];
bool ansichtActive[16][16];
byte ansichtNext;
byte ansichtPre;
byte ansichtFirst;
byte ansichtLast;
byte ansichtWechsel[16][16] = {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0},
							   {2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1},
							   {3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2},
							   {4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3},
							   {5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4},
							   {6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5},
							   {7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6},
							   {8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7},
							   {9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8},
							   {10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9},
							   {11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10},
							   {12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11},
							   {13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12},
							   {14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13},
							   {15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14},
							   {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
byte channelView = 0;
bool channelActive[16];
byte channelPlay1;
byte channelPlay2;
byte channelPlay;
bool longNotes[16];
short channelCount = 5;
short notenWert[16][16][8][16];
byte noteEdit;
bool notePitch81 = LOW;
bool notePitch82 = LOW;
bool notePitchAll1 = LOW;
bool notePitchAll2 = LOW;
bool noteVolume1 = LOW;
bool noteVolume2 = LOW;
short volumeWert[16][16][8][16];
byte volumeEdit;
bool volumeValue81 = LOW;
bool volumeValue82 = LOW;
bool volumeValueAll1 = LOW;
bool volumeValueAll2 = LOW;
bool volumeVolume1 = LOW;
bool volumeVolume2 = LOW;
short startNoten[8] = {49,42,45,43,41,39,38,36};
bool softwareMatrix[16][16][128];
bool note[16][128];
bool noteStop[16][128];
bool noteStopFunction[16][128];
unsigned long trellisTime = 0;				 // letzter Zeitwert bei dem der Ausgangzustand wechselte.
unsigned long trellisDebounce = 30;	 // Entprellzeit
unsigned long clockTime = 0;
unsigned long clockDebounce = 200;
unsigned long usbTime = 0;
unsigned long usbDebounce = 1;
byte usbTransfer[5];
int BPM = 120;
bool setBPM = LOW;
int latenz[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int latenzPlay[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte latenzChannel[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool setLatenz = LOW;
bool setLatenzChannel = LOW;
bool playLater[16][8];
byte playLaterNotenWert[16][8];
byte playLaterVolumeWert[16][8];
unsigned long playLaterMillis[16][8];
bool playLaterOff[16][8];
byte playLaterOffNote[16][8];
unsigned long playLaterOffMillis[16][8];
byte noteValue[16];
unsigned long makeClock1 = 0;
unsigned long clockCurrent;
volatile float clockIntervall;
byte clock2[16];
bool clockStart = LOW;
bool isClock = LOW;
bool isPlay = LOW;
bool isInternal = HIGH;
byte funtionMatrix1[128] = { 0, 1, 2, 3,16,17,18,19, 32, 33, 34, 35,112,113,114,115,
							 4, 5, 6 ,7,20,21,22,23, 36, 37, 38, 39,116,117,118,119,
							 8, 9,10,11,24,25,26,27, 40, 41, 42, 43,120,121,122,123,
							12,13,14,15,28,29,30,31, 44, 45, 46, 47,124,125,126,127,
							64,65,66,67,80,81,82,83, 96, 97, 98, 99, 48, 49, 50, 51,
							68,69,70,71,84,85,86,87,100,101,102,103, 52, 53, 54, 55,
							72,73,74,75,88,89,90,91,104,105,106,107, 56, 57, 58, 59,
							76,77,78,79,92,93,94,95,108,109,110,111, 60, 61, 62, 63};
byte funtionMatrix2[128];
byte hardwareMatrix[8][16] = {{ 0, 1, 2, 3,16,17,18,19, 32, 33, 34, 35,112,113,114,115},
							  { 4, 5, 6 ,7,20,21,22,23, 36, 37, 38, 39,116,117,118,119},
							  { 8, 9,10,11,24,25,26,27, 40, 41, 42, 43,120,121,122,123},
							  {12,13,14,15,28,29,30,31, 44, 45, 46, 47,124,125,126,127},
							  {64,65,66,67,80,81,82,83, 96, 97, 98, 99, 48, 49, 50, 51},
							  {68,69,70,71,84,85,86,87,100,101,102,103, 52, 53, 54, 55},
							  {72,73,74,75,88,89,90,91,104,105,106,107, 56, 57, 58, 59},
							  {76,77,78,79,92,93,94,95,108,109,110,111, 60, 61, 62, 63}};
bool copyAnsicht1 = LOW;
bool copyAnsichtAnsicht[16];
bool copyAnsichtChannel[16];
byte liveLowerNote[16];
bool playLive = LOW;
bool playLiveTest = LOW;
byte liveNotes[128] = { 50,50,50, 1,50, 3,50,50, 6,50, 8,50,10,50,50,50,
						50,50, 0,50, 2,50, 4, 5,50, 7,50, 9,50,11,50,50,
						50,50,50,13,50,15,50,50,18,50,20,50,22,50,50,50,
						50,50,12,50,14,50,16,17,50,19,50,21,50,23,50,50,
						50,50,50,25,50,27,50,50,30,50,32,50,34,50,50,50,
						50,50,24,50,26,50,28,29,50,31,50,33,50,35,50,50,
						50,50,50,37,50,39,50,50,42,50,44,50,46,50,50,50,
						50,50,36,50,38,50,40,41,50,43,50,45,50,47,50,50};
bool freeLiveNotes = HIGH;
byte liveIsPressed[128];
bool liveIsUnPressed[128];
byte MIDIByte;
byte statusByte;
byte dataByte1;
byte dataByte2;
FlexSerial MIDI1(-1, 9);
FlexSerial MIDI2(-1, 11);
FlexSerial MIDI3(-1, 6);
FlexSerial MIDI4(-1, 8);
FlexSerial MIDI5(-1, 10);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIInput);
//int led = 13;
//bool ledcheck;


void setup(){
	CCM_CS1CDR &= ~( CCM_CS1CDR_FLEXIO2_CLK_PRED( 7 ) );
	CCM_CS1CDR |= CCM_CS1CDR_FLEXIO2_CLK_PRED( 0b11 );
	pinMode(pinFunction, INPUT_PULLUP);
	pinMode(pinPlay, INPUT_PULLUP);
	MIDIInput.begin(MIDI_CHANNEL_OMNI);
	//pinMode(led, OUTPUT);
	MIDI1.begin(31250);
	MIDI2.begin(31250);
	MIDI3.begin(31250);
	MIDI4.begin(31250);
	MIDI5.begin(31250);
	for (byte c=0; c<128; c++){
		funtionMatrix2[funtionMatrix1[c]] = c;
	}
	
	for (byte c=0; c<128; c++){
		for (byte x=0; x<16; x++){
			noteStop[x][c] = LOW;
			noteStopFunction[x][c] = LOW;
			note[x][c] = LOW;
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
		noteValue[z] = 12;
		steps[z] = -1;
		clock2[z] = 40;
		noteTriplet[z] = LOW;
		copyAnsichtAnsicht[z] = LOW;
		copyAnsichtChannel[z] = LOW;
		liveLowerNote[z] = 36;
		for (byte c=0; c<8; c++){
			noteMute[z][c] = LOW;
			noteSolo[z][c] = LOW;
			for (byte x=0; x<16; x++){
				for (byte y=0; y<channelCount; y++){
					notenWert[y][x][c][z] = startNoten[c];
					volumeWert[y][x][c][z] = 100;
					pattern[y][x][c] = 0;
					if (x == 0){
						ansichtActive[y][x] = HIGH;
					} else {
						ansichtActive[y][x] = LOW;
					}
					ansichtPlay[x] = 0;
					ansichtView[x] = 0;
				}
			}
		}
	}
	/*MIDI1.write(noteSendOn);
	MIDI1.write(50);
	MIDI1.write(127);
	delay(500);
	noteOffVoid(50,0);
	delay(500);
	MIDI2.write(noteSendOn);
	MIDI2.write(50);
	MIDI2.write(127);
	delay(500);
	noteOffVoid(50,1);
	delay(500);
	MIDI3.write(noteSendOn);
	MIDI3.write(50);
	MIDI3.write(127);
	delay(500);
	noteOffVoid(50,2);
	delay(500);
	MIDI4.write(noteSendOn);
	MIDI4.write(50);
	MIDI4.write(127);
	delay(500);
	noteOffVoid(50,3);
	delay(500);*/
	trellis.begin(0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77);
	startLED(hardwareMatrix[3][7],hardwareMatrix[4][7],hardwareMatrix[3][8],hardwareMatrix[4][8]);
	trellis.writeDisplay();
	delay(warten);
	startLED(hardwareMatrix[3][6],hardwareMatrix[4][6],hardwareMatrix[3][9],hardwareMatrix[4][9]);
	trellis.writeDisplay();
	delay(warten);

	for (byte c=0;c<6;c++){
		startLED(hardwareMatrix[2-(c/2)][5-c],hardwareMatrix[5+(c/2)][5-c],hardwareMatrix[2-(c/2)][10+c],hardwareMatrix[5+(c/2)][10+c]);
		stopLED(hardwareMatrix[3-(c/2)][7-c],hardwareMatrix[4+(c/2)][7-c],hardwareMatrix[3-(c/2)][8+c],hardwareMatrix[4+(c/2)][8+c]);
		trellis.writeDisplay();
		delay(warten);
	}
		
	stopLED(hardwareMatrix[0][1],hardwareMatrix[7][1],hardwareMatrix[0][14],hardwareMatrix[7][14]);
	trellis.writeDisplay();
	delay(warten);
	stopLED(hardwareMatrix[0][0],hardwareMatrix[7][0],hardwareMatrix[0][15],hardwareMatrix[7][15]);
	trellis.writeDisplay();
	usbMIDI.setHandleRealTimeSystem(usbBeatClock);
	trellisTime = millis();
}

void loop(){
	//if (isPlay == LOW) { //Für Timing Probleme bitte zweiten Arduino die MIDI Beat Clock machen lassen.
	if (millis() - trellisTime > trellisDebounce){
		if (trellis.readSwitches()){
			for (byte z=0; z<numKeys; z++){
				if (trellis.justPressed(z)){
					if (function == LOW){ //Setze Step
						if (softwareMatrix[channelView][ansichtView[channelView]][z]==LOW){
							softwareMatrix[channelView][ansichtView[channelView]][z] = HIGH;
							trellis.setLED(z);
							trellis.writeDisplay();
						} else {
							softwareMatrix[channelView][ansichtView[channelView]][z] = LOW;
							trellis.clrLED(z);
							trellis.writeDisplay();
						}
					} else if (notePitch81 == HIGH && (z == 0 || z == 4 || z == 8 || z == 12 || z == 64 || z == 68 || z == 72 || z == 76)){ //to which note should the value be changed
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
						for (byte c=0; c<=notenWert[channelView][ansichtView[channelView]][noteEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						trellis.writeDisplay();
						notePitch81 = LOW;
						notePitch82 = HIGH;
					} else if (notePitch82 == HIGH){ //change the notePitch
						noteOffVoid(notenWert[channelView][ansichtView[channelView]][noteEdit][0],channelView);
						for (byte c=ansichtView[channelView]; c<16; c++){
							if (ansichtActive[channelView][c] == HIGH){
								for (byte x=0; x<16; x++){
										notenWert[channelView][c][noteEdit][x] = funtionMatrix2[z];
								}
							}
						}
						noteOnVoid(notenWert[channelView][ansichtView[channelView]][noteEdit][0],volumeWert[channelView][ansichtView[channelView]][noteEdit][0],channelView);
						noteStopFunction[channelView][notenWert[channelView][ansichtView[channelView]][noteEdit][0]] = HIGH;
						for (byte c=0; c<128; c++){
									trellis.clrLED(c);
						}
						for (byte c=0; c<=notenWert[channelView][ansichtView[channelView]][noteEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						
						trellis.writeDisplay();

					} else if (notePitchAll1 == HIGH){ //to which note should the value be changed
						for (byte c=0; c<128; c++){
								trellis.clrLED(c);
						}
						noteEdit = funtionMatrix2[z];
						for (byte c=0; c<=notenWert[channelView][ansichtView[channelView]][noteEdit/16][noteEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						trellis.writeDisplay();
						notePitchAll1 = LOW;
						notePitchAll2 = HIGH;
					} else if (notePitchAll2 == HIGH){ //change the notePitch
						noteOffVoid(notenWert[channelView][ansichtView[channelView]][noteEdit/16][noteEdit%16],channelView);
						notenWert[channelView][ansichtView[channelView]][noteEdit/16][noteEdit%16] = funtionMatrix2[z];
						noteOnVoid(notenWert[channelView][ansichtView[channelView]][noteEdit/16][noteEdit%16],volumeWert[channelView][ansichtView[channelView]][noteEdit/16][noteEdit%16],channelView);
						noteStopFunction[channelView][notenWert[channelView][ansichtView[channelView]][noteEdit/16][noteEdit%16]] = HIGH;
						for (byte c=0; c<128; c++){
									trellis.clrLED(c);
						}
						for (byte c=0; c<=notenWert[channelView][ansichtView[channelView]][noteEdit/16][noteEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						
						trellis.writeDisplay();
					} else if (volumeValue81 == HIGH && (z == 0 || z == 4 || z == 8 || z == 12 || z == 64 || z == 68 || z == 72 || z == 76)){ //to which note should the volume be changed
						for (byte c=0; c<128; c++){
								trellis.clrLED(c);
						}
						switch (z) {
							case 0:
								volumeEdit = 0;
								break;
							case 4:
								volumeEdit = 1;
								break;
							case 8:
								volumeEdit = 2;
								break;
							case 12:
								volumeEdit = 3;
								break;
							case 64:
								volumeEdit = 4;
								break;
							case 68:
								volumeEdit = 5;
								break;
							case 72:
								volumeEdit = 6;
								break;
							case 76:
								volumeEdit = 7;
								break;
						}
						for (byte c=0; c<=volumeWert[channelView][ansichtView[channelView]][volumeEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						trellis.writeDisplay();
						volumeValue81 = LOW;
						volumeValue82 = HIGH;
					} else if (volumeValue82 == HIGH){ //change the volumevalue
						noteOffVoid(notenWert[channelView][ansichtView[channelView]][volumeEdit][0],channelView);
						for (byte c=ansichtView[channelView]; c<16; c++){
							if (ansichtActive[channelView][c] == HIGH){
								for (byte x=0; x<16; x++){
										volumeWert[channelView][c][volumeEdit][x] = funtionMatrix2[z];
								}
							}
						}
						noteOnVoid(notenWert[channelView][ansichtView[channelView]][volumeEdit][0],volumeWert[channelView][ansichtView[channelView]][volumeEdit][0],channelView);
						noteStopFunction[channelView][notenWert[channelView][ansichtView[channelView]][volumeEdit][0]] = HIGH;
						for (byte c=0; c<128; c++){
									trellis.clrLED(c);
						}
						for (byte c=0; c<=volumeWert[channelView][ansichtView[channelView]][volumeEdit][0]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						
						trellis.writeDisplay();
					} else if (volumeValueAll1 == HIGH){ //to which volume should the value be changed
						for (byte c=0; c<128; c++){
								trellis.clrLED(c);
						}
						volumeEdit = funtionMatrix2[z];
						for (byte c=0; c<=volumeWert[channelView][ansichtView[channelView]][volumeEdit/16][volumeEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						trellis.writeDisplay();
						volumeValueAll1 = LOW;
						volumeValueAll2 = HIGH;
					} else if (volumeValueAll2 == HIGH){ //change the volumevalue
						noteOffVoid(notenWert[channelView][ansichtView[channelView]][volumeEdit/16][volumeEdit%16],channelView);
						volumeWert[channelView][ansichtView[channelView]][volumeEdit/16][volumeEdit%16] = funtionMatrix2[z];
						noteOnVoid(notenWert[channelView][ansichtView[channelView]][volumeEdit/16][volumeEdit%16],volumeWert[channelView][ansichtView[channelView]][volumeEdit/16][volumeEdit%16],channelView);
						noteStopFunction[channelView][notenWert[channelView][ansichtView[channelView]][volumeEdit/16][volumeEdit%16]] = HIGH;
						for (byte c=0; c<128; c++){
									trellis.clrLED(c);
						}
						for (byte c=0; c<=volumeWert[channelView][ansichtView[channelView]][volumeEdit/16][volumeEdit%16]; c++){
								trellis.setLED(funtionMatrix1[c]);
						}
						trellis.writeDisplay();
					} else if (setPattern1 == HIGH){ //to which 1-8 row should set the pattern
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
							for (byte c=ansichtView[channelView]; c<16; c++){
								if (ansichtActive[channelView][c] == HIGH){
									for (byte y=0; y<16; y++){
										if (y % patternEdit2 == 0){
											softwareMatrix[channelView][c][hardwareMatrix[patternEdit1][y]] = HIGH;
										} else {
											softwareMatrix[channelView][c][hardwareMatrix[patternEdit1][y]] = LOW;
										}
									}
								}
							}
							trellis.writeDisplay();
						}
					} else if (setMute == HIGH){ //Set Mute
						if (z == 0 || z == 4 || z == 8 || z == 12 || z == 64 || z == 68 || z == 72 || z == 76 || z == 1 || z == 5 || z == 9 || z == 13 || z == 65 || z == 69 || z == 73 || z == 77){
							switch (z) {
								case 0:
									muteEdit = 0;
									break;
								case 1:
									muteEdit = 0;
									break;
								case 4:
									muteEdit = 1;
									break;
								case 5:
									muteEdit = 1;
									break;
								case 8:
									muteEdit = 2;
									break;
								case 9:
									muteEdit = 2;
									break;
								case 12:
									muteEdit = 3;
									break;
								case 13:
									muteEdit = 3;
									break;
								case 64:
									muteEdit = 4;
									break;
								case 65:
									muteEdit = 4;
									break;
								case 68:
									muteEdit = 5;
									break;
								case 69:
									muteEdit = 5;
									break;
								case 72:
									muteEdit = 6;
									break;
								case 73:
									muteEdit = 6;
									break;
								case 76:
									muteEdit = 7;
									break;
								case 77:
									muteEdit = 7;
									break;
							}
							noteMute[channelView][muteEdit] = !noteMute[channelView][muteEdit];
							if (autoMute[channelView][muteEdit] == HIGH){
								autoMute[channelView][muteEdit] = LOW;
							}
							if (noteSolo[channelView][muteEdit] == HIGH){
								noteSolo[channelView][muteEdit] = LOW;
								for (byte c=0; c<8; c++){
									if (noteSolo[channelView][0] == LOW && noteSolo[channelView][1] == LOW && noteSolo[channelView][2] == LOW && noteSolo[channelView][3] == LOW && noteSolo[channelView][4] == LOW && noteSolo[channelView][5] == LOW && noteSolo[channelView][6] == LOW && noteSolo[channelView][7] == LOW && autoMute[channelView][c] == HIGH){
										autoMute[channelView][c] = LOW;
										noteMute[channelView][c] = LOW;
									}
								}
							}
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							for (byte c=0; c<8; c++){
								if (noteMute[channelView][c] == HIGH){
									trellis.setLED(hardwareMatrix[c][0]);
								} else {
									trellis.setLED(hardwareMatrix[c][1]);
								}
							}
							trellis.writeDisplay();
						}
					} else if (setSolo == HIGH){ //Set Solo
						if (z == 0 || z == 4 || z == 8 || z == 12 || z == 64 || z == 68 || z == 72 || z == 76 || z == 1 || z == 5 || z == 9 || z == 13 || z == 65 || z == 69 || z == 73 || z == 77){
							switch (z) {
								case 0:
									soloEdit = 0;
									break;
								case 1:
									soloEdit = 0;
									break;
								case 4:
									soloEdit = 1;
									break;
								case 5:
									soloEdit = 1;
									break;
								case 8:
									soloEdit = 2;
									break;
								case 9:
									soloEdit = 2;
									break;
								case 12:
									soloEdit = 3;
									break;
								case 13:
									soloEdit = 3;
									break;
								case 64:
									soloEdit = 4;
									break;
								case 65:
									soloEdit = 4;
									break;
								case 68:
									soloEdit = 5;
									break;
								case 69:
									soloEdit = 5;
									break;
								case 72:
									soloEdit = 6;
									break;
								case 73:
									soloEdit = 6;
									break;
								case 76:
									soloEdit = 7;
									break;
								case 77:
									soloEdit = 7;
									break;
							}
							noteSolo[channelView][soloEdit] = !noteSolo[channelView][soloEdit];
							for (byte c=0; c<128; c++){
								trellis.clrLED(c);
							}
							if (noteMute[channelView][soloEdit] == HIGH && noteSolo[channelView][soloEdit] == HIGH){
								noteMute[channelView][soloEdit] = LOW;
							}
							for (byte c=0; c<8; c++){
								if (noteSolo[channelView][c] == HIGH){
									trellis.setLED(hardwareMatrix[c][0]);
								} else {
									if (noteSolo[channelView][c] == LOW && noteSolo[channelView][soloEdit] == HIGH && c != soloEdit){
										autoMute[channelView][c] = HIGH;
										noteMute[channelView][c] = HIGH;
									} else if (noteSolo[channelView][0] == LOW && noteSolo[channelView][1] == LOW && noteSolo[channelView][2] == LOW && noteSolo[channelView][3] == LOW && noteSolo[channelView][4] == LOW && noteSolo[channelView][5] == LOW && noteSolo[channelView][6] == LOW && noteSolo[channelView][7] == LOW && autoMute[channelView][c] == HIGH && noteSolo[channelView][soloEdit] == LOW && c != soloEdit){
										autoMute[channelView][c] = LOW;
										noteMute[channelView][c] = LOW;
									}
									trellis.setLED(hardwareMatrix[c][1]);
								}
							}
							trellis.writeDisplay();
						}
					} else if (setBPM == HIGH){
						BPM = funtionMatrix2[z] + 57;
						for (byte c=0; c<128; c++){
							if (c < BPM - 56){
								trellis.setLED(funtionMatrix1[c]);
							} else {
								trellis.clrLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
					} else if (setLatenz == HIGH){
						latenz[channelView] = funtionMatrix2[z];
						for (byte c=0; c<128; c++){
							if (c <= latenz[channelView]){
								trellis.setLED(funtionMatrix1[c]);
							} else {
								trellis.clrLED(funtionMatrix1[c]);
							}
						}
						trellis.writeDisplay();
					} else if (copyAnsicht1 == HIGH){
						if(funtionMatrix2[z] > 31 && funtionMatrix2[z] < 48){
							if(copyAnsichtAnsicht[funtionMatrix2[z]-32] == LOW){
								copyAnsichtAnsicht[funtionMatrix2[z]-32] = HIGH;
								trellis.setLED(z);
							} else {
								copyAnsichtAnsicht[funtionMatrix2[z]-32] = LOW;
								trellis.clrLED(z);
							}
							trellis.writeDisplay();
						}
						if(funtionMatrix2[z] > 63 && funtionMatrix2[z] < 80){
							if(copyAnsichtChannel[funtionMatrix2[z]-64] == LOW){
								copyAnsichtChannel[funtionMatrix2[z]-64] = HIGH;
								trellis.setLED(z);
							} else {
								copyAnsichtChannel[funtionMatrix2[z]-64] = LOW;
								trellis.clrLED(z);
							}
							trellis.writeDisplay();
						}
						if(z == 72){
							for (byte c=0; c<16; c++){
								if(copyAnsichtAnsicht[c] == HIGH){
									for (byte y=0; y<channelCount; y++){
										if(copyAnsichtChannel[y] == HIGH){
											for (byte x=0; x<128; x++){
												softwareMatrix[y][c][x] = softwareMatrix[channelView][ansichtView[channelView]][x];
											}
											for (byte x=0; x<8; x++){
												for (byte a=0; a<16; a++){
													notenWert[y][c][x][a] = notenWert[channelView][ansichtView[channelView]][x][a];
													volumeWert[y][c][x][a] = notenWert[channelView][ansichtView[channelView]][x][a];
												}
											}
										}
									}
								}
							}
							for (byte c=0; c<16; c++){
								copyAnsichtAnsicht[c] = LOW;
							}
							for (byte y=0; y<channelCount; y++){
								copyAnsichtChannel[y] = LOW;
							}
							copyAnsicht1 = LOW;
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
					} else {
						switch (z) {
							case 0: //Set notePitch for 8
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=0; c<8; c++){
										trellis.setLED(hardwareMatrix[c][0]);
									}
									trellis.writeDisplay();
									notePitch81 = HIGH;
								}
								break;
							case 1: //Set notePitch for All
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=0; c<128; c++){
										trellis.setLED(funtionMatrix1[c]);
									}
									trellis.writeDisplay();
									notePitchAll1 = HIGH;
								}
								break;
							case 2: //Set notePitch for 8
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=0; c<8; c++){
										trellis.setLED(hardwareMatrix[c][0]);
									}
									trellis.writeDisplay();
									volumeValue81 = HIGH;
								}
								break;
							case 3: //Set notePitch for All
								if (isPlay == LOW){
									for (byte c=0; c<128; c++){
										trellis.clrLED(c);
									}
									for (byte c=0; c<128; c++){
										trellis.setLED(funtionMatrix1[c]);
									}
									trellis.writeDisplay();
									volumeValueAll1 = HIGH;
								}
								break;
							case 16: //Set Pattern
								for (byte c=0; c<128; c++){
									trellis.clrLED(c);
								}
								for (byte c=0; c<8; c++){
									trellis.setLED(hardwareMatrix[c][0]);
								}
								trellis.writeDisplay();
								setPattern1 = HIGH;
								break;
							case 17: //Set Mute
								for (byte c=0; c<128; c++){
										trellis.clrLED(c);
								}
								for (byte c=0; c<8; c++){
									if (noteMute[channelView][c] == HIGH || autoMute[channelView][c] == HIGH){
										trellis.setLED(hardwareMatrix[c][0]);
									} else {
										trellis.setLED(hardwareMatrix[c][1]);
									}
								}
								trellis.writeDisplay();
								setMute = HIGH;
								break;
							case 18: //Set Solo
								for (byte c=0; c<128; c++){
										trellis.clrLED(c);
								}
								for (byte c=0; c<8; c++){
									if (noteSolo[channelView][c] == HIGH){
										trellis.setLED(hardwareMatrix[c][0]);
									} else {
										trellis.setLED(hardwareMatrix[c][1]);
									}
								}
								trellis.writeDisplay();
								setSolo = HIGH;
								break;
							case 19: //Set Note Value to 4
								if (noteTriplet[channelView] == LOW){
									noteValue[channelView] = 24;
								} else {
									noteValue[channelView] = 16;
								}
								functionView();
								break;
							case 32: //Set Note Value to 8
								if (noteTriplet[channelView] == LOW){
									noteValue[channelView] = 12;
								} else {
									noteValue[channelView] = 8;
								}
								functionView();
								break;
							case 33: //Set Note Value to 16
								if (noteTriplet[channelView] == LOW){
									noteValue[channelView] = 6;
								} else {
									noteValue[channelView] = 4;
								}
								functionView();
								break;
							case 34: //Set Note Value to 32
								if (noteTriplet[channelView] == LOW){
									noteValue[channelView] = 3;
								} else {
									noteValue[channelView] = 2;
								}
								functionView();
								break;
							case 35: //Invert Triplets
								if (noteTriplet[channelView] == LOW){
									noteValue[channelView] = noteValue[channelView]*2/3;
									noteTriplet[channelView] = HIGH;
								} else {
									noteValue[channelView] = noteValue[channelView]*3/2;
									noteTriplet[channelView] = LOW;
								}
								functionView();
								break;
							case 112: // Invert Clock Source
								isInternal = !isInternal;
								functionView();
								break;
							case 113: // Set BPM
								for (byte c=0; c<128; c++){
									if (c < BPM - 56){
										trellis.setLED(funtionMatrix1[c]);
									} else {
										trellis.clrLED(funtionMatrix1[c]);
									}
								}
								trellis.writeDisplay();
								setBPM = HIGH;
								break;
							case 114: // Set Latenz
								for (byte c=0; c<128; c++){
									if (c <= latenz[channelView]){
										trellis.setLED(funtionMatrix1[c]);
									} else {
										trellis.clrLED(funtionMatrix1[c]);
									}
								}
								trellis.writeDisplay();
								setLatenz = HIGH;
								break;
							case 4: //Set ansichtView = 0
								setAnsichtView(0);
								break;
							case 5: //Set ansichtView = 1
								setAnsichtView(1);
								break;
							case 6: //Set ansichtView = 2
								setAnsichtView(2);
								break;
							case 7: //Set ansichtView = 3
								setAnsichtView(3);
								break;
							case 20: //Set ansichtView = 4
								setAnsichtView(4);
								break;
							case 21: //Set ansichtView = 5
								setAnsichtView(5);
								break;
							case 22: //Set ansichtView = 6
								setAnsichtView(6);
								break;
							case 23: //Set ansichtView = 7
								setAnsichtView(7);
								break;
							case 36: //Set ansichtView = 8
								setAnsichtView(8);
								break;
							case 37: //Set ansichtView = 9
								setAnsichtView(9);
								break;
							case 38: //Set ansichtView = 10
								setAnsichtView(10);
								break;
							case 39: //Set ansichtView = 11
								setAnsichtView(11);
								break;
							case 116: //Set ansichtView = 12
								setAnsichtView(12);
								break;
							case 117: //Set ansichtView = 13
								setAnsichtView(13);
								break;
							case 118: //Set ansichtView = 14
								setAnsichtView(14);
								break;
							case 119: //Set ansichtView = 15
								setAnsichtView(15);
								break;
							case 8: //Invert ansichtActive 0
								ansichtActive[channelView][0] = !ansichtActive[channelView][0];
								functionView();
								break;
							case 9: //Invert ansichtActive 1
								ansichtActive[channelView][1] = !ansichtActive[channelView][1];
								functionView();
								break;
							case 10: //Invert ansichtActive 2
								ansichtActive[channelView][2] = !ansichtActive[channelView][2];
								functionView();
								break;
							case 11: //Invert ansichtActive 3
								ansichtActive[channelView][3] = !ansichtActive[channelView][3];
								functionView();
								break;
							case 24: //Invert ansichtActive 4 
								ansichtActive[channelView][4] = !ansichtActive[channelView][4];
								functionView();
								break;
							case 25: //Invert ansichtActive 5
								ansichtActive[channelView][5] = !ansichtActive[channelView][5];
								functionView();
								break;
							case 26: //Invert ansichtActive 6
								ansichtActive[channelView][6] = !ansichtActive[channelView][6];
								functionView();
								break;
							case 27: //Invert ansichtActive 7
								ansichtActive[channelView][7] = !ansichtActive[channelView][7];
								functionView();
								break;
							case 40: //Invert ansichtActive 8
								ansichtActive[channelView][8] = !ansichtActive[channelView][8];
								functionView();
								break;
							case 41: //Invert ansichtActive 9
								ansichtActive[channelView][9] = !ansichtActive[channelView][9];
								functionView();
								break;
							case 42: //Invert ansichtActive 10
								ansichtActive[channelView][10] = !ansichtActive[channelView][10];
								functionView();
								break;
							case 43: //Invert ansichtActive 11
								ansichtActive[channelView][11] = !ansichtActive[channelView][11];
								functionView();
								break;
							case 120: //Invert ansichtActive 12
								ansichtActive[channelView][12] = !ansichtActive[channelView][12];
								functionView();
								break;
							case 121: //Invert ansichtActive 13
								ansichtActive[channelView][13] = !ansichtActive[channelView][13];
								functionView();
								break;
							case 122: //Invert ansichtActive 14
								ansichtActive[channelView][14] = !ansichtActive[channelView][14];
								functionView();
								break;
							case 123: //Invert ansichtActive 15
								ansichtActive[channelView][15] = !ansichtActive[channelView][15];
								functionView();
								break;
							case 12: //Set channelView = 0
								channelView = 0;
								functionView();
								break;
							case 13: //Set channelView = 1
								channelView = 1;
								functionView();
								break;
							case 14: //Set channelView = 2
								channelView = 2;
								functionView();
								break;
							case 15: //Set channelView = 3
								channelView = 3;
								functionView();
								break;
							case 28: //Set channelView = 4
								channelView = 4;
								functionView();
								break;
							case 64: //Invert Channel Active 0
								channelActive[0] = !channelActive[0];
								if (channelActive[0] == LOW && isPlay == HIGH){
									for (byte y=0; y<128; y++){
										if (noteStop[0][y] == HIGH){
											noteOffVoid(y,0);
										}
									}
								}
								functionView();
								break;
							case 65: //Invert Channel Active 1
								channelActive[1] = !channelActive[1];
								if (channelActive[1] == LOW && isPlay == HIGH){
									for (byte y=0; y<128; y++){
										if (noteStop[1][y] == HIGH){
											noteOffVoid(y,1);
										}
									}
								}
								functionView();
								break;
							case 66: //Invert Channel Active 2
								channelActive[2] = !channelActive[2];
								if (channelActive[2] == LOW && isPlay == HIGH){
									for (byte y=0; y<128; y++){
										if (noteStop[2][y] == HIGH){
											noteOffVoid(y,2);
										}
									}
								}
								functionView();
								break;
							case 67: //Invert Channel Active 3
								channelActive[3] = !channelActive[3];
								if (channelActive[3] == LOW && isPlay == HIGH){
									for (byte y=0; y<128; y++){
										if (noteStop[3][y] == HIGH){
											noteOffVoid(y,3);
										}
									}
								}
								functionView();
								break;
							case 80: //Invert Channel Active 4
								channelActive[4] = !channelActive[4];
								if (channelActive[4] == LOW && isPlay == HIGH){
									for (byte y=0; y<128; y++){
										if (noteStop[4][y] == HIGH){
											noteOffVoid(y,4);
										}
									}
								}
								functionView();
								break;
							case 68: //Invert Channel Long Note 0
								longNotes[0] = !longNotes[0];
								functionView();
								break;
							case 69: //Invert Channel Long Note 1
								longNotes[1] = !longNotes[1];
								functionView();
								break;
							case 70: //Invert Channel Long Note 2
								longNotes[2] = !longNotes[2];
								functionView();
								break;
							case 71: //Invert Channel Long Note 3
								longNotes[3] = !longNotes[3];
								functionView();
								break;
							case 84: //Invert Channel Long Note 4
								longNotes[4] = !longNotes[4];
								functionView();
								break;
							case 72: //Copy
								for (byte x=0; x<16; x++){
									trellis.clrLED(hardwareMatrix[2][x]);
								}
								for (byte x=0; x<channelCount; x++){
									trellis.clrLED(hardwareMatrix[4][x]);
								}
								trellis.writeDisplay();
								copyAnsicht1 = HIGH;
								break;
							case 76: // Play Live
								playLiveView();
								playLiveTest = HIGH;
								playLive = HIGH;
								break;
							case 77: // Play Live
								playLiveView();
								playLive = HIGH;
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
							}
						}
					}
					if (trellis.justReleased(z)){
						if (liveNotes[funtionMatrix2[z]] != 50){
							noteOffVoid(liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView], channelView);
							if (isPlay == HIGH && playLiveTest == LOW){
								liveIsUnPressed[liveNotes[funtionMatrix2[z]] + liveLowerNote[channelView]] = HIGH;
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
					if (softwareMatrix[channelView][ansichtView[channelView]][i] == LOW){
						trellis.clrLED(i);
					} else {
						trellis.setLED(i);
					}
				}
				trellis.writeDisplay();
				function = LOW;
				notePitch81 = LOW;
				notePitch82 = LOW;
				notePitchAll1 = LOW;
				notePitchAll2 = LOW;
				volumeValue81 = LOW;
				volumeValue82 = LOW;
				volumeValueAll1 = LOW;
				volumeValueAll2 = LOW;
				setPattern1 = LOW;
				setPattern2 = LOW;
				setMute = LOW;
				setSolo = LOW;
				setBPM = LOW;
				setLatenz = LOW;
				setLatenzChannel = LOW;
				copyAnsicht1 = LOW;
				playLive = LOW;
				playLiveTest = LOW;
			}
		}
	}
	for (byte i=0; i<channelCount; i++){
		for (byte y=0; y<8; y++){
			if (playLater[i][y] == HIGH) {
				if (millis() - playLaterMillis[i][y] > latenz[i]){
					noteOnVoid(playLaterNotenWert[i][y], playLaterVolumeWert[i][y], i);
					playLater[i][y] = LOW;
				}
			}
			if (playLaterOff[i][y] == HIGH) {
				if (millis() - playLaterOffMillis[i][y] > latenz[i]){
					noteOffVoid(playLaterOffNote[i][y], i);
					playLaterOff[i][y] = LOW;
				}
			}
		}
	}
	if (isInternal == HIGH && isPlay == HIGH) {
		clockIntervall = 60.0/BPM*1000*1000/24.0;
		if (micros() > makeClock1){
			makeClock1 = makeClock1 + clockIntervall;
			beatClock(CLOCK);
		}
	}
	if (isInternal == LOW && isClock == HIGH) {
		if((millis() - clockTime > clockDebounce)){ //Wenn Pause ist
			stopPlay();
		}
	}
	if (bouncePlay.update() && isInternal == HIGH){
		if (bouncePlay.fallingEdge()){
			//digitalWrite(led, LOW);
			makeClock1 = micros();
			isPlay = !isPlay;
			if (isPlay == LOW){
				stopPlay();
				//digitalWrite(led, LOW);
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
				noteOnVoid(dataByte1, dataByte2, channelView);
				if (isPlay == HIGH && playLiveTest == LOW && playLive == HIGH){
					liveIsPressed[dataByte1] = dataByte2;
				}
				break;
			case midi::NoteOff:
				dataByte1 = MIDIInput.getData1();
				dataByte2 = MIDIInput.getData2();
				noteOffVoid(dataByte1, channelView);
				if (isPlay == HIGH && playLiveTest == LOW && playLive == HIGH){
					liveIsUnPressed[dataByte1] = HIGH;
				}
				break;
		}
	}
}

void usbBeatClock(byte realtimebyte){
	if (isInternal == LOW){
		beatClock(realtimebyte);
	}
}

void beatClock(byte realtimebyte){
	//if(realtimebyte == START || realtimebyte == CONTINUE){zaehlerClock = 0;}
	if(realtimebyte == CLOCK){
		isClock = HIGH;
		clockTime = millis();
		zaehlerClock++;
		channelPlay2 = 0;
		for(byte i = 0; i < 16; i++) latenzPlay[i] = latenz[i];
		if (zaehlerClock == 25){zaehlerClock = 1;}		 //Eine Viertelnote ist vorbei
		for (channelPlay1 = 0; channelPlay1<channelCount; channelPlay1++){
			if (latenzPlay[channelPlay1] == min(latenzPlay[0], min(latenzPlay[1], min(latenzPlay[2], min(latenzPlay[3], latenzPlay[4]))))) {
				beatClock2(channelPlay1);
				latenzPlay[channelPlay1] = 500;
				channelPlay2++;
				channelPlay1 = -1;
			}
			if (channelPlay2 == channelCount) {
				channelPlay1 = channelCount+10;
			}
		}
	}
}
/*if (latenzPlay[channelPlay1] == min(latenzPlay[0], min(latenzPlay[1], min(latenzPlay[2], min(latenzPlay[3], min(latenzPlay[4], min(latenzPlay[5], min(latenzPlay[6], min(latenzPlay[7], min(latenzPlay[8], min(latenzPlay[9], min(latenzPlay[10], min(latenzPlay[11], min(latenzPlay[12], min(latenzPlay[13], min(latenzPlay[14], latenzPlay[15])))))))))))))))) {*/
void beatClock2(byte channelPlay){
	for (byte i=0; i<16; i++){
		if (ansichtActive[channelPlay][ansichtWechsel[i][ansichtPlay[channelPlay]]] == HIGH){
			ansichtNext = i;
			i = 17;
		}
	}
	for (byte i=ansichtPlay[channelPlay] - 1; i<16 && i>=0; i--){
		if (ansichtActive[channelPlay][i] == HIGH){
			ansichtPre = i;
			i = 17;
		}
	}
	for (byte i=0; i<16; i++){
		if (ansichtActive[channelPlay][i] == HIGH){
			ansichtFirst = i;
			i = 17;
		}
	}
	for (byte i=15; i<16 && i>=0; i--){
		if (ansichtActive[channelPlay][i] == HIGH){
			ansichtLast = i;
			i = 17;
		}
	}
	if ((zaehlerClock + noteValue[channelPlay] - 1) % noteValue[channelPlay] == 0){
		clock2[channelPlay] = zaehlerClock;
		if (noteTriplet[channelPlay] == LOW){
			if (steps[channelPlay] < 15){
				steps[channelPlay]++;
			} else {
				steps[channelPlay] = 0;
			}
		} else {
			if (steps[channelPlay] < 11){
				steps[channelPlay]++;
			} else {
				steps[channelPlay] = 0;
			}
		}
		if (ansichtPlay[channelPlay] == ansichtView[channelPlay]){
			if (function == LOW && channelPlay == channelView){
				for (byte i=0; i<128; i++){
					if (softwareMatrix[channelView][ansichtPlay[channelView]][i] == LOW){
						trellis.clrLED(i);
					} else {
						trellis.setLED(i);
					}
				}
			}
		}
		if (channelActive[channelPlay] == HIGH){
			for (achtNoten=0; achtNoten<8; achtNoten++){
				if (ansichtActive[channelPlay][ansichtPlay[channelPlay]] == HIGH && noteMute[channelPlay][achtNoten] == LOW){
					if (softwareMatrix[channelPlay][ansichtPlay[channelPlay]][hardwareMatrix[achtNoten][steps[channelPlay]]] == HIGH){
						if (longNotes[channelPlay] == LOW){
							note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = HIGH;
							if (latenz[channelPlay] == 0){
								noteOnVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
							} else {
								playLater[channelPlay][achtNoten] = HIGH;
								playLaterMillis[channelPlay][achtNoten] = millis();
								playLaterNotenWert[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								playLaterVolumeWert[channelPlay][achtNoten] = volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
							}
						} else if ((softwareMatrix[channelPlay][ansichtPlay[channelPlay]][hardwareMatrix[achtNoten][steps[channelPlay]-1]] == LOW || notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]-1] != notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]]) && steps[channelPlay] > 0){
							note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = HIGH;
							if (latenz[channelPlay] == 0){
								noteOnVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
							} else {
								playLater[channelPlay][achtNoten] = HIGH;
								playLaterMillis[channelPlay][achtNoten] = millis();
								playLaterNotenWert[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								playLaterVolumeWert[channelPlay][achtNoten] = volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
							}
						} else if ((softwareMatrix[channelPlay][ansichtPre][hardwareMatrix[achtNoten][15]] == LOW || notenWert[channelPlay][ansichtPre][achtNoten][15] != notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]]) && steps[channelPlay] == 0 && ansichtPlay[channelPlay] > ansichtFirst){
							note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = HIGH;
							if (latenz[channelPlay] == 0){
								noteOnVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
							} else {
								playLater[channelPlay][achtNoten] = HIGH;
								playLaterMillis[channelPlay][achtNoten] = millis();
								playLaterNotenWert[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								playLaterVolumeWert[channelPlay][achtNoten] = volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
							}
						} else if ((softwareMatrix[channelPlay][15][hardwareMatrix[achtNoten][ansichtLast]] == LOW || notenWert[channelPlay][ansichtLast][achtNoten][15] != notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]]) && steps[channelPlay] == 0 && ansichtPlay[channelPlay] == ansichtFirst){
							note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = HIGH;
							if (latenz[channelPlay] == 0){
								noteOnVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
							} else {
								playLater[channelPlay][achtNoten] = HIGH;
								playLaterMillis[channelPlay][achtNoten] = millis();
								playLaterNotenWert[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								playLaterVolumeWert[channelPlay][achtNoten] = volumeWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
							}
						}
						if (channelPlay == channelView && function == LOW && ansichtPlay[channelPlay] == ansichtView[channelPlay]){
							trellis.clrLED(hardwareMatrix[achtNoten][steps[channelPlay]]);
						}
					} else if (channelPlay == channelView && function == LOW && ansichtPlay[channelPlay] == ansichtView[channelPlay]){
						trellis.setLED(hardwareMatrix[achtNoten][steps[channelPlay]]);
					}
				}
			}
			if (function == LOW && channelPlay == channelView && ansichtPlay[channelPlay] == ansichtView[channelPlay]){
				trellis.writeDisplay();
			}
		}
	}
	if (channelActive[channelPlay] == HIGH){
		if(zaehlerClock == clock2[channelPlay] + noteValue[channelPlay] - 1){
			freeLiveNotes = HIGH;
			for (byte i=0; i<128; i++){
				if (liveIsPressed[i] > 0){
					livePlay(i, liveIsPressed[i]);
				}
			}
			for (byte i=0; i<128; i++){
				if (liveIsUnPressed[i] == HIGH){
					liveIsPressed[i] = 0;
					liveIsUnPressed[i] = LOW;
				}
			}
			for (achtNoten=0; achtNoten<8; achtNoten++){
				if (ansichtActive[channelPlay][ansichtPlay[channelPlay]] == HIGH && noteMute[channelPlay][achtNoten] == LOW){
					if (softwareMatrix[channelPlay][ansichtPlay[channelPlay]][hardwareMatrix[achtNoten][steps[channelPlay]]] == HIGH){
						if (longNotes[channelPlay] == LOW){
								note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = LOW;
								if (latenz[channelPlay] == 0){
									noteOffVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][achtNoten] = HIGH;
									playLaterOffMillis[channelPlay][achtNoten] = millis();
									playLaterOffNote[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								}
						} else if (channelPlay == channelView && liveIsPressed[notenWert[channelView][ansichtPlay[channelView]][achtNoten][steps[channelView]]] > 0){
							
						} else {
							if ((softwareMatrix[channelPlay][ansichtPlay[channelPlay]][hardwareMatrix[achtNoten][steps[channelPlay]+1]] == LOW || notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]+1] != notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]]) && steps[channelPlay] < 15){
								note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = LOW;
								if (latenz[channelPlay] == 0){
									noteOffVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][achtNoten] = HIGH;
									playLaterOffMillis[channelPlay][achtNoten] = millis();
									playLaterOffNote[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								}
							} else if ((softwareMatrix[channelPlay][ansichtNext][hardwareMatrix[achtNoten][0]] == LOW || notenWert[channelPlay][ansichtNext][achtNoten][0] != notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]]) && steps[channelPlay] == 15 && ansichtPlay[channelPlay] < ansichtLast){
								note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = LOW;
								if (latenz[channelPlay] == 0){
									noteOffVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][achtNoten] = HIGH;
									playLaterOffMillis[channelPlay][achtNoten] = millis();
									playLaterOffNote[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								}
							} else if ((softwareMatrix[channelPlay][0][hardwareMatrix[achtNoten][ansichtFirst]] == LOW || notenWert[channelPlay][ansichtFirst][achtNoten][0] != notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]]) && steps[channelPlay] == 15 && ansichtPlay[channelPlay] == ansichtLast){
								note[channelPlay][hardwareMatrix[achtNoten][steps[channelPlay]]] = LOW;
								if (latenz[channelPlay] == 0){
									noteOffVoid(notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]],channelPlay);
								} else {
									playLaterOff[channelPlay][achtNoten] = HIGH;
									playLaterOffMillis[channelPlay][achtNoten] = millis();
									playLaterOffNote[channelPlay][achtNoten] = notenWert[channelPlay][ansichtPlay[channelPlay]][achtNoten][steps[channelPlay]];
								}
							}
						}
					}
				}
			}
		}
	}
	if (steps[channelPlay] == 15 && zaehlerClock == 24){
		for (byte i=0; i<16; i++){
			if (ansichtActive[channelPlay][ansichtWechsel[i][ansichtPlay[channelPlay]]] == HIGH) {
				if (ansichtPlay[channelPlay] == ansichtView[channelPlay]){
					ansichtView[channelPlay] = ansichtWechsel[i][ansichtView[channelPlay]];
				}
				ansichtPlay[channelPlay] = ansichtWechsel[i][ansichtPlay[channelPlay]];
				i = 17;
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
	trellis.setLED(hardwareMatrix[0][2]);
	trellis.setLED(hardwareMatrix[0][3]);
	trellis.setLED(hardwareMatrix[0][4]);
	trellis.setLED(hardwareMatrix[0][5]);
	trellis.setLED(hardwareMatrix[0][6]);
	if (noteValue[channelView] == 24 || noteValue[channelView] == 16){
		trellis.setLED(hardwareMatrix[0][7]);
	} else if (noteValue[channelView] == 12 || noteValue[channelView] == 8){
		trellis.setLED(hardwareMatrix[0][8]);
	} else if (noteValue[channelView] == 6 || noteValue[channelView] == 4){
		trellis.setLED(hardwareMatrix[0][9]);
	} else if (noteValue[channelView] == 3 || noteValue[channelView] == 2){
		trellis.setLED(hardwareMatrix[0][10]);
	}
	if (noteTriplet[channelView] == HIGH){
		trellis.setLED(hardwareMatrix[0][11]);
	}
	if (isInternal == HIGH){
		trellis.setLED(hardwareMatrix[0][12]);
	} else {
		trellis.clrLED(hardwareMatrix[0][12]);
	}
	trellis.setLED(hardwareMatrix[0][13]);
	trellis.setLED(hardwareMatrix[0][14]);
	for (byte x=0; x<16; x++){
		if (ansichtView[channelView] == x){
			trellis.setLED(hardwareMatrix[1][x]);
		}
	}
	for (byte x=0; x<16; x++){
		if (ansichtActive[channelView][x]==HIGH){
			trellis.setLED(hardwareMatrix[2][x]);
		}
	}
	for (byte x=0; x<channelCount; x++){
		if (channelView == x){
			trellis.setLED(hardwareMatrix[3][x]);
		}
		if (channelActive[x] == HIGH){
			trellis.setLED(hardwareMatrix[4][x]);
		}
		if (longNotes[x] == HIGH){
			trellis.setLED(hardwareMatrix[5][x]);
		}
	}
	trellis.setLED(hardwareMatrix[6][0]);
	trellis.setLED(hardwareMatrix[7][0]);
	trellis.setLED(hardwareMatrix[7][1]);
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
			MIDI1.write(noteSendOn);
			MIDI1.write(valueNoteOff);
			MIDI1.write(velocityZero);
			
			MIDI1.write(noteSendOff);
			MIDI1.write(valueNoteOff);
			MIDI1.write(velocityZero);
			break;
		case 1:
			MIDI2.write(noteSendOn);
			MIDI2.write(valueNoteOff);
			MIDI2.write(velocityZero);
			
			MIDI2.write(noteSendOff);
			MIDI2.write(valueNoteOff);
			MIDI2.write(velocityZero);
			break;
		case 2:
			MIDI3.write(noteSendOn);
			MIDI3.write(valueNoteOff);
			MIDI3.write(velocityZero);
			
			MIDI3.write(noteSendOff);
			MIDI3.write(valueNoteOff);
			MIDI3.write(velocityZero);
			break;
		case 3:
			MIDI4.write(noteSendOn);
			MIDI4.write(valueNoteOff);
			MIDI4.write(velocityZero);
			
			MIDI4.write(noteSendOff);
			MIDI4.write(valueNoteOff);
			MIDI4.write(velocityZero);
			break;
		case 4:
			MIDI5.write(noteSendOn);
			MIDI5.write(valueNoteOff);
			MIDI5.write(velocityZero);
			
			MIDI5.write(noteSendOff);
			MIDI5.write(valueNoteOff);
			MIDI5.write(velocityZero);
			break;
	}
	noteStopFunction[channelNoteOff][valueNoteOff] = LOW;
	noteStop[channelNoteOff][valueNoteOff] = LOW;
}
void noteOnVoid(byte valueNoteOn, byte volumeNoteOn, byte channelNoteOn){
	//delay (latenz[channelNoteOn]);
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
			MIDI1.write(noteSendOn);
			MIDI1.write(valueNoteOn);
			MIDI1.write(volumeNoteOn);
			break;
		case 1:
			MIDI2.write(noteSendOn);
			MIDI2.write(valueNoteOn);
			MIDI2.write(volumeNoteOn);
			break;
		case 2:
			MIDI3.write(noteSendOn);
			MIDI3.write(valueNoteOn);
			MIDI3.write(volumeNoteOn);
			break;
		case 3:
			MIDI4.write(noteSendOn);
			MIDI4.write(valueNoteOn);
			MIDI4.write(volumeNoteOn);
			break;
		case 4:
			MIDI5.write(noteSendOn);
			MIDI5.write(valueNoteOn);
			MIDI5.write(volumeNoteOn);
			break;
	}
	noteStop[channelNoteOn][valueNoteOn] = HIGH;
}
void startLED(byte LED1,byte LED2,byte LED3,byte LED4){
	trellis.setLED(LED1);
	trellis.setLED(LED2);
	trellis.setLED(LED3);
	trellis.setLED(LED4);
}
void stopLED(byte LED1,byte LED2,byte LED3,byte LED4){
	trellis.clrLED(LED1);
	trellis.clrLED(LED2);
	trellis.clrLED(LED3);
	trellis.clrLED(LED4);
}
void stopPlay(){
	if (function == LOW){
		if (ansichtPlay[channelView] == ansichtView[channelView]){
			for (byte i=0; i<128; i++){	 //Nimm den Strich weg
				if (softwareMatrix[channelView][ansichtPlay[channelView]][i] == LOW){
					trellis.clrLED(i);
				} else {
					trellis.setLED(i);
				}
			}
			trellis.writeDisplay();
		}
	}
	for (byte x=0; x<16; x++){
		steps[x] = -1;
		for (byte y=0; y<128; y++){
			if (noteStop[x][y] == HIGH){
				noteOffVoid(y,x);
			}
		}
	}
	isClock = LOW;
	clockStart = HIGH;
	zaehlerClock = 0;
	for (byte x=0; x<channelCount; x++){
		ansichtPlay[x] = 0;
	}
}

void setAnsichtView(byte x){
	for (byte c=0; c<channelCount; c++){
		if (ansichtActive[c][x]==HIGH || channelView==c || ansichtActive[channelView][x]==LOW) {
			ansichtView[c] = x;
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
			trellis.setLED(hardwareMatrix[c][2]);
			trellis.setLED(hardwareMatrix[c][4]);
			trellis.setLED(hardwareMatrix[c][6]);
			trellis.setLED(hardwareMatrix[c][7]);
			trellis.setLED(hardwareMatrix[c][9]);
			if (liveLowerNote[channelView] == 84 && c > 5){
				trellis.clrLED(hardwareMatrix[c][11]);
				trellis.clrLED(hardwareMatrix[c][13]);
			} else {
				trellis.setLED(hardwareMatrix[c][11]);
				trellis.setLED(hardwareMatrix[c][13]);
			}
		} else { //Black Keys
			trellis.setLED(hardwareMatrix[c][3]);
			trellis.setLED(hardwareMatrix[c][5]);
			trellis.setLED(hardwareMatrix[c][8]);
			if (liveLowerNote[channelView] == 84 && c > 5){
				trellis.clrLED(hardwareMatrix[c][10]);
				trellis.clrLED(hardwareMatrix[c][12]);
			} else {
				trellis.setLED(hardwareMatrix[c][10]);
				trellis.setLED(hardwareMatrix[c][12]);
			}
		}
	}
	trellis.writeDisplay();
}
void livePlay(byte liveNotePlay, byte liveVolumePlay){
	for (achtNoten=0; achtNoten<8; achtNoten++){
		if (notenWert[channelView][ansichtPlay[channelView]][achtNoten][steps[channelView]] == liveNotePlay && softwareMatrix[channelView][ansichtPlay[channelView]][hardwareMatrix[achtNoten][steps[channelView]]] == HIGH){
			freeLiveNotes = LOW;
		}
	}
	if (freeLiveNotes == HIGH){
		for (achtNoten=0; achtNoten<8; achtNoten++){
			if (softwareMatrix[channelView][ansichtPlay[channelView]][hardwareMatrix[achtNoten][steps[channelView]]] == LOW){
				softwareMatrix[channelView][ansichtPlay[channelView]][hardwareMatrix[achtNoten][steps[channelView]]] = HIGH;
				notenWert[channelView][ansichtPlay[channelView]][achtNoten][steps[channelView]] = liveNotePlay;
				volumeWert[channelView][ansichtPlay[channelView]][achtNoten][steps[channelView]] = liveVolumePlay;
				achtNoten = 8;
			}
		}
	}
	freeLiveNotes = HIGH;
}
/*
void readMIDI(){
	do {
		if (MIDI1.available() > 0){
			MIDIByte[0] = MIDI1.read();
			MIDIByte[1] = MIDI1.read();
			if (MIDIByte[0] >= 128){
				MIDIByte[2] = MIDI1.read();
				statusByte = MIDIByte[0];
				dataByte1 = MIDIByte[1];
				dataByte2 = MIDIByte[2];
			} else {
				dataByte1 = MIDIByte[0];
				dataByte2 = MIDIByte[1];
			}
			if (statusByte >= 144 && statusByte <= 159 && dataByte2 > 0){
				noteOnVoid(dataByte1, 100, 0);
				if (isPlay == HIGH && playLiveTest == LOW && playLive == HIGH){
					liveIsPressed[dataByte1] = HIGH;
					livePlay(dataByte1, dataByte2);
				}
			}
			if ((statusByte >= 128 && statusByte <= 143) || (statusByte >= 144 && statusByte <= 159 && dataByte2 == 0)){
				noteOffVoid(dataByte1, 0);
				if (isPlay == HIGH && playLiveTest == LOW && playLive == HIGH){
					liveIsPressed[dataByte1] = LOW;
				}
			}
		}
	}
	while ((MIDI1.available() > 2 && MIDIByte[0] >= 128) || (MIDI1.available() > 1 && MIDIByte[0] < 128));
}*/
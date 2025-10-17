
//toodo: Consider a more general status menu, that shows things like pulseResolution, tempo, etc?  How often to update?
//todo: it's kind of cool when I lower the events buffer way low and loop off of that, maybe make that something you can turn on/off
//todo: update code to use the createOffNote() function where appropraite


//TODO: try, it might not work well, but try having the SD matrix cells flash when notes come in?
//too slow for midi processing probably --- maybe if I pull it out and just update outside the while loop?

//TODO: add a menu option to decide if percolation requires same-channel notes. 

//TODO: Submenus.

//TODO: Do I want to re-add the ability to change channels mid-stutter with buttons?  It did lead to some cool effects.

//TODO: you have enough memory (you do), maybe consider way upping max pulse resolution and then working in grains?
//grains only really work with really dense notes though. This isn't audio.

//todo: I mean, in general figure out if you want to be able to assign specific synths to specific channels.
//maybe hold C and to enter "synth assign mode", then either navigate the OLED or enable us to enter numbers?
//B then rotate -/S/D/S&D?  Maybe enter the mode and then use the d pad to navigate the OLED, left-right to move around, up down to rotate type?

//doing so would let you do things like have CCs you can manipulate, enable jitter for drum machines, etc.  But how to do it?
//I guess an enum and pointers to arrays or something?  

//todo: Another thing you could do is enable specific CCs to be glitched in various ways.  Maybe would
//want to enable the scroll wheel for that. 

//todo: I could have drum machines only jitter within themselves, I could have drum machines
//be able to jitter across eachother, I could have drum machines be able to jitter with synths.
//unclear which is preferable. If we allow synths, do we fix a synth note or do we let it be the natural note number?
//presumably the latter, so hats become one note, bass becomes another.  Though that *will* be out of key, so...
//we'll start by just jittering internal to each machine. Jittering is also channel-internal, so this is easier.

//todo: note that with the new stutter buffer system, jittering no longer affects stutter, though
//it does affect the still incoming notes for the eventsBuffer. Could consider re-enabling jitter within the buffer
//as well.... I'm not sure.  Probably not.

//todo: maybe monitor for overload, some ideas in the chatgpt project for how to do that.

//todo: investigate what other things you can turn off (using menu options) in order to reduce loop time.

//todo: we could drain all the midi notes, then do ALL processing after, using the playTime to 
//figure out relative time (just like how we do for stutter).  Allegedly this may help with e.g. clock.

//todo: More aggressive lookups. Instead of storing e.g. channels and notes, we store a [16][128] bool array
//for channels and notes, so we can instantly check if a note is active.  This would use 16*128 = 2048 bits = 256 bytes per array, so 512 bytes total,
//which is not too bad.  Would speed up lookups a lot.  Would need to make sure to update these arrays properly though.

//todo: note off dropping-- worth it?  maybe two different prob comparisons?

//todo: I could sometimes, randomly, instead of playing an active note, swap it out with one from the stutter buffer.
//might add too much time overhead.

//todo: random arping? probably have to limit this to be during stutter only and/or limited to one synth at a time,
//which is awkward from a UI perspective.  Basically you have to track **all* on notes, and when >2 notes is received in a channel,
//arp it.  Which.... gets rough.  Maybe try this out as its own tool on a mega first. 

//per brett, I should only octave shift if I'm not doing an extension add.  But I dunno. I do want it to sound
//a little "wrong" some times, so it'll be glitchy.

//note delay by pulse Number instead? gets tricky, because off notes still need to be tied to time probably?  I guess could do them to the nearest pulse as well, shouldn't be that different. 

//restoreDefaults() should call the reset drum defaults function, so that I only have to update one thing in the code.

//todo: could also randomly just play notes from the stutter buffer sometimes.  Call this... blooping? I dunno
//this can be done outside of the midi read loop, so it should be fast.

//I like overall velocity coercion I think.  I have it resetting currentPulseInBar to 0 on start/continue, so it should be fine.
//should I also reset on new switch activations so it's possible to get "off"? Could also have a manual sync button? 
//just use the keypad, there's still two fre letters

//instead of cycling, should I have the groove selectable via menu?

//set retrigger synth time and maybe reretrigger prob?
//reorganize menus, order and maybe even sub menus
//synth swapping-- any active synth channel? If incorporated into jitter, you'd have to change the jitter tracking
//I don't want to add another button-- also basically impossible at this point.


//retriggering probably should send notes off, so we actually get the envelope to reset etc. Not sure if it does that right now. 

//TODO: remove Config Channels menu, move channel modify menu up, physically remove dipswitches.

#include "NoteStructs.h"
#include "SortedBuffer.h"
#include "MidiUtils.h"
#include "PinDefinitions.h"
#include "SystemConfig.h"
#include <EEPROM.h>
#include <Arduino.h>
#include <CircularBuffer.h>
#include <Adafruit_MCP23X17.h>
Adafruit_MCP23X17 mcpControls;
//midi stuff
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_RX_SERIAL, MIDIrx);
MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_TX_SERIAL, MIDItx); 

//display stuff
//- OLED Screen
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Display dimensions defined in SystemConfig.h
Adafruit_SSD1306 statusDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire2, PIN_OLED_RESET);
//- TM1637 (7-seg) screen
#include <TM1637Display.h>
TM1637Display seg7display(PIN_TM1637_CLK, PIN_TM1637_DIO); 

// Temporary view system
unsigned long tempViewStartTime = 0;
bool tempViewActive = false;
void (*tempViewCallback)() = nullptr;

// Display timing defined in SystemConfig.h

//menu display
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "MenuManager.h"
// Pin setup
// TFT pins defined in PinDefinitions.h


// SPI1 hardware peripheral
Adafruit_ST7789 menuTft = Adafruit_ST7789(&SPI1, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
MenuManager menu(menuTft);

byte oldPulseResolution;


//controls interaction stuff
//button helper -- not currently used for most buttons
struct ButtonHelper{
  
  int pinNumber;
  bool buttonState = false;
  bool lastButtonState = false;
  bool justPressed = false;

  bool update()//returns edge
  {
  lastButtonState = buttonState;
  buttonState = digitalRead(pinNumber) == HIGH;
  // Return true only if button was just pressed
  justPressed = (buttonState && !lastButtonState);
  if(justPressed){
    Serial.print("Pressed button on pin ");
    Serial.println(pinNumber);
  }
  return justPressed;
  }

  bool isPressed()
  {
    return buttonState;
  }


  //setup helper with given pin
 void setup(int pin, int mode = INPUT_PULLUP)
{
    pinNumber = pin;
    pinMode(pinNumber, mode);             // use passed mode or default
    buttonState = digitalRead(pinNumber) == HIGH;
    lastButtonState = buttonState;
}

};

struct SwitchHelper{
  int pinNumber;
  bool switchState = false;
  bool lastSwitchState = false;
  bool justChanged = false;

  bool update()//returns edge
  {
  lastSwitchState = switchState;
  switchState = digitalRead(pinNumber) == LOW;
  // Return true only if switch was just changed
  justChanged = (switchState != lastSwitchState);
  return justChanged;
  }

  bool isOn()
  {
    return switchState;
  }

  void setup(int pin)
  {
    pinNumber = pin;
    pinMode(pinNumber, INPUT_PULLUP);
    switchState = digitalRead(pinNumber) == HIGH;
    lastSwitchState = switchState;
  }
  

};

struct McpButtonHelper{
  Adafruit_MCP23X17* mcp;
  int pinNumber;
  bool buttonState = false;
  bool lastButtonState = false;
  bool justPressed = false;

  bool update()//returns edge
  {
  lastButtonState = buttonState;
  buttonState = mcp->digitalRead(pinNumber) == HIGH;
  // Return true only if button was just pressed
  justPressed = (buttonState && !lastButtonState);
  return justPressed;
  }

  bool isPressed()
  {
    return buttonState;
  }


  //setup helper with given pin
 void setup(int pin, Adafruit_MCP23X17* mcpInstance, int mode = INPUT_PULLUP)
{
    pinNumber = pin;
    Serial.print("Setting up MCP button on pin ");
    Serial.println(pinNumber);
    mcp = mcpInstance;
    mcp->pinMode(pinNumber, mode);
    mcp->digitalWrite(pinNumber, HIGH);
    buttonState = mcp->digitalRead(pinNumber) == HIGH;
    lastButtonState = buttonState;
}

};


//Control pad
// Menu control pins defined in PinDefinitions.h

McpButtonHelper menuRightButton;
McpButtonHelper menuLeftButton;
McpButtonHelper menuUpButton;
McpButtonHelper menuDownButton;
McpButtonHelper menuSelectButton;

// -----------------
// KEYPAD
// -----------------
struct MatrixKeypad {
  static const uint8_t ROWS = KEYPAD_ROWS;
  static const uint8_t COLS = KEYPAD_COLS;

  uint8_t rowPins[ROWS];
  uint8_t colPins[COLS];
  bool lastState[ROWS][COLS];

  char keymap[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };

  void begin(const uint8_t* rPins, const uint8_t* cPins) {
    for (uint8_t i = 0; i < ROWS; i++) rowPins[i] = rPins[i];
    for (uint8_t i = 0; i < COLS; i++) colPins[i] = cPins[i];

    for (uint8_t r = 0; r < ROWS; r++) {
      mcpControls.pinMode(rowPins[r], OUTPUT);
      mcpControls.digitalWrite(rowPins[r], HIGH);
    }

    for (uint8_t c = 0; c < COLS; c++) {
      mcpControls.pinMode(colPins[c], INPUT_PULLUP);
      mcpControls.digitalWrite(colPins[c], HIGH);
    }

    for (uint8_t r = 0; r < ROWS; r++)
      for (uint8_t c = 0; c < COLS; c++)
        lastState[r][c] = false;
  }

  void update() {
    for (uint8_t r = 0; r < ROWS; r++) {
      mcpControls.digitalWrite(rowPins[r], LOW);

      for (uint8_t c = 0; c < COLS; c++) {
        bool pressed = (mcpControls.digitalRead(colPins[c]) == LOW);

        if (pressed && !lastState[r][c]) onPress(r, c, keymap[r][c]);
        if (!pressed && lastState[r][c]) onRelease(r, c, keymap[r][c]);

        lastState[r][c] = pressed;
      }

      mcpControls.digitalWrite(rowPins[r], HIGH);
    }
  }

  char lastKeyPressed = 0;
  void onPress(uint8_t, uint8_t, char key) {
    lastKeyPressed = key;
    // Serial.print("Pressed: ");
    // Serial.println(key);
  }

  void onRelease(uint8_t, uint8_t, char key) {
    // Serial.print("Released: ");
    // Serial.println(key);
  }
};

// Keypad matrix pins (defined in PinDefinitions.h)
const uint8_t COL_PINS[4] = {MCP_KEYPAD_COL1, MCP_KEYPAD_COL2, MCP_KEYPAD_COL3, MCP_KEYPAD_COL4};
const uint8_t ROW_PINS[4] = {MCP_KEYPAD_ROW1, MCP_KEYPAD_ROW2, MCP_KEYPAD_ROW3, MCP_KEYPAD_ROW4};

MatrixKeypad keypad;

//debug flags defined in SystemConfig.h



//tempo tracker stuff
// #include "Tempo_Tracker.h"
// TempoTracker tempoTracker;


// Log button pin defined in PinDefinitions.h
ButtonHelper logButton;


//System configuration constants defined in SystemConfig.h


// --- LED Pins ---
// --- Button pins ---
// Button pins defined in PinDefinitions.h
ButtonHelper panicButton;

// --- Potentiometer Pins ---
// Potentiometer pin defined in PinDefinitions.h

// --- Loop buffer ---

struct PitchBender {
    byte channel;
    int16_t currentBend = 0;
    int16_t minBend;
    int16_t maxBend;
    byte numBendPasses;
    bool directionForward;

    long lastUpdate = 0;
    int bendStepInterval;   // ms between updates
    int bendStepSize;       // step size per update

    int pickRandomBend() {
        int bend;
        do {
            bend = random(-8192, 8192);
        } while (bend >= -800 && bend <= 800);
        return bend;
    }

    PitchBender() 
  : channel(0),
    currentBend(0),
    minBend(0),
    maxBend(0),
    numBendPasses(0),
    directionForward(true),
    lastUpdate(0),
    bendStepInterval(20),
    bendStepSize(5) {}

    PitchBender(byte ch) : channel(ch) {
        int a = pickRandomBend();
        int b = pickRandomBend();

        minBend = min(a, b);
        maxBend = max(a, b);
        bendStepInterval = random(5, 30);  // update interval in ms
        bendStepSize = random(5, 100);     // pitch increment per step
        numBendPasses = random(1, 5);      // number of passes (up or down = 1 pass)
        directionForward = (random(2) == 1);
        currentBend = directionForward ? minBend : maxBend;
        lastUpdate = millis();

        sendBend(); // start immediately
    }

    void update() {
        if (numBendPasses == 0) return;

        long now = millis();
        if (now - lastUpdate >= bendStepInterval) {
            lastUpdate = now;

            if (directionForward) {
                currentBend = min(currentBend + bendStepSize, maxBend);
                if (currentBend == maxBend) {
                    directionForward = false;
                    numBendPasses--;
                }
            } else {
                currentBend = max(currentBend - bendStepSize, minBend);
                if (currentBend == minBend) {
                    directionForward = true;
                    numBendPasses--;
                }
            }

            sendBend();
        }
    }

    void sendBend() {
        MIDItx.sendPitchBend(currentBend, channel);
    }

    void finishBend() {
        MIDItx.sendPitchBend(0, channel);
    }
};

struct DrumMachine{
  int channel;
  byte instrumentsLearned[MAX_INSTRUMENTS_PER_DRUM_MACHINE];
  int numInstruments = 0;

  //construct from channel
  DrumMachine(int ch) : channel(ch) {
    numInstruments = 0;
  }

  //default constructor
  DrumMachine() : channel(0) {
    numInstruments = 0;
  }
};

struct DelayedNoteOn{
  byte note;
  unsigned long playTime;
  unsigned long delayTime;

  //construct from note, playtime, and delay tim
  DelayedNoteOn(byte n, unsigned long pt, unsigned long dt) : note(n), playTime(pt), delayTime(dt) {}

  //default constructor
  DelayedNoteOn() : note(0), playTime(0), delayTime(0) {}
};



// --- Buffers ---
CircularBuffer<MidiEvent, MAX_EVENTS> eventsBuffer;
CircularBuffer<MidiEvent, MAX_EVENTS> stutterBuffer; //we dump eventsBuffer into this when we start stuttering

CircularBuffer<unsigned long, MAX_PULSES_PER_STUTTER> pulseStartTimes;
CircularBuffer<unsigned long, MAX_PULSES_PER_STUTTER> stutterPulseStartTimes; // not sure I need this but might as well make it now.
//todo: yeah, you only need to grab the first pulseStartTimes element, to set the loop start time or something like that.
//there's no need to copy over all the pulseSTartTime elements. Can remove this later to save memory.

CircularBuffer<MidiEvent, RETRIGGER_BUFFER_SIZE> retriggerBuffer; //maybe change this to "delayedNotesBuffer" if we end up doing that (holding back some notes for a little)
CircularBuffer<PitchBender, NUM_ACTIVE_PITCHBENDS> pitchbendBuffer;

struct PlayTimeCmp {
    bool operator()(const MidiEvent& a, const MidiEvent& b) const {
        return a.playTime > b.playTime;
    }
};

SortedBuffer<MidiEvent, MAX_DELAYED_NOTES, PlayTimeCmp> delayedNotesBuffer; //sorted buffer for delayed notes
DelayedNoteOn delayedOnNotes[16][MAX_DELAYED_NOTES/16]; //array of delayed on notes, divided by channel

int numDelayedOnNotes[16] = {0};

JitteredNote jitterBuffer[JITTER_BUFFER_SIZE];
int jitterCount = 0;

JitteredNote drumJitterBuffer[DRUM_JITTER_BUFFER_SIZE];
int drumJitterCount = 0;

PercNote percBuffer[PERC_BUFFER_SIZE];
int percCount = 0;

DrumMachine drumMachines[MAX_DRUM_MACHINES];
int numDrumMachines = 0;

// --- Script states ---
bool isLooping = false;
bool prevLooping = false;

// Generic blink state variables (can be reused by other blink routines)
unsigned long blinkStartTime = 0;
int blinkStep = 0;
const int blinkPatternLength = BLINK_PATTERN_LENGTH;  // Defined in SystemConfig.h
bool isBlinking = false;


// --- Button states ---
bool stutterButtonPressed = false;
bool prevStutterPressed = false;

// --- Dipswitch states ---
//these encode the midi dipswitch states
// uint16_t oldDrumState = 0;



// --- Pot States --- 
int stretchPotValueLiteral = 0;
float stretchPotValueFloat = 0;
float stretchValue = 1.0;
float oldStretchValue = 1.0;
float lastTimeStretchPotUpdated = 0;


//loop control states
unsigned long loopStartTime;
bool loopInitialized = false;
unsigned long playbackStartTime;
unsigned long playbackEndTime;
unsigned long playbackLength;
//this is related to pulse resolution
int currentPulse = 0;

//this is progress through a full bar, maybe off-beat, of 96 pulses from the start.
byte currentPulseInBar = 0;




//misc constants defined in SystemConfig.h
unsigned long lastStutterChange = 0;
unsigned long lastButtonChangeTime = 0;
MidiEvent dummyEvent;
unsigned long dummyTime;
int numSynthNoteOnsInStutterBuffer;
bool maybeDropNote = true;
bool haveSeenClock = false;

uint16_t drumState = 0;
uint16_t synthState = 0;


//one per pulse
byte velocityCoerceValues[10][16] = {
 // Jagged Accent Stabs – high punchy hits with soft ghost notes in between
 {127, 20, 85, 18, 124, 15, 90, 22, 126, 19, 83, 17, 125, 16, 88, 21},

 // Rolling Push–Pull – mid-range tension alternating with strong accents
 {118, 15, 92, 28, 121, 20, 87, 25, 124, 18, 84, 22, 119, 17, 91, 24},

 // Ghost-Laden Groove – extremely soft notes punctuated by isolated accents
 {127, 15, 82, 18, 123, 17, 88, 16, 125, 20, 85, 15, 121, 18, 90, 16},

 // Slow Swell and Crash – dynamic swells ending in hard accents
 {115, 25, 86, 20, 122, 30, 84, 22, 127, 28, 92, 19, 120, 24, 89, 21},

 // Offbeat Snap – sharp hits on offbeats with subtle soft notes
 {123, 18, 87, 15, 119, 20, 91, 16, 127, 19, 84, 15, 121, 22, 90, 17},

 // Chaotic Pulse – unpredictable mix of extreme highs and lows
 {126, 15, 92, 28, 124, 16, 88, 25, 127, 20, 83, 18, 125, 17, 91, 22},

 // Narcolepsy - most notes are very soft, sporadic high velocity hits
 {127, 10, 100, 12, 5, 15, 95, 14, 8, 11, 120, 13, 124, 14, 3, 12},

 // Nuclear Burst – almost silent then nuclear blasts
 {3, 2, 127, 1, 5, 2, 127, 3, 1, 4, 127, 2, 6, 3, 127, 1},

 // Razor Saw – alternating top-speed strikes and needle whispers
 {127, 4, 122, 3, 127, 2, 120, 5, 127, 3, 124, 1, 127, 4, 123, 2},

 // Avalanche – soft snow then collapsing with massive hits
 {6, 10, 15, 20, 40, 70, 100, 127, 10, 6, 8, 127, 15, 4, 127, 2}
};

const char* velocityCoerceLabels[10] = {
  "Jagged Accent Stabs",
  "Rolling Push-Pull",
  "Ghost-Laden Groove",
  "Slow Swell and Crash",
  "Offbeat Snap",
  "Chaotic Pulse",
  "Narcolepsy",
  "Nuclear Burst",
  "Razor Saw",
  "Avalanche"
};


byte velocityCoercionIdx = 0;



//note that active notes tracking for 256 has been solved by isolating the MIDI in, but it's still
//useful to have.
#ifdef ACTIVE_NOTES_DEBUG
//debug stuff - MAX_NOTES defined in SystemConfig.h
//there's only 128 midi notes but we're erroneously seeing notes greater than that, so we need a buffer to handle those as well.
bool activeNotes[MAX_NOTES] = { false };
#endif

// --- Menu States ---
//these two are set by button-switches. We'll keep it like that. 
bool retriggerOn = false; 
bool jitterOn = false; 
// Switch pins defined in PinDefinitions.h
SwitchHelper retriggerSwitch;
SwitchHelper jitterSwitch;

//temp
bool velocityCoercion = false;
// Velocity coercion switch pin defined in PinDefinitions.h
SwitchHelper velocityCoercionSwitch;

byte octaveShiftOption = 0; //todo: implement
/*
Octave shift options: 
  0: No Octave Shift
  1: Shift only fundamentals
  2: Shift on all
*/

// int debugSerialCount = 0;

// --- helper functions ---

//properly, we should be able to just do sendNoteON, but why risk it.
void forwardNote(MidiEvent event) {

  if (event.type == MIDI_NOOP) {
    return;
  }

//going to get screwy with CC and other non-notes probably's probably
  #ifdef ACTIVE_NOTES_DEBUG
      if (!activeNotes[event.note]) {
        Serial.print(F("Saw note#"));
        Serial.print(event.note);
        Serial.print(F(" channel#"));
        Serial.println(event.channel);
        activeNotes[event.note] = true;
      }
  #endif

  if(event.type==midi::NoteOn){
    //debug -- send note number, channel, and type
    // Serial.print("[");
    // Serial.print(millis());
    // Serial.print(F("] Forwarding NoteOn: note#"));
    // Serial.print(event.note);
    // Serial.print(F(" channel#"));
    // Serial.println(event.channel);
    //end debug
    MIDItx.sendNoteOn(event.note, event.velocity, event.channel);
  } else if(event.type==midi::NoteOff){
    //debug
        //debug -- send note number, channel, and type
    // Serial.print("[");
    // Serial.print(millis());
    // Serial.print(F("] Forwarding NoteOff: note#"));
    // Serial.print(event.note);
    // Serial.print(F(" channel#"));
    // Serial.println(event.channel);
    //end debug
    MIDItx.sendNoteOff(event.note, 0, event.channel);
  }
  else{
    Serial.print("Somehow being asked to sending note of type ");
    Serial.println(event.type);
  }
}

void midiPanic() {
  for (int ch = 1; ch <= 16; ch++) {
    MIDItx.sendControlChange(123, 0, ch);
  }
}

bool checkIfMIDIOn(byte channel_num){
  if(channel_num==0){return true;}
  if(channel_num>16){
    Serial.println(F("Got invalid channel!")); 
    return false;}
  return menu.drumMIDIenabled[channel_num-1]||menu.synthMIDIenabled[channel_num-1];
}

void killTrackedChannelsNotes(){
  for(int ch=1; ch<=16; ch++){
    if(checkIfMIDIOn(ch)){
      MIDItx.sendControlChange(123,0,ch);
    }
  }
}

byte pickRandomElement(const byte arr[], byte arrSize) {
    byte index = random(0, arrSize);
    return arr[index];
}

//this should be passed in synthMIDIEnabled or drumMIDIEnabled. Or, maybe later, we'll combine the two? 
//note it only works for channel arrays though, it requires size 16
byte getRandomActiveChannel(const bool arr[16]) {
    int selected = -1;
    int n = 0; // number of active channels seen so far

    for (int i = 0; i < 16; i++) {
        if (arr[i]) {
            if (random(++n) == 0) { // pick this one with probability 1/n
                selected = i+1;
            }
        }
    }

    if (selected == -1) {
        Serial.println(F("Invalid channel returned!"));
        return 255; // no active channel
    }

    return selected;
}


MidiEvent createOffNote(MidiEvent note, long timeOff = millis()){
  //todo: how do we want to handle non-note midi events?
  //currently we'll just... pass through? Which we'll also do for off notes. But of course make sure we don't do that. 
  if(note.type!=midi::NoteOn){return note;}
  MidiEvent offNote;
  offNote.type = midi::NoteOff;
  offNote.note = note.note;
  offNote.channel = note.channel;
  offNote.velocity = note.velocity;
  offNote.playTime = timeOff;
  offNote.played = false;
  offNote.pulseNumber = note.pulseNumber; //this *could* get fucky-wucky, pay attention if this matters.
  return offNote;
}

bool randomProbResult(byte success_rate, long upper_bound = 100){
  long random_num = random(0,upper_bound);
  return random_num<success_rate;
}

byte randomOctave() {
  byte random_num = random(100); // 0–99

  if (random_num >= 90) return 2; // 90–99 → 10%
  if (random_num >= 60) return 1; // 60–89 → 30%
  return 0;                        // 0–59 → 60%
}


void handleClock() {
    MIDItx.sendRealTime(midi::Clock);
    haveSeenClock = true;
}

void restoreDefaults() {
  // Drum defaults: false, false, false, false, false, false, false, false, false, true, true, false, false, false, false, false
    bool drumDefaults[16] = {false, false, false, false, false, false, false, false, true, true, true, false, false, false, false, false};
    bool synthDefaults[16] = {true, true, true, true, true, true, true, true, false, false, false, true, true, true, true, true};
    drumState = 0;
    synthState = 0;
    for (int i = 0; i < 16; i++) {
      menu.drumMIDIenabled[i] = drumDefaults[i];
      menu.synthMIDIenabled[i] = synthDefaults[i];
      if (drumDefaults[i]) drumState |= (1 << i);
      if (synthDefaults[i]) synthState |= (1 << i);
    }
    // Set menu defaults
  menu.stutterLengthActiveIdx = 9; // 1/4 note
  menu.offsetActiveIdx = 3; // Major Offset
  menu.noteJitterProb = 10;
  menu.drumJitterProb = 10;
  menu.retriggerProb = 10;
  menu.stutterTemperature = 0;
  menu.retriggerSynths = false;
  menu.randomDropProb = 0;
  menu.delayNoteProb = 0;
  menu.pitchbendProb = 10;
  menu.minDelayTime = 500;
  menu.maxDelayTime = 4000;
    // Immediately save defaults to EEPROM so future boots load correct values
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.put(EEPROM_ADDR_DRUM_STATE, drumState);
    EEPROM.put(EEPROM_ADDR_SYNTH_STATE, synthState);
  menu.saveStutterLength(EEPROM_ADDR_STUTTER_LENGTH);
  menu.saveOffset(EEPROM_ADDR_OFFSET);
  menu.saveNoteJitterProb(EEPROM_ADDR_JITTER_PROB);
  menu.saveDrumJitterProb(EEPROM_ADDR_DRUM_JITTER_PROB);
  menu.saveRetriggerProb(EEPROM_ADDR_RETRIGGER_PROB);
  menu.saveStutterTemperature(EEPROM_ADDR_STUTTER_TEMPERATURE);
  menu.saveSynthRetrigger(EEPROM_ADDR_SYNTH_RETRIGGER);
  menu.saveRandomDropProb(EEPROM_ADDR_RANDOM_DROP_PROB);
  menu.saveDelayNoteProb(EEPROM_ADDR_DELAY_NOTE_PROB);
  menu.savePitchbendProb(EEPROM_ADDR_PITCHBEND_PROB);
  menu.saveMinDelayTime(EEPROM_ADDR_MIN_DELAY_TIME);
  menu.saveMaxDelayTime(EEPROM_ADDR_MAX_DELAY_TIME);

}

void disableAll() {
  // drums do not get disabled, that'd be annoying.
  // Drum defaults: false, false, false, false, false, false, false, false, false, true, true, false, false, false, false, false
    bool drumDefaults[16] = {false, false, false, false, false, false, false, false, true, true, true, false, false, false, false, false};
    bool synthDefaults[16] = {true, true, true, true, true, true, true, true, false, false, false, false, true, true, true, true};
    drumState = 0;
    synthState = 0;
    for (int i = 0; i < 16; i++) {
      menu.drumMIDIenabled[i] = drumDefaults[i];
      menu.synthMIDIenabled[i] = synthDefaults[i];
      if (drumDefaults[i]) drumState |= (1 << i);
      if (synthDefaults[i]) synthState |= (1 << i);
    }
    // Set menu defaults
  menu.stutterLengthActiveIdx = 9; // 1/4 note
  menu.offsetActiveIdx = 0; // No Offset
  menu.noteJitterProb = 0;
  menu.drumJitterProb = 0;
  menu.retriggerProb = 0;
  menu.stutterTemperature = 0;
  menu.retriggerSynths = false;
  menu.randomDropProb = 0;
  menu.delayNoteProb = 0;
  menu.pitchbendProb = 0;
  menu.minDelayTime = 500;
  menu.maxDelayTime = 4000;
    // Immediately save defaults to EEPROM so future boots load correct values
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.put(EEPROM_ADDR_DRUM_STATE, drumState);
    EEPROM.put(EEPROM_ADDR_SYNTH_STATE, synthState);
  menu.saveStutterLength(EEPROM_ADDR_STUTTER_LENGTH);
  menu.saveOffset(EEPROM_ADDR_OFFSET);
  menu.saveNoteJitterProb(EEPROM_ADDR_JITTER_PROB);
  menu.saveDrumJitterProb(EEPROM_ADDR_DRUM_JITTER_PROB);
  menu.saveRetriggerProb(EEPROM_ADDR_RETRIGGER_PROB);
  menu.saveStutterTemperature(EEPROM_ADDR_STUTTER_TEMPERATURE);
  menu.saveSynthRetrigger(EEPROM_ADDR_SYNTH_RETRIGGER);
  menu.saveRandomDropProb(EEPROM_ADDR_RANDOM_DROP_PROB);
  menu.saveDelayNoteProb(EEPROM_ADDR_DELAY_NOTE_PROB);
  menu.savePitchbendProb(EEPROM_ADDR_PITCHBEND_PROB);
  menu.saveMinDelayTime(EEPROM_ADDR_MIN_DELAY_TIME);
  menu.saveMaxDelayTime(EEPROM_ADDR_MAX_DELAY_TIME);

}

//function prototypes
void playSavedPulses();


bool readStutterButton();

void clearOldNotes(int expiredPulse);

MidiEvent createEmptyEvent(byte pulseNumber);

void manglerHandleClock();

void triggerBufferFullBlink();
void updateBufferFullBlink();
void checkPulseBufferFullSetLED();


void setupStatusDisplay();
void drawSDMatrix(bool drumArr[16], bool synthArr[16]);
void drawStretchDisplay();

void recoverStatusDisplay();
bool safeStatusDisplay();

void cueRetriggeredNote(MidiEvent me);
void playRetriggeredNotes();

void pruneFinishedBends();
void pushBend(int channel);
void updateBends();
bool checkForNoteOn(byte noteOffNumber);
void startBufferFullBlink();
void pruneBends();
void showPanicDisplay();
void drawStretchStatusDisplay();

MidiEvent maybeNoteNumberJitter(MidiEvent event);
MidiEvent maybeDrumJitter(MidiEvent event);

MidiEvent maybePercolateNote(MidiEvent event, byte index_number);

void clearAllPercolatedNotes();
void clearStutterBuffer();
void clearStutterPulseTimes();

bool maybeLearnInstrument(byte channel, byte note);
bool maybeLearnDrumMachine(byte channel);
void refreshDrumMachines();

DelayedNoteOn checkForDelayedOn(byte channelNumber, byte noteNumber);
void playDelayedNotes();

void showVelocityCoercionLabel();

// --- State variables for pending updates ---

void setup() {
  // put your setup code here, to run once:
  
  // Blink LED to confirm Teensy is running (before serial init)
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  
  Serial.begin(115200);

  Serial.println(F("Starting setup"));
  
  Wire2.begin();      
     
  if (!mcpControls.begin_I2C(0x25, &Wire2)) {
    Serial.println("MCP23017 not found on 0x25!");
    for (;;);
  }
  Serial.println("MCP23017 on 0x25 initialized.");

  Serial.println(F("Entering setup status display"));
  //display setup
  setupStatusDisplay();
  Serial.println(F("Setting pins"));
  pinMode(PIN_STUTTER_SWITCH, INPUT); 
 
  panicButton.setup(PIN_PANIC_BUTTON);

  pinMode(PIN_BUFFER_FULL_LED, OUTPUT);
  logButton.setup(PIN_LOG_BUTTON, INPUT);

  retriggerSwitch.setup(PIN_RETRIGGER_SWITCH);
  jitterSwitch.setup(PIN_JITTER_SWITCH);
  
  //temp 
  velocityCoercionSwitch.setup(PIN_VELOCITY_COERCION);

  Serial.println("Turning on buffer LED pin");
  digitalWrite(PIN_BUFFER_FULL_LED, HIGH);
  delay(100);

  Serial.println(F("Turning on MIDI"));
  MIDItx.begin(MIDI_CHANNEL_OMNI);
  MIDItx.turnThruOff();
  MIDIrx.begin(MIDI_CHANNEL_OMNI);
  MIDIrx.turnThruOff();
  Serial.println(F("MIDI started"));
  
  MIDIrx.setHandleClock(handleClock);

//setup 7seg-- 0x0f is 15.
  seg7display.setBrightness(0x0f); 


  drawStretchDisplay();

  digitalWrite(PIN_BUFFER_FULL_LED, LOW);



  // //setup menu screen
  SPI1.begin();
  // Serial.println("Set up SPI1");

  menuTft.init(TFT_INIT_HEIGHT, TFT_INIT_WIDTH);   // Dimensions from SystemConfig.h
  menuTft.setRotation(3);   // landscape
  Serial.println("init screen");

  menuTft.fillScreen(ST77XX_BLACK);

  menuTft.setTextColor(ST77XX_CYAN);
  menuTft.setCursor(MARGIN_LEFT, MARGIN_TOP);
  menuTft.setTextSize(3);
  menuTft.print(F("MIDI Glitcher"));
  menuTft.setCursor(MARGIN_LEFT+10, MARGIN_TOP + 10);
  menuTft.setTextColor(ST77XX_MAGENTA);
  menuTft.print(F("MIDI Glitcher"));
  menuTft.setCursor(MARGIN_LEFT + 20, MARGIN_TOP + 20);
  menuTft.setTextColor(ST77XX_YELLOW);
  menuTft.print(F("MIDI Glitcher"));
  //print "press select to start" in bottom right corner
  menuTft.setCursor((MARGIN_LEFT - MARGIN_RIGHT) / 2, TFT_HEIGHT - MARGIN_BOTTOM - 16);
  menuTft.setTextColor(ST77XX_WHITE);
  menuTft.setTextSize(1);
  menuTft.print(F("Press Select to Start"));

  Serial.println("setting up keyboard");
  keypad.begin(ROW_PINS, COL_PINS);

  Serial.println("Setting up menu buttons");
  menuRightButton.setup(MCP_MENU_RIGHT, &mcpControls);
  menuLeftButton.setup(MCP_MENU_LEFT, &mcpControls);
  menuUpButton.setup(MCP_MENU_UP, &mcpControls);
  menuDownButton.setup(MCP_MENU_DOWN, &mcpControls);
  menuSelectButton.setup(MCP_MENU_SELECT, &mcpControls);
  Serial.println("Menu buttons set up");


  //load EEPROM states and menu settings
  Serial.println("Loading EEPROM states");
  uint8_t magic = EEPROM.read(EEPROM_ADDR_MAGIC);
 
  if (magic == EEPROM_MAGIC) {
  EEPROM.get(EEPROM_ADDR_DRUM_STATE, drumState);
  EEPROM.get(EEPROM_ADDR_SYNTH_STATE, synthState);
  // Load menu settings from fixed addresses
  menu.stutterLengthActiveIdx = EEPROM.read(EEPROM_ADDR_STUTTER_LENGTH);
  menu.offsetActiveIdx = EEPROM.read(EEPROM_ADDR_OFFSET);
  menu.noteJitterProb = EEPROM.read(EEPROM_ADDR_JITTER_PROB);
  menu.drumJitterProb = EEPROM.read(EEPROM_ADDR_DRUM_JITTER_PROB);
  menu.retriggerProb = EEPROM.read(EEPROM_ADDR_RETRIGGER_PROB);
  menu.stutterTemperature = EEPROM.read(EEPROM_ADDR_STUTTER_TEMPERATURE);
  menu.retriggerSynths = EEPROM.read(EEPROM_ADDR_SYNTH_RETRIGGER);
  menu.randomDropProb = EEPROM.read(EEPROM_ADDR_RANDOM_DROP_PROB);
  menu.delayNoteProb = EEPROM.read(EEPROM_ADDR_DELAY_NOTE_PROB);
  EEPROM.get(EEPROM_ADDR_PITCHBEND_PROB, menu.pitchbendProb);
  EEPROM.get(EEPROM_ADDR_MIN_DELAY_TIME, menu.minDelayTime);
  EEPROM.get(EEPROM_ADDR_MAX_DELAY_TIME, menu.maxDelayTime);

  Serial.print("Loaded noteJitterProb from EEPROM: ");
  Serial.println(menu.noteJitterProb);
  Serial.print("Loaded stutterTemperature from EEPROM: ");
  Serial.println(menu.stutterTemperature);
  Serial.println("EEPROM states and menu settings loaded");
  } else {
    Serial.println("EEPROM magic byte not found, using defaults");
    restoreDefaults();
    //create drum machines on startup. TODO: functionality to relearn drum machines when we update
    //drumMachineMIDIEnabled

  }



  // Decode drumMIDIenabled and synthMIDIenabled arrays
  for (int i = 0; i < 16; i++) {
    menu.drumMIDIenabled[i] = (drumState & (1 << i)) ? true : false;
    menu.synthMIDIenabled[i] = (synthState & (1 << i)) ? true : false;
  }
  Serial.println(F("Drawing SD matrix again (why?)"));
  drawSDMatrix(menu.drumMIDIenabled, menu.synthMIDIenabled); 
  oldPulseResolution = menu.pulseResolution;
  // To save a setting after it changes, use:
  // EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
  // EEPROM.put(EEPROM_ADDR_DRUM_STATE, drumState);
  // EEPROM.put(EEPROM_ADDR_SYNTH_STATE, synthState);
  // EEPROM.write(EEPROM_ADDR_STUTTER_LENGTH, menu.stutterLengthActiveIdx);
  // EEPROM.write(EEPROM_ADDR_OFFSET, menu.offsetActiveIdx);
  // EEPROM.write(EEPROM_ADDR_JITTER_PROB, menu.noteJitterProb);
  // EEPROM.write(EEPROM_ADDR_RETRIGGER_PROB, menu.retriggerProb);
 

  Serial.println(F("Ending setup"));
}



void loop() {
 
  //first, we check to see if we got a clock message. If we did, we handle it and return. This still doesn't fix
  //the overwhelmed by notes problem.
  while(MIDIrx.read()){

    // debugSerialCount = max(debugSerialCount, Serial8.available());

      byte type = MIDIrx.getType();
     
      if(type==midi::Clock){
        manglerHandleClock();
      }
      else if (type==midi::NoteOn || type==midi::NoteOff){
        //we handle notes below
  
     //we've already checked if we have a clock
     
      //non-clock handling
      
        //skip the read if we've JUST changed the stutter button state -- this doesn't apply to clock
    if (millis() - lastButtonChangeTime < 50) {
        continue;
      }

        byte channel = MIDIrx.getChannel();
    
        //we still only append to the events buffer if we're tracking that midi channel, we still only forward
        //if we're not looping or not on a tracked channel. 
        //So what we'll do is always create a note 
        //note this if check is redundant with the one above. 
        if (type == midi::NoteOn || type == midi::NoteOff) {
          byte note = MIDIrx.getData1();
          byte velocity = MIDIrx.getData2();

          // Serial.print("Received MIDI ");
          // Serial.print((type == midi::NoteOn) ? "NoteOn" : "NoteOff");  
          // Serial.print(" - Ch:");
          // Serial.print(channel);
          // Serial.print("N: ");
          // Serial.println(note);
          //if we're not tracking that channel, we just forward the note.
          //note this bypasses noteForward.
    if(!checkIfMIDIOn(channel)){
      if(type==midi::NoteOff){
        // Serial.print("Forwarding untracked NoteOff: note#");
        MIDItx.sendNoteOff(note, velocity, channel);
      }
      else if(type==midi::NoteOn){
        // Serial.print("Forwarding untracked NoteOn: note#");
        MIDItx.sendNoteOn(note, velocity, channel);
      }
      
       }
    else{
          //if maybeDropNote is true, then we roll a die. If it's above menu.randomDropProb, we return here (drop the note)
          //note we don't do anything with the note offs, which either never causes an issue or rarely does, not sure which.
          if(maybeDropNote && type==midi::NoteOn){
            //roll a die
            if(randomProbResult(menu.randomDropProb)){
              continue;
            }
          }
          
            
                   
          //create a new midiEvent
          MidiEvent newEvent = MidiEvent();
          newEvent.type = type;
          newEvent.channel = channel;
          newEvent.note = note;


#ifdef DEBUG
            if (note > 127) {
              Serial.print(F("READ OUT OF BOUNDS NOTE:"));
              Serial.print(note);
              Serial.print(F(" Loop State:"));
              Serial.print(isLooping);
              Serial.print(F(" Channel: "));
              Serial.println(channel);
            }
#endif
            //debug
            // Serial.print("BEFORE PROCESSING: Note:");
            // Serial.println(note);
            //end debug
          newEvent.velocity = velocity;
          newEvent.playTime = millis();
          newEvent.played = false;
          newEvent.pulseNumber = currentPulse;


          //if we're not droopping a note, we may delay it.  This happens before any jittering or other processing.
          if(randomProbResult(menu.delayNoteProb)){
            //if it's a note on, we delay it and make note of that
            if(type==midi::NoteOn){
              unsigned long delay = random(menu.minDelayTime, menu.maxDelayTime);
              newEvent.playTime+=delay;
              delayedNotesBuffer.push(newEvent);
              DelayedNoteOn dno = DelayedNoteOn(newEvent.note, newEvent.playTime, delay);
         
              delayedOnNotes[channel-1][numDelayedOnNotes[channel-1]++] = dno;
     
              newEvent.print();
    
              continue;
            }
          }
          if (type==midi::NoteOff && menu.delayNoteProb>0){
           
              DelayedNoteOn dno = checkForDelayedOn(channel, note);
             
              if(dno.note!=255){
               
                newEvent.playTime += dno.delayTime;
                delayedNotesBuffer.push(newEvent);

              newEvent.print();
              continue;
              }
              
            }


          

          //if it's a drum machine, maybe learn the new note/instrument
          
          // todo: combine the jitter on check?
          if(type==midi::NoteOn && menu.drumMIDIenabled[channel-1] && jitterOn){
            //maybe learn the instrument
            maybeLearnInstrument(channel, note);
            //now maybe drum jitter the note
            newEvent = maybeDrumJitter(newEvent);
          }

          //we can probably consolidate some of these conditionals at some point.
          //maybe jitter the note
          //we adjust for both note on and note off in case later glitches care about note off (though that is unlikely).
          if(jitterOn && menu.synthMIDIenabled[channel-1]){
          newEvent = maybeNoteNumberJitter(newEvent);  
          //note the jittered note does get sent to the buffer -- change from before, which jittered within the buffer. (so now if we jitter 46->48, we stutter the 48 each time)
          }

          //if  we're on a tracked channel, add to buffer.  Two ifs because I expect to come back in here and add other sblocks
          
          
          if((type == midi::NoteOn) || (type == midi::NoteOff && checkForNoteOn(newEvent.note))){
              //check if buffer is full now
               if ((eventsBuffer.size()==MAX_EVENTS) && !isBlinking) {
                Serial.println(F("Buffer full!"));
              startBufferFullBlink();
                }
              //push the new event.
              // Serial.print("Pushing event to buffer-- Note:");
              // Serial.print(newEvent.note);
              // Serial.print(" , Ch:");
              // Serial.print(newEvent.channel);
              // Serial.print(" ,v:");
              // Serial.println(newEvent.velocity);
              eventsBuffer.push(newEvent);

              if(retriggerOn && menu.drumMIDIenabled[channel-1] || (menu.synthMIDIenabled[channel-1] && menu.retriggerSynths)){
              //retrigger cue logic -- works on both synth and drum currently
              if(randomProbResult(menu.retriggerProb)){
                  // Serial.println("Cued a retrigger note");
                  cueRetriggeredNote(newEvent);
              }
              }
              
          }

          //apply velocity coercion to both loop and unlooped note
          if(velocityCoercion){
            
            //divide by 6 to get which sixteenth note we're on.
            newEvent.velocity = velocityCoerceValues[velocityCoercionIdx][currentPulseInBar/6];
          }
          
         
          
          //if we're not looping OR we're on an untracked channel-- the contents of this block, pass through the note 

          //now if we're not looping, we forward the note. We might delay it here.
      if(!isLooping){
          // Serial.println("Not delaying note");
        //forward the note
        forwardNote(newEvent);
      }

        }

      }//end this-is-a-note logic
        else{//start of this-is-not-a-note logic
          //this print is okay because it should never happen (if it does we're probably overloaded anyway).
          Serial.print("Got non-note MIDI type ");
          Serial.println(type);
        }
      
        
      }//end this-is-a-note logic 
      else if(type==midi::Stop){
        currentPulseInBar = 0;
        midiPanic();
        MIDItx.sendRealTime(midi::Stop);
      }
      else if(type==midi::Start||type==midi::Continue){
        currentPulseInBar = 0;
        MIDItx.sendRealTime((midi::MidiType)type);
      }
      else{
        Serial.print("Got unexpected note type: ");
        Serial.println((midi::MidiType)type);
      }
  }  ///end new read-midi -- that's a while loop, hence no else{} here
//end midi processing
  
//general stuff we'll use that gets done only per-clock-pulse.
  if(haveSeenClock){
    
    //roll for a possible pitchbend
  if(menu.pitchbendProb>0){
    //maybe create a new pitchbender
    if(randomProbResult(menu.pitchbendProb, 100000)){
      byte bendChannel = getRandomActiveChannel(menu.synthMIDIenabled);
      if(bendChannel!=255){
      pushBend(bendChannel);
      }
      // Serial.println(bendChannel);
    }
    updateBends();
    }

    haveSeenClock = false;
  }

  //play any delayed notes that are due (really, first one only)
  playDelayedNotes();

  // Check if restore defaults was requested from menu (after menu handling)
  if (menu.readyToRestoreDefaults) {
    restoreDefaults();
    menu.readyToRestoreDefaults = false;
  }
  if(menu.readyToDisableAll) {
    disableAll();
    menu.readyToDisableAll = false;
  }
  
  //start new only-if-no-midi logic

  //check the keypad
  static unsigned long lastKeypadUpdate = 0;
  if (millis() - lastKeypadUpdate > 50) {
    keypad.update();
    lastKeypadUpdate = millis();
  }

  //first, check for shortcut buttons
  if (keypad.lastKeyPressed == 'A') {
    Serial.println("Pressed A");
    reverseMidiBuffer(stutterBuffer);
    keypad.lastKeyPressed = 0;
  }
  if (keypad.lastKeyPressed == 'D'){
    menu.currentMenu = MAIN_MENU;
    menu.render();
    keypad.lastKeyPressed = 0;
  }
  // Handle keypad input for menus using a switch statement
  if (keypad.lastKeyPressed) {
    switch (menu.currentMenu) {
      case NOTE_JITTER_PROB_MENU:
        menu.handleJitterKeypad(keypad.lastKeyPressed);
        menu.render();
        keypad.lastKeyPressed = 0;
        break;
      case DRUM_JITTER_PROB_MENU:
        menu.handleDrumJitterKeypad(keypad.lastKeyPressed);
        menu.render();
        keypad.lastKeyPressed = 0;
        break;
      case RETRIGGER_PROB_MENU:
        menu.handleRetriggerKeypad(keypad.lastKeyPressed);
        menu.render();
        keypad.lastKeyPressed = 0;
        break;
      case STUTTER_TEMPERATURE_MENU:
        menu.handleStutterTemperatureKeypad(keypad.lastKeyPressed);
        menu.render();
        keypad.lastKeyPressed = 0;
        break;
      case RANDOM_DROP_PROB_MENU:
        menu.handleRandomDropProbKeypad(keypad.lastKeyPressed);
        menu.render();
        keypad.lastKeyPressed = 0;
        break;
      case DELAY_NOTE_PROB_MENU:
        menu.handleDelayNoteProbKeypad(keypad.lastKeyPressed);
        menu.render();
        keypad.lastKeyPressed = 0;
        break;
      case PITCHBEND_PROB_MENU:
        menu.handlePitchbendProbKeypad(keypad.lastKeyPressed);
        menu.render();
        keypad.lastKeyPressed = 0;
        break;
      // Add more cases for future menus here
      default:
        break;
    }
  }

  //debug -- test menu buttons
  if(menuDownButton.update()){
    menu.handleInput(BUTTON_DOWN);
    menu.render();
    Serial.println(F("Menu Down button pressed"));
  }
  if(menuUpButton.update()){
    menu.handleInput(BUTTON_UP);
    menu.render();
    Serial.println(F("Menu Up button pressed"));
  }
  if(menuLeftButton.update()){
    menu.handleInput(BUTTON_LEFT);
    menu.render();
    Serial.println(F("Menu Left button pressed"));
  }
  if(menuRightButton.update()){
    menu.handleInput(BUTTON_RIGHT);
    menu.render();
    Serial.println(F("Menu Right button pressed"));
  }

  if(menuSelectButton.update()){
    menu.handleInput(BUTTON_SELECT);
    menu.render();
    Serial.println(F("Menu Select button pressed"));
  }
  //end debug

  //////Button & switch reads
  if(retriggerSwitch.update()){
    Serial.println(F("Retrigger switch changed!"));
  };
  retriggerOn = retriggerSwitch.isOn();
  if(jitterSwitch.update()){
    Serial.println(F("Jitter switch changed!"));
  };
  jitterOn = jitterSwitch.isOn();

if(velocityCoercionSwitch.update()){
    // Serial.println(F("Velocity Coercion switch changed!"));
    if(velocityCoercionSwitch.isOn()){
    velocityCoercionIdx= (velocityCoercionIdx+1)%(sizeof(velocityCoerceValues)/sizeof(velocityCoerceValues[0]));
    // Serial.print("Coercion index is now");
    Serial.println(velocityCoercionIdx);
    // Start temporary view
    tempViewCallback = showVelocityCoercionLabel;
    tempViewStartTime = millis();
    tempViewActive = true;

    tempViewCallback(); 
    }
  };
  velocityCoercion = velocityCoercionSwitch.isOn();


  stutterButtonPressed = readStutterButton();
  

  if(menu.redrawSDMatrix){
    drawSDMatrix(menu.drumMIDIenabled, menu.synthMIDIenabled);
    menu.redrawSDMatrix = false;
  }

  // Handle menu-driven reset to default for channel config
  if(menu.pendingChannelDefaultsReset) {
    Serial.println(F("Resetting channel config to defaults!"));
    // Drum defaults: false, false, false, false, false, false, false, false, false, true, true, false, false, false, false, false
    bool drumDefaults[16] = {false, false, false, false, false, false, false, false, true, true, true, false, false, false, false, false};
    bool synthDefaults[16] = {true, true, true, true, true, true, true, true, false, false, false, true, true, true, true, true};
    drumState = 0;
    synthState = 0;
    for (int i = 0; i < 16; i++) {
      menu.drumMIDIenabled[i] = drumDefaults[i];
      menu.synthMIDIenabled[i] = synthDefaults[i];
      if (drumDefaults[i]) drumState |= (1 << i);
      if (synthDefaults[i]) synthState |= (1 << i);
    }
    // Save to EEPROM
    EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
    EEPROM.put(EEPROM_ADDR_DRUM_STATE, drumState);
    EEPROM.put(EEPROM_ADDR_SYNTH_STATE, synthState);
    Serial.println(F("Channel config defaults saved to EEPROM."));
    drawSDMatrix(menu.drumMIDIenabled, menu.synthMIDIenabled);
    menu.pendingChannelDefaultsReset = false;
  }

// read switches and build new state
  //get pot value
  stretchPotValueLiteral = analogRead(PIN_STRETCH_POT);

  stretchPotValueFloat = 0.1 + 3.9*(stretchPotValueLiteral/1023.0);
//smoothing
  stretchValue =  0.95 * stretchValue + 0.05 * stretchPotValueFloat;

  if(millis()-lastTimeStretchPotUpdated>MIN_STRETCH_INTERVAL_UPDATE){
   if(fabs(oldStretchValue - stretchValue)>0.05){
    // Serial.println(stretchValue);
    oldStretchValue = stretchValue;
    lastTimeStretchPotUpdated = millis();

    
  drawStretchDisplay();
  // Show stretch value on status display for TEMP_DISPLAY_TIME ms
  tempViewCallback = drawStretchStatusDisplay;
  tempViewStartTime = millis();
  tempViewActive = true;
  tempViewCallback();


   }
  }

 

  ///////Panic button logic
if (panicButton.update()) {
    midiPanic();
#ifdef DEBUG
    Serial.println(F("Panic!"));
    Serial.println("Attempting to recover tft...");
    SPI.end();            // tear down SPI
    delay(10);
    SPI.begin();          // reinit SPI
    menuTft.init(TFT_INIT_HEIGHT, TFT_INIT_WIDTH);   // TFT dimensions from SystemConfig.h
    menuTft.setRotation(3);   // landscape
    menuTft.fillScreen(ST77XX_BLACK);

    menuTft.setTextColor(ST77XX_CYAN);
    menuTft.setCursor(10, 10);
    menuTft.setTextSize(2);
    menuTft.print(F("MIDI Glitcher"));
    menuTft.setCursor(15,25);
    menuTft.setTextColor(ST77XX_MAGENTA);
    menuTft.print(F("MIDI Glitcher"));
    menuTft.setCursor(20,40);
    menuTft.setTextColor(ST77XX_YELLOW);
    menuTft.print(F("MIDI Glitcher"));
    //print "press select to start" in bottom right corner
    menuTft.setCursor(135-24, 135-16);
    menuTft.setTextColor(ST77XX_WHITE);
    menuTft.setTextSize(1);
    menuTft.print(F("Press Select to Start"));
#endif
    // Start temporary view
    tempViewCallback = showPanicDisplay;
    tempViewStartTime = millis();
    tempViewActive = true;

    tempViewCallback();  // draw it immediately


}


  // if(playButton.update()){
  //   //first press may not work, MIDIPlayState may be misleading
  //   Serial.println("play button pressed!");
  //   if(MIDIPlayState){
  //     // MIDI.sendRealTime(midi::Stop);
  //     Serial.println("Stop!");
  //     MIDI.sendStop();
  //   }
  //   else{
  //     // MIDI.sendRealTime(midi::Start);
  //     Serial.println("start!");
  //     MIDI.sendStart();
  //   }
  // MIDIPlayState = !MIDIPlayState;
  // }

  ///check if pulse resolution has changed and respond appropriately. If the new resolution is bigger, we have no problem. If it's smaller, we clear out the bad notes & start times.
  if (menu.pulseResolution < oldPulseResolution) {
    //clear out extra notes
    for (unsigned int i = 1; i < menu.pulseResolution + 1; i++) {
      int removalIndex = (currentPulse + i) % oldPulseResolution;
      clearOldNotes(removalIndex);
    }
    // //clear out extra start times
    // Serial.print("On pulse#");
    // Serial.println(currentPulse);
    for (unsigned int i = 0; i < oldPulseResolution - menu.pulseResolution; i++) {
    //   Serial.print("Removing the");
    //   Serial.print(i);
    //   Serial.print("nth pulse, now have");
      pulseStartTimes.pop(dummyTime);
    //   Serial.println(pulseStartTimes.size());

    }
  }

  ///stutter button is pressed
  if (stutterButtonPressed && !prevStutterPressed) {
    Serial.println("Stutter button pressed!");
    //clear the stutter buffer
    clearStutterBuffer();
    clearStutterPulseTimes();
    //dump the events buffer into the stutter buffer (don't empty the events buffer)
    for(unsigned int i =0; i<eventsBuffer.size(); i++){
      MidiEvent e = eventsBuffer[i];
      stutterBuffer.push(e);
    }

    digitalWrite(PIN_BUFFER_FULL_LED,LOW);
    //
    //kill existing notes on tracked channels
    killTrackedChannelsNotes();
    loopStartTime = millis();
    //debug
    // Serial.print("set loopStartTime to:");
    // Serial.println(loopStartTime);
    isLooping = true;
    
    //copy events buffer to stutter buffer
    numSynthNoteOnsInStutterBuffer = 0;
    for(unsigned int i =0; i<stutterBuffer.size(); i++){
      if(stutterBuffer[i].type==midi::NoteOn && menu.synthMIDIenabled[stutterBuffer[i].channel-1]){
        numSynthNoteOnsInStutterBuffer++;
      }
    }
    //copy pulse start times to stutter pulse start times
    for(unsigned int i =0; i<pulseStartTimes.size(); i++){
      unsigned long t = pulseStartTimes[i];
      stutterPulseStartTimes.push(t);
    }
  }

  ///stutter button is released
  if (!stutterButtonPressed && prevStutterPressed) {
    isLooping = false;
    killTrackedChannelsNotes();
  }

  //check if button state has changed at all
  if (stutterButtonPressed != prevStutterPressed) {
    lastButtonChangeTime = millis();
    // #ifdef DEBUG
    // Serial.print("# pulses in pulseStartTimes:");
    // Serial.print(pulseStartTimes.size());
    // Serial.print(", pulse resolution: ");
    // Serial.println(pulseResolution);
    // #endif
  }

////////////////////////End button-switch stuff////////////////////////

//pre-note loop logic
if(isLooping){}
//pre-note nonloop logic
else{
  // check if pulseREsolution buffer is full
  checkPulseBufferFullSetLED();
  updateBufferFullBlink();
}


  //stutter behavior. This is a first attempt, we still need to quantize the playback.
  //for now we'll just read the final pulse up until the time the button was pressed. Later we'll keep it reading until it hits the next quarter note or whatever.
  //quantization may not be as important since we actually track silence.

  if (isLooping) {//pure is looping logic (not part of midi read)
    //if we don't have pulseResolution's worth of pulse, we don't do playback
  //I don't think I need this flag logic, I think I *can* just return out of here.  
  bool validLoopFlag = true;

  //stil require we have a full buffer, but that should almost always happen because the dumping happens before this block of code
  if (stutterPulseStartTimes.size() < menu.pulseResolution) {
    Serial.println(F("Insufficient number of pulses!")); 
    validLoopFlag=false;
    isLooping=false;
  }

if(validLoopFlag){
  //starting the loop.
   if (!prevLooping) {
    //Mark the current time, the end of the stutter, this will make our stutter-loop duration a little less than pulseResolution*pulseDuration,
    //which is what it should be.  A TODO will be to calculate bpm and figure out how long the stutter should last.

    //we also set our Loop Start Time, we'll use this to determine when we've played through everything.
    loopStartTime = millis();
    // Serial.print(F("set loop start time to: "));
    // Serial.println(loopStartTime);


    //get the length of the playback for only the last 'pulseResolution' pulses
    playbackStartTime = stutterPulseStartTimes[0];
    //this is left as-is rather than just doing playbackLenght = loopStartTime - playbackStartTime in anticipation of the bpm-estimation pulse-ending logic
    playbackEndTime = loopStartTime;
    //moving playbackLength setting to inside the isLooping Loop, outside of !prevLooping guard, so that it responds to movements of the stretch knob
    }
    playbackLength = stretchValue*(playbackEndTime - playbackStartTime);
    // Serial.print("CurrentPulse: ");
    // Serial.print(currentPulse);
    // Serial.print(" OldestPulse: ");
    // Serial.print(eventsBuffer[0].pulseNumber);
    // int debugDiff = (currentPulse + pulseResolution - eventsBuffer[0].pulseNumber) % pulseResolution;
    // Serial.print(" Cicular diff: ");
    // Serial.println(debugDiff);
    // // debugCounter[debugDiff]++;
    // Serial.print("  Time Diff: ");
    // Serial.println(playbackLength);
    // debugBuffer[debugDiff].push(playbackLength);


    playSavedPulses();
    //if we've played through a whole loop, reset everything and kill active notes
    if (millis() - loopStartTime > playbackLength) {

      loopStartTime = millis();
      for (unsigned int i = 0; i < stutterBuffer.size(); i++) {
        // Iterate through all events in this pulse.  Note this resets ALL, not just the ones that are active due to pulseResolution
        stutterBuffer[i].played = false;
      }
      //kill all midi Notes on tracked channels at the end of the loop (we may not have the note-offs for all our note-ons, so this is necessary)
      killTrackedChannelsNotes();
      //clear out the perc'd notes
      clearAllPercolatedNotes();
    }
  }
  }//end isLooping logic
  //after-note pure !isLooping logic
  else{
    if(retriggerOn){playRetriggeredNotes();}
  }
//end of pure !isLoopingLogic

//debug
if(logButton.update()){
  // Serial.print(F("Current bpm: "));
  // Serial.println(tempoTracker.bpm);
  //if we're loopoing, print out the notes and channels of everything in the buffer
  if(isLooping){
    Serial.print("There are ");
    Serial.print(stutterBuffer.size());
    Serial.println(" events in the stutter buffer:");
    
  }
  // for(unsigned int i =0; i<stutterBuffer.size(); i++){
  //   MidiEvent e = stutterBuffer[i];
  //   e.print();
  // }
  //print out contents of delayedOnNotes
  for(int ch=0; ch<16; ch++){
    if(numDelayedOnNotes[ch]>0){
      for(int n=0; n<numDelayedOnNotes[ch]; n++){
        DelayedNoteOn dno = delayedOnNotes[ch][n];
      }
    }
  }

  // Serial.print("debug serial count max: ");
  // Serial.println(debugSerialCount);
  // debugSerialCount = 0;
}
  
  prevStutterPressed = stutterButtonPressed;
  prevLooping = isLooping;
  oldPulseResolution = menu.pulseResolution;

  if((menu.pitchbendProb>0)||(pitchbendBuffer.size()>0)){
  pruneBends();
  }
  
  //update temporary view
  if (tempViewActive && millis() - tempViewStartTime > TEMP_DISPLAY_TIME) {
    tempViewActive = false;
    drawSDMatrix(menu.drumMIDIenabled, menu.synthMIDIenabled);  // restore SD matrix view
}

}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////// OTHER FUNCTIONS ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

void clearStutterBuffer(){
  stutterBuffer.clear();
  numSynthNoteOnsInStutterBuffer = 0;
}
void clearStutterPulseTimes(){
  stutterPulseStartTimes.clear();
}

//I'm pretty sure it's okay for this to block.
void playSavedPulses() {
    // Serial.print("Playing saved pulses, size:");
    // Serial.println(eventsBuffer.size());

  // Iterate through all saved pulses
  for (unsigned int i = 0; i < stutterBuffer.size(); i++) {
    // Iterate through all events

    MidiEvent& event = stutterBuffer[i];  // Use reference to original
    //todo: maybe fix this?s
    // Serial.print("first pulse in events buffer:");
    // Serial.print(eventsBuffer[0].pulseNumber);
    // Serial.print(" sv: ");
    // Serial.print(stretchValue);
    // Serial.print(" ept: ");
    // Serial.print(event.playTime);
    // Serial.print(" pst: ");
    // Serial.print(playbackStartTime);
    unsigned long relative_note_time = stretchValue*(event.playTime - playbackStartTime);
    // Serial.print(" rnt: ");
    // Serial.print(relative_note_time);
    unsigned long current_relative_time = millis() - loopStartTime;
    // Serial.print(" crt: ");
    // Serial.print(current_relative_time);
    // Serial.print("playback start time - loop start time: ");
    // Serial.println(loopStartTime - playbackStartTime);
    
    

    // Now we're checking the ORIGINAL event's played status -- though I'm not convinced this matters
    if (!event.played && current_relative_time >= relative_note_time) {

      stutterBuffer[i].played = true;

#ifdef ACTIVE_NOTES_DEBUG
      if (!activeNotes[event.note]) {
        Serial.print(F("Saw note#"));
        Serial.print(event.note);
        Serial.print(F(" channel#"));
        Serial.println(event.channel);
        activeNotes[event.note] = true;
      }
#endif

      // Serial.print(F("playing buffer event-- Note:"));
      // Serial.print(event.note);
      // Serial.print(F(" , Ch:"));
      // Serial.print(event.channel);
      // Serial.print(F(" ,v:"));
      // Serial.println(event.velocity);

      //if STUTTER_TEMPERATURE>0, we maybe percolate the note
      MidiEvent maybeNewEvent = event;
      if(menu.stutterTemperature>0){
        maybeNewEvent = maybePercolateNote(event, i);
      }

      forwardNote(maybeNewEvent);
    }
  }
}


bool readStutterButton() {
  static bool lastButtonState = HIGH;
  static bool buttonState = HIGH;
  static unsigned long lastDebounceTime = 0;

  bool reading = digitalRead(PIN_STUTTER_SWITCH);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    if (reading != buttonState) {
      buttonState = reading;
    }
  }


  lastButtonState = reading;

  return buttonState == LOW;
}




void clearOldNotes(int expiredPulse) {
  // Serial.print("Clearing out pulse #");
  // Serial.println(expiredPulse);
  eventsBuffer.removeIf([&](const MidiEvent& e) {
    return e.pulseNumber == expiredPulse;  // use the pulse you actually want to clear
  });
}

MidiEvent createEmptyEvent(byte pulseNumber) {
  MidiEvent emptyEvent = MidiEvent();
  emptyEvent.type = MIDI_NOOP;
  emptyEvent.channel = 0;
  emptyEvent.note = 0;
  emptyEvent.velocity = 0;
  emptyEvent.playTime = millis();
  emptyEvent.played = false;
  emptyEvent.pulseNumber = pulseNumber;
  return emptyEvent;
}


void manglerHandleClock() {
  currentPulse = (currentPulse + 1) % menu.pulseResolution;
  // Serial.println(currentPulse);
  clearOldNotes(currentPulse);
  unsigned long pulseTime = millis();
  //once the buffer is full, remove the oldest
  if(pulseStartTimes.size()>=menu.pulseResolution){
  pulseStartTimes.pop(dummyTime);
  }
  //add the new pulse start time
  pulseStartTimes.push(pulseTime);
  // tempoTracker.addPulse(pulseTime);
  // tempoTracker.calculateBPM();

  currentPulseInBar = (currentPulseInBar + 1) % 96;
  
}


//this SHOULD use eventsBuffer.
bool checkForNoteOn(byte noteOffNumber) {
  for (int i = eventsBuffer.size() - 1; i >= 0; --i) {
    const MidiEvent& ev = eventsBuffer[i];
    if (ev.note == noteOffNumber) {
      return (ev.type == midi::NoteOn);
    }
  }
  // note never appeared
  return false;
}

//BUFFER LED stuff
void checkPulseBufferFullSetLED(){
  if(isBlinking){return;}
  if (pulseStartTimes.size() >= menu.pulseResolution) {
    digitalWrite(PIN_BUFFER_FULL_LED, HIGH);
  }
  else{
    digitalWrite(PIN_BUFFER_FULL_LED,LOW);
  
  }
}

/// Call this frequently in loop() to pulse LED
void updateBufferFullBlink() {
  unsigned long now = millis();
  
  if (!isBlinking) return;
  if (now - blinkStartTime >= BUFFER_FULL_LED_BLINK_TIME) {
    blinkStep++;

    if (blinkStep >= blinkPatternLength) {
      // Done with current blink sequence
      isBlinking = false;
      blinkStep = 0;
      return;
    }

    // Determine pattern based on starting LED state
    bool ledWasOn = digitalRead(PIN_BUFFER_FULL_LED);
    bool ledState;

    if (ledWasOn) {
      // LED was on: pulse OFF-ON-OFF-ON-OFF
      ledState = (blinkStep % 2 == 0) ? LOW : HIGH;
    } else {
      // LED was off: pulse ON-OFF-ON-OFF-ON
      ledState = (blinkStep % 2 == 0) ? HIGH : LOW;
    }

    digitalWrite(PIN_BUFFER_FULL_LED, ledState);
    blinkStartTime = now;
  }
}
// Call this to start a new pulse sequence
void startBufferFullBlink() {
  Serial.println(F("starting buffer full blink"));
  isBlinking = true;
  blinkStep = 0;
  blinkStartTime = millis();
}

bool maybeLearnDrumMachine(byte channel){
  //first, check if this is even a drum machine
  if(!menu.drumMIDIenabled[channel-1]){
    return false;
  }
  // then, check if this channel is already learned
  for(int i = 0; i < numDrumMachines; i++) {
    if (drumMachines[i].channel == channel) {
      // Already learned
      return false;
    }
  }
  // Not found, try to add if space
  if(numDrumMachines >= MAX_DRUM_MACHINES) {
    Serial.println(F("Max drum machines learned, can't learn more"));
    return false;
  }
  DrumMachine newMachine = DrumMachine(channel);
  drumMachines[numDrumMachines] = newMachine;
  numDrumMachines++;
  Serial.print("Learned new drum machine on channel ");
  Serial.println(channel);
  return true;
}

bool maybeLearnInstrument(byte channel, byte notenumber){
  if(!menu.drumMIDIenabled[channel-1]){
    return false;
  }
  // Find or create the drum machine for this channel
  DrumMachine* drumMachine = nullptr;
  for(int i = 0; i < numDrumMachines; i++) {
    if (drumMachines[i].channel == channel) {
      drumMachine = &drumMachines[i];
      break;
    }
  }
  if (!drumMachine) {
    // Try to create a new drum machine
    if (!maybeLearnDrumMachine(channel)) {
      Serial.print("Tried to learn instrument on channel ");
      Serial.print(channel);
      Serial.println(" but couldn't");
      return false;
    }
    drumMachine = &drumMachines[numDrumMachines-1];
  }
  // Check if the instrument is already learned
  for(int i = 0; i < drumMachine->numInstruments; i++) {
    if(drumMachine->instrumentsLearned[i] == notenumber) {
      // Already learned
      return false;
    }
  }
  // Add the instrument if there is space
  if(drumMachine->numInstruments < MAX_INSTRUMENTS_PER_DRUM_MACHINE) {
    drumMachine->instrumentsLearned[drumMachine->numInstruments] = notenumber;
    drumMachine->numInstruments++;
    Serial.print("Learned new instrument ");
    Serial.print(notenumber);
    Serial.print(" on drum machine channel ");
    Serial.println(channel);
    return true;
  } else {
    Serial.print("Tried to learn instrument ");
    Serial.print(notenumber);
    Serial.print(" on channel ");
    Serial.print(channel);
    Serial.println(" but couldn't");
    return false;
  }
}

void refreshDrumMachines() {
  //first, we remove all non-drum machines.  Then we try to learn all the drum machines in order
  for(int i = 0; i < numDrumMachines; i++) {
    if (!menu.drumMIDIenabled[drumMachines[i].channel - 1]) {
      // This channel is no longer a drum machine, remove it
      Serial.print("Forgetting drum machine on channel ");
      Serial.println(drumMachines[i].channel);
      // Shift remaining machines down
      for(int j = i; j < numDrumMachines - 1; j++) {
        drumMachines[j] = drumMachines[j + 1];
      }
      numDrumMachines--;
      i--; // Check this index again as it now has a new machine
    }
  }
}

///////Glitching functions (non-stutter)

//pitchbend functions
void pushBend(int channel){
  //if we're already bending on this channel, we exit
  for(unsigned int i = 0; i<pitchbendBuffer.size(); i++){
    if(channel == pitchbendBuffer[i].channel){
      return;
    }
  }
  //otherwise, create a new pushbend object and stick it on
  Serial.print(F(">>>>>>>>> creating bend on channel "));
  Serial.println(channel);
  PitchBender bend = PitchBender(channel);
  //if we're pushing off an old pitchbend, make sure we turn it off
  if(pitchbendBuffer.size()==NUM_ACTIVE_PITCHBENDS){
    pitchbendBuffer[0].finishBend();
  }
  pitchbendBuffer.push(bend);
}
void updateBends(){
  for(unsigned int i = 0; i<pitchbendBuffer.size();i++){
    pitchbendBuffer[i].update();
  }
}

void pruneBends(){
    pitchbendBuffer.removeIf([](PitchBender& pitchBend){
    if (pitchBend.numBendPasses==0) {
        pitchBend.finishBend();
        return true;
    }
    return false;
});
}

//Jitter functions

// --- Add a jittered note ---
bool addJitteredNote(byte oldNote, byte newNote, byte channel) {
    if (jitterCount >= JITTER_BUFFER_SIZE) {
        // buffer full, handle warning elsewhere (LED blink)
        return false;
    }
    jitterBuffer[jitterCount++] = {oldNote, newNote, channel};
    return true;
}

// --- Remove a note matching oldNote & channel. Send the corresponding note off for any found. ---
bool removeJitteredNote(byte oldNote, byte channel) {
    bool removed = false;
    for (int i = 0; i < jitterCount; i++) {
        if (jitterBuffer[i].originalNote == oldNote &&
            jitterBuffer[i].channel == channel) {
            MIDItx.sendNoteOff(jitterBuffer[i].newNote, 0, channel);
            jitterBuffer[i] = jitterBuffer[jitterCount - 1];
            jitterCount--;
            i--; // check swapped element
            removed = true;
        }
    }
    return removed;
}


// --- Find newNote(s) for a given oldNote & channel ---
//not going to use this function I think.  Leaving it for now so I can copy its logic
void getJitterNewNotes(byte oldNote, byte channel, byte outNotes[], int &outCount) {
    outCount = 0;
    for (int i = 0; i < jitterCount; i++) {
        if (jitterBuffer[i].originalNote == oldNote &&
            jitterBuffer[i].channel == channel) {
            outNotes[outCount++] = jitterBuffer[i].newNote;
        }
    }
}
MidiEvent maybeNoteNumberJitter(MidiEvent event) {
    // produce a NEW MIDI note
    MidiEvent newEvent;
    //if it's not a note event we don't want it.  
    if(!((event.type==midi::NoteOn) || (event.type==midi::NoteOff))){return event;}
    //if it is a note event...
    newEvent.channel = event.channel;
    newEvent.velocity = event.velocity;
    newEvent.playTime = event.playTime;
    newEvent.played = event.played;
    newEvent.pulseNumber = event.pulseNumber;

    //first, we turn off & remove past jittered notes
    bool removedANote = removeJitteredNote(event.note, event.channel);
    if(removedANote && event.type==midi::NoteOff){
      //if we removed a note off, we'll just pass on a NO_OP midi event. The other fields don't really matter.
      //This is to avoid sending an off for a note we jittered TO. 
      newEvent.type = MIDI_NOOP;
      return newEvent;
    }


    //then we start constructing our new event (which may be identical to event if we end up not jittering)
    newEvent.type = event.type;

    //get the current note
    byte current_note = event.note;

    //only jitter NoteOn events
    if(event.type==midi::NoteOn){
    // roll a die
  if (randomProbResult(menu.noteJitterProb)) {
        // Serial.print(F("jittering note on channel "));
        // Serial.println(event.channel);
        byte jitter = pickRandomElement(menu.currentOffsetSet->offsets, menu.currentOffsetSet->size);
        int8_t plus_or_minus = randomProbResult(50) ? 1 : -1;  // ternary operator
        int8_t octave = randomOctave();
        byte jittered_note = (int)current_note + (jitter + plus_or_minus*12*octave);
        //get into the right range
        
        // wrap down if >127
        while (jittered_note > 127) jittered_note -= 12;

        // wrap up if <0
        while (jittered_note < 0) jittered_note += 12;
        
        addJitteredNote(current_note, jittered_note, event.channel); // track old -> new
        current_note = jittered_note;
        
    }
    }

    newEvent.note = current_note;
    return newEvent;
}


// --- Add a jittered note ---
bool addJitteredDrumNote(byte oldNote, byte newNote, byte channel) {
    if (drumJitterCount >= DRUM_JITTER_BUFFER_SIZE) {
        // buffer full, handle warning elsewhere (LED blink)
        return false;
    }
    drumJitterBuffer[drumJitterCount++] = {oldNote, newNote, channel};
    return true;
}

// --- Remove a note matching oldNote & channel. Send the corresponding note off for any found. ---
bool removeJitteredDrumNote(byte oldNote, byte channel) {
    bool removedANote = false;
    for (int i = 0; i < drumJitterCount; i++) {
        if (drumJitterBuffer[i].originalNote == oldNote &&
            drumJitterBuffer[i].channel == channel) {
            // turn off the jittered note
            MIDItx.sendNoteOff(drumJitterBuffer[i].newNote, 0, channel);
            // swap with last element instead of shifting
            drumJitterBuffer[i] = drumJitterBuffer[drumJitterCount - 1];
            drumJitterCount--;
            i--; // check the swapped element
            removedANote = true;
        }
    }
    return removedANote;
}


//this is a much simpler function the synth jittering because we won't worry 
//about sending off notes.  We just assume the drum machine doesn't care about those.
//todo: mayyyyyybe change this later. Won't be too bad to do so.
MidiEvent maybeDrumJitter(MidiEvent event){
    //first, we turn off & remove past jittered notes
    bool removedANote = removeJitteredNote(event.note, event.channel);
    if(removedANote && event.type==midi::NoteOff){
      //if we removed a note off, we'll just send a no-op event
       MidiEvent newEvent;
      newEvent.type = MIDI_NOOP;
      newEvent.channel = 0;
      return newEvent; 
    }
    //if it's some other non-note on-- a note off that we didn't remove a note for-- just return that. 
  if(!(event.type==midi::NoteOn)){return event;}
  //first, find the learned drum machine for this channel.  If it's not there, just return the original event.
  DrumMachine* drumMachine = nullptr;
  for(int i = 0; i < numDrumMachines; i++) {
    if (drumMachines[i].channel == event.channel) {
      drumMachine = &drumMachines[i];
      break;
    }
  }
  if (!drumMachine) {
    return event;
  }

  //micro optimization-- if there's only one instrument, we can't jitter
  if(drumMachine->numInstruments<2){
    return event;
  }
  //roll to see if we jitter -- we use the same probability as synth jittering
  if(!randomProbResult(menu.drumJitterProb)){
    return event;
  }
  //if we get here, we jitter.  We pick a random learned instrument from the drum machine. Like with
  //note jittering, this is allowed to be the same as the original note.
  byte newNote = pickRandomElement(drumMachine->instrumentsLearned, drumMachine->numInstruments);
  // Serial.print("New drum note: ");
  
  //debug
  // Serial.print(F("Jittering drum note on channel "));
  // Serial.print(event.channel);
  // Serial.print(F(" from "));
  // Serial.print(event.note);
  // Serial.print(F(" to "));
  // Serial.println(newNote);
  
  if(newNote!=event.note){
    event.note = newNote;
  }
  

  return event;
}

//end jitter functions

//retrigger functions
void cueRetriggeredNote(MidiEvent me){
  //if it's not a note-on, we're done
  // Serial.println("retriggered note cued");
  if(me.type!=midi::NoteOn){return;}
  MidiEvent rEvent;
  rEvent.type = me.type;
  rEvent.channel = me.channel;
  rEvent.note = me.note;
// #ifdef DEBUG
//             if (note > 127) {
//               Serial.print("READ OUT OF BOUNDS NOTE:");
//               Serial.print(note);
//               Serial.print(" Loop State:");
//               Serial.print(isLooping);
//               Serial.print(" Channel: ");
//               Serial.println(channel);
//             }
// #endif
          rEvent.velocity = me.velocity;
          //note this means that if it's a synth & drum, it uses the synth time. 
          int retriggerTime;
          if(menu.drumMIDIenabled[me.channel-1]){
             retriggerTime = RETRIGGER_DRUM_TIME;
          }
          else{
            retriggerTime = RETRIGGER_SYNTH_TIME;
          }
          rEvent.playTime = millis()+retriggerTime;
          
          // rEvent.played = false; //not used
          // rEvent.pulseNumber = currentPulse; //not used
          retriggerBuffer.push(rEvent);
          
          MidiEvent endEvent = createOffNote(rEvent, rEvent.playTime+RETRIGGER_NOTE_LENGTH);
          retriggerBuffer.push(endEvent);
    //start maybe adding more retriggered notes, up to 10.  If we hit a "no" at any point, we stop the process
    for(int i=0;i<10;i++){
      if(!randomProbResult(RERETRIGGER_PROB)){return;} //stop if we fail a roll
      //assuming we passed, we just create a new retrigger event, being sure to increse the time 
      long newEndTime =  rEvent.playTime  + (1+i)*retriggerTime;
      MidiEvent otherEvent;
      otherEvent.type = rEvent.type;
      otherEvent.channel = rEvent.channel;
      otherEvent.note = me.note;
      otherEvent.velocity = me.velocity;
      otherEvent.playTime = newEndTime;
      retriggerBuffer.push(otherEvent);
      MidiEvent otherEndEvent = createOffNote(otherEvent, newEndTime+RETRIGGER_NOTE_LENGTH);
      retriggerBuffer.push(otherEndEvent);

    }
    if (retriggerBuffer.size() == RETRIGGER_BUFFER_SIZE) {
      Serial.println(F("**********************************"));
      Serial.println(F("Retrigger buffer full!"));
      Serial.println(F("**********************************"));
    }

}

void playRetriggeredNotes(){
  retriggerBuffer.removeIf([](MidiEvent& event){
    if (event.playTime <= millis()) {
        forwardNote(event);
        return true;
    }
    return false;
});

}
//end retrigger functions


// --- Add a percolated note ---
bool addPercolatedNote(byte oldNote, byte newNote, byte channel) {
    if (percCount >= PERC_BUFFER_SIZE) {
        // buffer full, handle warning elsewhere (LED blink)
        return false;
    }
    percBuffer[percCount++] = {oldNote, newNote, channel};
    return true;
}

// --- Remove a note matching oldNote & channel. Send the corresponding note off for any found. ---
bool removePercolatedNote(byte oldNote, byte channel) {
  bool removedANote = false;
    for (int i = 0; i < percCount; i++) {
        if (percBuffer[i].originalNote == oldNote &&
            percBuffer[i].channel == channel) {
            //turn off the percolated note
            MIDItx.sendNoteOff(percBuffer[i].newNote, 0, channel);
            // shift all later elements down
            removedANote = true;
            for (int j = i; j < percCount - 1; j++) {
                percBuffer[j] = percBuffer[j + 1];
            }
            percCount--;
            i--; // check new element at this position
        }
    }
    return removedANote;
}

void clearAllPercolatedNotes() {
  //turn off all percolated notes -- this works in concert with killing all tracked channels, but covers when we change channel
  //mid stutter (not as easy now that that's done through the menu)
  for (int i = 0; i < percCount; i++) {
      MIDItx.sendNoteOff(percBuffer[i].newNote, 0, percBuffer[i].channel);
  }
    percCount = 0; // clear the buffer
}
//percolation of stutter
///basically, this takes a midi event and randomly selects one within (-STUTTER_TEMPERATURE, STUTTER_TEMPERATURE) events of it
//and maybe plays that note number instead.  This is ONLY done for note on events right now (I'm feeling lazy and memory shy),
//we'll rely on the end-of-loop midi offs to turn off the notes.
//upgrading to work on off notes as well now, basically copying how I do jittered notes.  Need to be careful though, since
//this one only fires during stutters-- I don't think that makes a difference, but be careful.  Need to think through
//how it interacts with end-of-loop note offs.  Maybe just clear them all at the end of each loop?
//maybe I'll instead create a random number of steps and then walk down a filtered buffer of only
//synth notes and/or only channel-sharing notes?

//this only fires when stuttering, so we use stutterbuffer instead of eventsbuffer
MidiEvent maybePercolateNote(MidiEvent event, byte index_number){
  //if we get a note-off, we want to turn off any percolated notes from the same original note
  /*bool removedANote = removeJitteredNote(event.note, event.channel);
    if(removedANote && event.type==midi::NoteOff){
      //if we removed a note off, we'll just pass on a NO_OP midi event. The other fields don't really matter.
      //This is to avoid sending an off for a note we jittered TO. 
      newEvent.type = MIDI_NOOP;
      return newEvent;
    }*/
  
    bool removedANote = removePercolatedNote(event.note, event.channel);
    //we return a midi noop event, since we don't want to send a note off for the original note
    if(removedANote&& event.type==midi::NoteOff){
    MidiEvent newEvent;
    newEvent.type = MIDI_NOOP;
    newEvent.channel = event.channel;
    newEvent.note = event.note;
    return newEvent;
  }

  //if a non-qualifying event, exit right away  Note this shouldn't get hit by noteoffs..
  if(event.type != midi::NoteOn || !menu.synthMIDIenabled[event.channel - 1]) {
    return event;}

  //only do this for note on events for now (TODO: eugh, going to have to change note offs)
  if(event.type == midi::NoteOn && menu.synthMIDIenabled[event.channel - 1]){
    int random_step_count = random(0, menu.stutterTemperature);
    //cap random_step_count to numSynthNoteOnsInStutterBuffer-1  . Use mod for more variatoin. 
    if(random_step_count >= numSynthNoteOnsInStutterBuffer) {
      random_step_count = random_step_count % numSynthNoteOnsInStutterBuffer;
    }
     //50/50 chance of going up or down
    int8_t direction;
    if(randomProbResult(50)){
      direction = 1;
    }else{direction = -1;}
    if(random_step_count==0){return event;} //no change
    byte steps_taken = 0;
    unsigned int working_index = index_number;
    byte newNote;
    //if we do a full loop, just return the original note, don't keep going. This should never happen.
    unsigned int maxSteps = stutterBuffer.size();
    while(steps_taken<random_step_count&&maxSteps--){
      //we walk through stutterBuffer in the direction, we only count synth events.
      working_index = (working_index + stutterBuffer.size() + direction) % stutterBuffer.size();
      if(menu.synthMIDIenabled[stutterBuffer[working_index].channel-1] && stutterBuffer[working_index].type==midi::NoteOn){
        steps_taken++;
    }
  }
  if (maxSteps == 0) {
  Serial.println(F("Warning: No additional synth events found, returning original note"));
  return event;
  }
    //microoptimization--if we ended up where we started, just return the original note
    if(working_index==index_number){return event;}
    newNote = stutterBuffer[working_index].note;
    //if we got the same note as before, don't create a new event, just return the original
    if(newNote==event.note){return event;}
    //otherwise, create a new event with the new note
    MidiEvent newEvent = event; //copy everything else
    newEvent.note = newNote;
    Serial.print(F("Percollating note "));
    Serial.print(event.note);
    Serial.print(F(" on channel "));
    Serial.print(event.channel);
    Serial.print(F(" to "));
    Serial.println(newNote);
    Serial.print(F(" on channel "));
    Serial.println(stutterBuffer[working_index].channel);
    //if we do have a new note, we need to track it so we can turn it off later
    addPercolatedNote(event.note, newNote, event.channel);
    return newEvent;
  }
  else{return event;}
}

//optimized check, swaps found note with last element and decrements count
DelayedNoteOn checkForDelayedOn(byte channel, byte noteOnNumber) {
    for (int i = 0; i < numDelayedOnNotes[channel-1]; i++) {
        if (delayedOnNotes[channel-1][i].note == noteOnNumber) {
            DelayedNoteOn found = delayedOnNotes[channel-1][i];
            // swap with last element instead of shifting
            delayedOnNotes[channel-1][i] = delayedOnNotes[channel-1][numDelayedOnNotes[channel-1]-1];
            numDelayedOnNotes[channel-1]--;
            return found;
        }
    }
    return DelayedNoteOn(255,0,0);
}
//play delayed notes
void playDelayedNotes(){
  //play all the notes that have play time <= millis(), max once per loop.
  if(delayedNotesBuffer.size()==0){return;}
  auto firstEvent = delayedNotesBuffer.peek();
  if(firstEvent->playTime <= millis()){
    forwardNote(*firstEvent);
    delayedNotesBuffer.pop(dummyEvent);
  }

}


//////Display functions

void setupStatusDisplay() {
  if (!statusDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Setup statusdisplay failed!"));
    for (;;); // halt if OLED init fails
  }
  statusDisplay.clearDisplay();
  statusDisplay.setTextSize(1);
  statusDisplay.setTextColor(SSD1306_WHITE);
}

//not currently used. No real memory-related display problems with the teensy.
void recoverStatusDisplay() {
    // statusDisplay.clearDisplay();
    // statusDisplay.display();       // push a blank screen
    // statusDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // full init
    // redrawDisplay();         // draw your current state
    statusDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // full init first
    statusDisplay.clearDisplay();                     // then clear
}



void drawSDMatrix(bool drumArr[16], bool synthArr[16]) {
  statusDisplay.clearDisplay();
  statusDisplay.setTextColor(SSD1306_WHITE);
  statusDisplay.setTextSize(1);
  statusDisplay.setCursor(0, 0);

  int cols = 4;
  int rows = 4;
  int cellW = SCREEN_WIDTH / cols;   // 32 px
  int cellH = SCREEN_HEIGHT / rows;  // 16 px

  // draw vertical grid lines
  for (int c = 1; c < cols; c++) {
    int x = c * cellW;
    statusDisplay.drawLine(x, 0, x, SCREEN_HEIGHT, SSD1306_WHITE);
  }

  // draw horizontal grid lines
  for (int r = 1; r < rows; r++) {
    int y = r * cellH;
    statusDisplay.drawLine(0, y, SCREEN_WIDTH, y, SSD1306_WHITE);
  }

  // draw contents
  for (int i = 0; i < 16; i++) {
    int row = i / cols;
    int col = i % cols;
    // Serial.print(row);
    // Serial.print(", ");
    // Serial.print(col);
    // Serial.print(" -- ");
    // Serial.print(i);
    // Serial.print(" -- ");
    // Serial.println(synthMIDIenabled[i]);

    char buf[3] = ""; // "DS" + null
    if (drumArr[i] && synthArr[i]) {
      strcpy(buf, "DS");
    } else if (drumArr[i]) {
      strcpy(buf, "D");
    } else if (synthArr[i]) {
      strcpy(buf, "S");
    } else {
      strcpy(buf, "-"); 
    }

    int charW = 6;  // default font width
    int charH = 8;  // default font height
    int textW = strlen(buf) * charW;
    int textH = charH;

    int cellX = col * cellW;
    int cellY = row * cellH;

    int x = cellX + (cellW - textW) / 2;
    int y = cellY + (cellH - textH) / 2;

    statusDisplay.setCursor(x, y);
    statusDisplay.print(buf);
  }

  statusDisplay.display();
  // safeDisplayUpdate();
}

//todo: remove



void drawStretchDisplay(){
  seg7display.showNumberDecEx(100*stretchValue,0b00000000,true,4, 0);
}

void showPanicDisplay() {
    const char* panicMsg = "PANIC!";
    statusDisplay.clearDisplay();
    statusDisplay.setTextSize(2);
    statusDisplay.setTextColor(SSD1306_WHITE);

    int16_t x1, y1;
    uint16_t w, h;
    statusDisplay.getTextBounds(panicMsg, 0, 0, &x1, &y1, &w, &h);

    int x = (statusDisplay.width() - w) / 2;
    int y = (statusDisplay.height() - h) / 2;

    statusDisplay.setCursor(x, y);
    statusDisplay.println(panicMsg);
    statusDisplay.display();
}

void showVelocityCoercionLabel(){
  const char* velMsg = velocityCoerceLabels[velocityCoercionIdx];
  statusDisplay.clearDisplay();
  statusDisplay.setTextSize(2);
  statusDisplay.setTextColor(SSD1306_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  statusDisplay.getTextBounds(velMsg, 0, 0, &x1, &y1, &w, &h);
  int x = (statusDisplay.width() - w) / 2;
  int y = (statusDisplay.height() - h) / 2;
  statusDisplay.setCursor(x, y);
  statusDisplay.println(velMsg);
  statusDisplay.display();
}

void drawStretchStatusDisplay() {
  char stretchVal[8];
  snprintf(stretchVal, sizeof(stretchVal), "%.2f", stretchValue);
  statusDisplay.clearDisplay();
  // Small text: 'stretch' top left
  statusDisplay.setTextSize(1);
  statusDisplay.setTextColor(1);
  statusDisplay.setCursor(0, 0);
  statusDisplay.print("stretch");
  // Large text: value bottom right
  statusDisplay.setTextSize(4);
  int16_t x1, y1;
  uint16_t w, h;
  statusDisplay.getTextBounds(stretchVal, 0, 0, &x1, &y1, &w, &h);
  int x = statusDisplay.width() - w - 2;
  int y = statusDisplay.height() - h - 2;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  statusDisplay.setCursor(x, y);
  statusDisplay.print(stretchVal);
  statusDisplay.display();
}
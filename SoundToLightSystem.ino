//// Zhang, Julia
//// Gagneja, Mehek
//// BIOE 490: Microcontroller Applications Final Project

// Sound to Light System
//
// An integrated system that transforms audio input into forms of light. The following modes are available:
// - Music Visualizer: Analyzes audio input and displays resulting frequency spectrum via Processing.
// - Voice Recognition: Control an LED to perform various tasks with your voice. Can be programmed to respond
//                      to general built-in commands or specific voices. Can also be expanded for other actions
//                      besides LED control.
// See specific sketches for more in-depth use instructions.
//
// The system starts in the Music Visualizer mode. To switch to the Voice Recognition mode, press the button
// on the breadboard. To switch back to the Music Visualizer mode, press the RESET button on the Arduino.

#include "MusicVisualizer.h"
#include "VoiceRecognition.h"
#include "Arduino.h"

const int buttonPin = 7; // Setting the pin from which we will be reading button state

MusicVisualizer musicVisualizer; // Object instantiation from the classes we defined 
VoiceRecognition voiceRecognition;

int buttonPressCount = 0; // Constants to help us track changes in button state 
int buttonState = 0;
unsigned long buttonPressTime = 0;
boolean delayCondition = true;

void setup() {
  // This section will set up both musicVisualizer as well as voiceRecognition, set baud rate and set input pin
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  musicVisualizer.setupForMV();
  voiceRecognition.setupForVR();
}

void loop() {
  // Within this loop, we will be switching from music visualization to voice recognition based on the button state as well as tracking time to ensure that button presses are detected only once
  TIMSK0 = 1; // Turn timer back on so millis() can work
  buttonState = digitalRead(buttonPin); // Reading from the pin to check on the state of the button
  
  if (buttonState == HIGH && delayCondition) {
    buttonPressTime = millis(); // Set the time when button is pressed
    delayCondition = false; // Sets condition so that it will not be allowed to keep counting
    buttonPressCount = buttonPressCount + 1; // Change the count of button press
  }
  if(millis() - buttonPressTime > 1000){ // Checking to see if the time elapsed has been sufficiently long, prevents repeated counting of one button press
    delayCondition = true; // Change condition so they can keep counting
  }else{
    delayCondition = false; // Change condition so they can't keep counting
  }
     
  if(buttonPressCount%2 == 0){ // All the even numbers will run music visualizer and all the odd numbers will run voice recognition
    musicVisualizer.FHTOutput();
   }else{ 
    voiceRecognition.runVR();
  }
    
}

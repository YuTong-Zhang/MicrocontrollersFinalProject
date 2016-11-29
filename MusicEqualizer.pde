//// Zhang, Julia
//// Gagneja, Mehek
//// BIOE 490: Microcontroller Applications Final Project

// Equalizer for use with Sound to Light System
// 
// Don't just hear your music, see it. Music equalizer displays frequency spectrum of your favorite
// song in an aesthetically appealing manner. Works in conjuction with Music Visualizer Arduino
// sketch for use with Sound to Light System. Once you have uploaded the Arduino sketch to your board,
// return to this sketch and press play. Then press play on your music and enjoy seeing it on the
// big screen.

import processing.serial.*; // Connects to serial port to access microphone data

Serial port; // Creating a serial object called port
String val; 
PImage fade; // Initializes image for fade effect
float rWidth = 1000*0.99; //Creates reduction factor for fade
float rHeight = 400*0.99;
int hVal = 0; // Initializes hue value for color change effect
int calibration = 0; // Initializes calibration variable
int[] calValues = new int[62]; // Creates empty array for calibration values
int[] calAverages = new int[62]; // Creates empty array for calibration average

void setup(){
  String portName = Serial.list()[2]; // Connects to specific port on device
  port = new Serial(this,portName,9600); // Baud rate set at 9600 to access serial port
  size(1000, 400); // Sets size of equalizer
  fade = loadImage("pressplay.jpg");  // Sets personalized image as initial fade image
}

void draw(){
  background(0);
  // When implemented in a loop, the following section creates a 3-D effect where the previous 
  // frequency spectrums freeze and fade back into the screen in a Star-Wars-esque manner
  tint(255,255,255,252); // Reduces brightness of following image
  image(fade,(width-rWidth)/2,(height-rHeight)/2,rWidth,rHeight); // Displays faded image slightly smaller, and higher than original image
  noTint(); // Turns off tint
  
  smooth();
  colorMode(HSB); // Sets color mode value 
  fill(hVal, 255,255); // Cycles through hue and brightness to expose a greater color palete
  stroke(hVal, 255,225); // Sets the stroke to cycle through the whole color spectrum 
  colorMode(RGB); //Sets color mode to red green and blue
  
  
  if(port.available() > 0){ // Check for serial connection
    val = port.readStringUntil('\n'); // Imports a line of microphone data. Each line of microphone data corresponds to a spectrum of 32 bins.
  }
  if(val != null){ // If imported values from the microphone exist
    String[] values = val.split(" "); // Split the line of data into an array split at spaces
    if(values.length == 33){ // Creates a new array that ensures no extra values are present
      int count = 0;
      String[] newValues = new String[values.length-3];
      
      /*
       * This following section calibrates the system by tracking values in the ambient environment and subtracting them from the final display
       * This allows for better visualization of the changes in the input from music rather than displaying always existent noise
       */    
      if(calibration < 11){ // Calibrates for 10 iterations to set baseline audio input
        calibration = calibration + 1; // Keeps track of how many iterations has passed
        for(int i = 2; i<values.length-1; i++){ // Sum up each bin for each iteration
          int val = Integer.parseInt(values[i-2]);
          calValues[i-2] = calValues[i-2] + val;
        }
        if(calibration == 10){ // Once enough data has been collected, we divide by 10 to get the average noise for each bin
          for(int i = 0; i<calValues.length-1; i++){
            calAverages[i] = calValues[i]/10; // Getting the average
          }
        }
      }
      else{
        for(int i = 2; i<values.length-1; i++){
          newValues[i-2] = values[i];
        }
        // Creates the colored snapshots of the equalizer
        for(int i = 0; i<newValues.length-1; i++){
            count = count +1;  // Keeping track of which bin we are looking at and where it should go on the equalizer
            int castedBinValue = Integer.parseInt(newValues[i]); // Parses values from string into integer
            castedBinValue = castedBinValue - calAverages[i]; // Calibrates value by subtracting calibrated average
            if(castedBinValue < 0){ // Eliminates any negative values by setting them to zero
              castedBinValue = 0;
            }
            rect(count*30, 400, 30, -castedBinValue*5); // Equalizer magic. The value of each bin is used to create a rectangle of that height, representing the amplitude of the frequency.
        }
        fade = get(); // Grabs current screen as fade image to pass onto next loop
        count = 0; // Resets count
        // Creates the white initial snapshot of the equalizer
        for(int i = 0; i<newValues.length-1; i++){
            count = count +1; // Keeping track of which bin we are looking at and where it should go on the equalizer
            int castedBinValue = Integer.parseInt(newValues[i]); // Parses values from string into integer
            castedBinValue = castedBinValue - calAverages[i]; // Calibrates value by subtracting calibrated average
            if(castedBinValue < 0){ // Eliminates any negative values by setting them to zero
              castedBinValue = 0;
            }
            fill(255);
            stroke(255);
            rect(count*30, 400, 30, -castedBinValue*5); // Equalizer magic. The value of each bin is used to create a rectangle of that height, representing the amplitude of the frequency.
        }
        count = 0; // Resets count
      }
    }
    hVal +=1; // Adds to hue value so color changes in next pass through loop
    if(hVal > 255){ // Resets hue value when it exceeds 255
      hVal = 0;
    }
  }
}
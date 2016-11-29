// Music Visualizer header file for use with Sound to Light System
//
// Don't just hear your music, see it. Music Visualizer works in conjunction with the Music Equalizer Processing
// sketch to display a frequency spectrum of your favorite song. Performs a fast Hartley transform (FHT) to 
// process incoming audio data (from microphone wired to Arduino). Processed data is then sent to Processing 
// sketch via serial port for it to be displayed.
// To use, simply upload the sketch onto your board, then press play on the Processing sketch. Then press play on 
// your music and enjoy seeing it on the big screen.

#define LOG_OUT 1 // Use the log output function
#define FHT_N 64 // Set to 64 point fht
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // Shorthand functions that do bit manipulation
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#include <FHT.h> // include the library
#include "Arduino.h"

/*
 * Creating a class for music visualizer with all of its variables and methods
 */
class MusicVisualizer{
  public:
    unsigned long StartTime = 0;
    unsigned long EndTime = 0;
    void setupForMV(); // Methods that belong in this class which includes setting up music visualizer and running FHT on the data from microphone
    void FHTOutput();
};

/*
 * setupForMV() is able to change sampling speed, use serial port, input pin and change adc properties
 */
void MusicVisualizer::setupForMV() {
   cbi(ADCSRA,ADPS2); // Bit manipulation to alter sampling speed of the arduino
   sbi(ADCSRA,ADPS1);
   sbi(ADCSRA,ADPS0);
   Serial.begin(9600); // Use the serial port
   // turn off timer0 for lower jitter
   ADCSRA = 0xe5; // Set the adc to free running mode
   ADMUX = 0x40; // Use adc0
   DIDR0 = 0x01; // Turn off the digital input for adc0
}

/*
 * FHTOutput() runs a fast hartley transform to change the audio input from time to frequency spectrum, then these values are binned and then displayed in processing
 */
void MusicVisualizer::FHTOutput() {
    TIMSK0 = 0; // Turning off timer reduces jitter
     cli();  // UDRE interrupt slows this way down on arduino1.0
     for (int i = 0 ; i < FHT_N ; i++) { // Save 64 samples
       while(!(ADCSRA & 0x10)); // Wait for adc to be ready
       ADCSRA = 0xf5; // Restart adc
       byte m = ADCL; // Fetch adc data
       byte j = ADCH;
       int k = (j << 8) | m; // Form into an int
       k -= 0x0200; // Form into a signed int
       k <<= 6; // Form into a 16b signed int
       fht_input[i] = k; // Put real data into bins
     }
     fht_window(); // Window the data for better frequency response
     fht_reorder(); // Reorder the data before doing the fht
     fht_run(); // Process the data in the fht
     fht_mag_log(); // Take the output of the fht
     sei();
     for(byte i = 0;i < FHT_N/2;i++){ // For each value inside fht output, print it out or print 0 if less than 1, gets rid of nonzero values
      if(fht_log_out[i] < 1){
        Serial.print(0);
      }
      else{
        Serial.print(fht_log_out[i]);
      }
      Serial.print(" ");
     }
     Serial.println();
 }


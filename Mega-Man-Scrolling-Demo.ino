//Mega Man Scrolling Demo
//Created by UncleSporky, 5/2015
//
//This program draws a simple scrolling background with Mega Man
//running in front of it as music plays.  The graphics and music
//are both from Mega Man 2, Wily's castle stage 1, on the NES.
//
//The d-pad changes the greyscale display mode, and A and B
//control the music.  Hold A as you turn on the Arduboy to start
//with the music muted.
//
//Program code copyright (c) UncleSporky 2015
//All rights reserved
//All other copyrights belong to their respective copyright holders

#include <SPI.h>
#include "arduboy.h"
#include "bitmaps.h"
#include "music.h"

#define B_BUTTON 1
#define A_BUTTON 2
#define DOWN 4
#define RIGHT 8
#define UP 16
#define LEFT 32

Arduboy display;

byte frame = 0;       //greyscale frame - 0 displays one grey frame,
                        //1 and 2 display the other, emphasizing some
                        //bits and giving us two shades of grey
byte BGmove = 0;      //counts how many pixels the background has moved
int barrelx = 64;     //location of the background barrels
int cloudx = 16;      //location of the cloud
byte megaCounter = 0; //a counter so we don't advance Mega Man's animation frames too quickly
byte megaFrame = 0;   //Mega Man's animation frame
long delta = 0;       //the time that has passed between one frame of motion and the next
byte buttons;         //stores the current button state
byte drawMode = 2;    //the current user-selected greyscale drawing mode
byte musicMode = 1;   //the current user-selected music mode (1 = play music)

long before;
long after;

int microDelay=7480;
long next_micros =0;
unsigned int voltage=0;

int tick=0;
void setup()
{
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    display.start();

    buttons = display.getInput();  //begin with a quick check of the A button
    if(buttons & A_BUTTON) {musicMode = 0;}  //if the user wants to start the program muted

    drawMode = 1;
    next_micros = micros();
}

void loop() {
  //In the drawBitmap code below, (bool)frame * SIZE_WHATEVER is basically saying,
  //if the frame is greater than 0, then skip the first bitmap and render the second
  //one instead.  This is what alternates rapidly between the two bitmaps and displays
  //the image with two shades of grey.

  //I have modified the default arduboy.cpp drawBitmap function.
  //It used to | (OR) bitmaps with the background, which meant that you couldn't draw
  //bitmaps on top of existing ones to overwrite them.  I simply changed this to = (equals)
  //to overwrite those pixels.  Without this, a display.clearDisplay() would be
  //necessary here.  Instead, we just paint over the whole screen with background slices.

  //I also copied drawBitmap and modified it to create a drawMaskedBitmap function, for
  //drawing Mega Man with transparency.  First it does an & (AND) with the background and
  //the mask to draw black pixels in the shape of Mega Man, and then it does a ^ (XOR) on
  //top of that with the bitmap.  Background in transparent areas is untouched, but the
  //black space gets Mega Man painted on it.

  before = micros();
  display.clearDisplay();
  for(byte x = 0; x < 5; x++) {  //the background "slice" is 32 pixels wide, so we need to draw
                                 //5 of them - one will usually be partly offscreen on the left
                                 //side, and one on the right side
    // for (byte y=0; y<4; y++) {
    display.drawSprite((x * 32) - BGmove, 0, bg, frame%2);
    // }
    // display.drawBitmap((x * 32) - BGmove, (x%2==0) ? 8 : -8, bg + ((bool)frame * SIZE_BG), 32, 64, 1);
  }

  //draw three barrels
  // display.drawBitmap(barrelx, 32, barrel + ((bool)frame * SIZE_BARREL), 16, 16, 1);
  // display.drawBitmap(barrelx + 32, 32, barrel + ((bool)frame * SIZE_BARREL), 16, 16, 1);
  // display.drawBitmap(barrelx + 32, 24, barrel + ((bool)frame * SIZE_BARREL), 16, 16, 1);
  display.drawSprite(barrelx, 32, barrel, frame%2);
  display.drawSprite(barrelx + 32, 32, barrel, frame%2);
  display.drawSprite(barrelx + 32, 24, barrel, frame%2);


  //draw a cloud
  // display.drawBitmap(cloudx / 2, 0, cloud + ((bool)frame * SIZE_CLOUD), 48, 8, 1);
  display.drawSprite(cloudx / 2, 0, cloud, frame%2);

  //draw Mega Man, with his sprite masked in order to do transparency
  //the megaFrame calculation skips downward to later frames of his run animation
  // display.drawMaskedBitmap(52, 24, megaman + (megaFrame * (SIZE_MEGAMAN * 3)) + ((bool)frame * SIZE_MEGAMAN),
  //                                  megaman + (megaFrame * (SIZE_MEGAMAN * 3)) + (SIZE_MEGAMAN * 2), 24, 24, 1);

  display.drawSprite(52,24,megaman, megaFrame*2 + (frame%2), megaman_mask, megaFrame);

  //copy the buffer to the screen and show us the picture we have built!

  after = micros();
  // display.setCursor(0,0);
  // display.print(after-before);

  // if (tick%1024==0) {
  //   display.setCursor(50,16);
  //   display.println(microDelay);
  //   display.display();
  //   delay(250);
  //   }
    tick++;


  // display.println(voltage);



  // delay(9);
  //advance the greyscale control frame
  frame++;
  // delayMicroseconds(microDelay);
  while (micros() < next_micros ) {

  }
  // last_micros += microDelay;
  next_micros=micros()+microDelay;
  display.display();


  if (frame==1 && drawMode==1) {
    // delay(2);
  }
  if(frame > drawMode) {frame = 0;}
  if(drawMode == 0) {frame = 2;}

  // if(musicMode && !display.tune_playing) {display.tune_playscore(wily);}  //play music, and loop it if it finishes playing
  // if(!musicMode && display.tune_playing) {display.tune_stopscore();}  //stop music if the user has chosen to turn it off

  //this delta calculation keeps the animation from happening too fast
  if(millis() >= delta) {
    BGmove++;  //move the background
    if(BGmove > 31) {BGmove = 0;}

    barrelx--;  //move the set of barrels
    if(barrelx < -47) {barrelx = 128;}

    cloudx--;  //move the cloud
    if(cloudx < -95) {cloudx = 256;}

    megaCounter++;  //animate Mega Man; the 6 below controls how quickly his run animates
    if(megaCounter > 6) {megaCounter = 0; megaFrame++;}
    if(megaFrame > 3) {megaFrame = 0;}  //Mega Man has 4 frames in his run animation

    buttons = display.getInput();  //get the current button state

    if(buttons & LEFT) {drawMode = 2;}  //when you press left, you're in mode 0, which shows two shades of grey
    else if(buttons & RIGHT) {drawMode = 1;}  //when you press right, you're in mode 1, which shows one shade of grey
    // else if(buttons & UP) {drawMode = 0;}  //when you press up, you're in mode 2, which draws in black and white

    if ((buttons & UP) ) {
      microDelay+=10;
    }

    if ((buttons & DOWN)) {
      if (microDelay>=10){
        microDelay-=10;
      }
    }

    if(buttons & A_BUTTON) {musicMode = 0;}  //stop playing music when the player presses A
    else if(buttons & B_BUTTON) {musicMode = 1;}  //resume playing music when the player presses B

    // get volate
    // ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    // // we also need MUX5 for temperature check

    // delay(2); // Wait for ADMUX setting to settle
    // ADCSRA |= _BV(ADSC); // Start conversion
    // while (bit_is_set(ADCSRA,ADSC)); // measuring

    // voltage = 1125300L  / ADCW;


    //change the 10 here to make the animation faster or slower!  less is faster
    delta = millis() + 10;
  }


}

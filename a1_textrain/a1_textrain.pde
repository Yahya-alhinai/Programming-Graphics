/**
    CSci-4611 Assignment #1 Text Rain
**/

import processing.video.*;

// Global variables for handling video data and the input selection screen
String[] cameras;
Capture cam;
Movie mov;
PImage inputImage;
boolean inputMethodSelected = false;

int max = 10000;
int n = 0;
int move_down = 0;
char[] letter = new char[max];
int[] place = new int[max];
int[] fall = new int[max];
int[] speed = new int[max];
int threshold = 128;
int debug = 1;
int width_place = 0;
int height_place = 0;


void setup() {
  size(1280, 720);
  inputImage = createImage(width, height, RGB);
  
  String[] lines = loadStrings("TextSource.txt");
  
  for(int h = 0; h < lines.length; h++)
  {
    for(int k = 0; k < lines[h].length(); k++)
    {
      if(n == max)
      {
        h = lines.length;
        break;        
      }

      char let = lines[h].charAt(k);
      // only accept alphabets letter
      if (let == ' ' || let == '\n') 
      {
        width_place = (int)random(10, width-110);
        height_place = (int)random(-100*height, -100);
      }
      if ((let >= 97 && let <= 122) || (let >= 65 && let <= 90))
      {
        letter[n] = let;
        place[n] = (int) random(width_place, width_place+100);
        fall[n] = (int) random(height_place, height_place+100);
        speed [n] = (int) random(1, 8);
        n++;
      }
    }
  }
  
  println("The total number of testRain is " + n);
  //The color and the size of the rainText
  fill(120, 0, 0);
  textSize(22);
}


// Returns: thresholded color (black or white)
color thresholdPixel(color inputPixel) {
  if (((inputPixel >> 8) & 0xFF) < threshold)
    return #000000;
  return #FFFFFF;
}


void draw() {
  // When the program first starts, draw a menu of different options for which camera to use for input
  // The input method is selected by pressing a key 0-9 on the keyboard
  if (!inputMethodSelected) {
    cameras = Capture.list();
    int y = 40;
    text("O: Offline mode, test with TextRainInput.mov movie file instead of live camera feed.", 20, y);
    y += 40;
    for (int i = 0; i < min(9, cameras.length); i++) {
      text(i + 1 + ": " + cameras[i], 20, y);
      y += 40;
    }
    return;
  }

  // This part of the draw loop gets called after the input selection screen, during normal execution of the program.  
  // STEP 1.  Load an image, either from a movie file or from a live camera feed. Store the result in the inputImage variable
  if ((cam != null) && (cam.available())) {
    cam.read();
    inputImage.copy(cam, 0, 0, cam.width, cam.height, 0, 0, inputImage.width, inputImage.height);
  } else if ((mov != null) && (mov.available())) {
    mov.read();
    inputImage.copy(mov, 0, 0, mov.width, mov.height, 0, 0, inputImage.width, inputImage.height);
  }

  // Fill in your code to implement the rest of TextRain here..
  // Tip: This code draws the current input image to the screen
  
  // flip between orignal picture and debugging mode 
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (debug == 1)
        set(width-j, i, inputImage.pixels[i * width + j]);
      else 
        set(width-j, i, thresholdPixel(inputImage.pixels[i * width + j]));
    }
  }
    
  // update the position of the textRain
  for (int i = 0; i < n; i++)
  {
    // update test iff they are above the bottem line limit
    if (fall[i] < height + 20)
    {
      color x = (int) green(get(place[i], fall[i]));
      if (fall[i] < 0)
      {
        fall[i]++;
      } 
      else if(x > 128 || fall[i] > height - 5)
      {
        fall[i] = fall[i] + speed[i] + fall[i]/150;
      }
      else 
      {
        if (speed[i] != 1) speed[i] = 1;
        for (int j=0; j < 5; j++){
          fall[i] -= 3;
          x = (int) green(get(place[i], fall[i]));
          if (x > 128) break;
        }
      }
      
      text(letter[i], place[i], fall[i]);
    }
  }
  
}



void keyPressed() {
  if (!inputMethodSelected) {
    // If we haven't yet selected the input method, then check for 0 to 9 keypresses to select from the input menu
    if ((key >= '0') && (key <= '9')) {
      int input = key - '0';
      if (input == 0) {
        println("Offline mode selected.");
        mov = new Movie(this, "TextRainInput.mov");
        mov.loop();
        inputMethodSelected = true;
      } else if ((input >= 1) && (input <= 9)) {
        println("Camera " + input + " selected.");
        // The camera can be initialized directly using an element from the array returned by list():
        cam = new Capture(this, cameras[input - 1]);
        cam.start();
        inputMethodSelected = true;
      }
    }
    return;
  }

  // This part of the keyPressed routine gets called after the input selection screen during normal execution of the program
  // Fill in your code to handle keypresses here..
  if (key == CODED) {
    if (keyCode == UP) {
      if (threshold < 255) threshold++;
    } else if (keyCode == DOWN) {
      if (threshold > 0) threshold--;
    }
  } else if (key == ' ') {
    debug ^= 1;
  }

}

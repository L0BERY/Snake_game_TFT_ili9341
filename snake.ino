#include "FS.h"
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false

#define H 240
#define W 320

//TFT_eSPI_Button button;

bool toch = true;
bool SnakeRun = true;
const int MaxLen = (W * H) / 64 - 2;
const int up = 0;
const int down = 1;
const int left = 2;
const int right = 3;

int SnakeLen = 3;
int Snake = up;
int Snake_X[MaxLen] = {0};
int Snake_Y[MaxLen] = {0};

int FoodX = random(5, 315);
int FoodY = random(4, 235);

unsigned long timee;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.invertDisplay(0);
  tft.setRotation(1);
  touch_calibrate();
 
  tft.fillScreen(TFT_BLACK);
 
  tft.drawRect(0, 0, W, H, TFT_WHITE);


  Snake_X[0] = W / 2 - 4;
  Snake_Y[0] = H / 2;

  timee = millis();
}

void loop() {
  
  if(SnakeRun){
    tft.drawRect(0, 0, W, H, TFT_WHITE);
    Upd();
  }
  else if(!SnakeRun){

  }

}

void Upd(){
  uint16_t x = 0, y = 0; 
  if(tft.getTouch(&x, &y)){
    if((x < 100) && (Snake != up) && toch){
      Snake = down;
      toch = false;
    }
    if((x > 220) && (Snake != down) && toch){
      Snake = up;
      toch = false;
    }
    if((y < 120) && (x > 100) && (x < 220) && (Snake != right) && toch){
      Snake = left;
      toch = false;
    }
    if((y > 120) && (x > 100) && (x < 220) && (Snake != left) && toch){
      Snake = right;
      toch = false;
    }
  }
  if((millis() - timee) > 100){
     timee = millis();
    toch = true;
    if(Snake_X[0] - 3 <= FoodX && Snake_Y[0] - 3 <= FoodY && Snake_X[0] + 3 >= FoodX && Snake_Y[0] + 3 >= FoodY){
      SnakeLen++;
      tft.fillCircle(FoodX, FoodY, 5, TFT_BLACK);
      FoodX = random(1, 53) * 6;
      FoodY = random(1, 40) * 6;
    }

    int xx = Snake_X[SnakeLen - 1], yy = Snake_Y[SnakeLen - 1];
    for(int i = SnakeLen - 2; i >= 0; i--){
     Snake_X[i + 1] = Snake_X[i];
     Snake_Y[i + 1] = Snake_Y[i];
    }
    if(Snake == up){
      Snake_Y[0] = Snake_Y[0] - 6;
    }
    else if(Snake == down){
      Snake_Y[0] = Snake_Y[0] + 6;
    }
    else if(Snake == left){
      Snake_X[0] = Snake_X[0] - 6;
     }
    else if(Snake == right){
      Snake_X[0] = Snake_X[0] + 6;
    }
    if(Snake_X[0] <= 0){
      SnakeRun = false;
      //Snake_X[0] = W;
    }
    else if(Snake_X[0] > W){
      SnakeRun = false;
      //Snake_X[0] = 0;
    }
    else if(Snake_Y[0] <= 0){
      SnakeRun = false;
      //Snake_Y[0] = H;
    }
    else if(Snake_Y[0] > H){
      SnakeRun = false;
      //Snake_Y[0] = 0;
    }

    for(int i = 4; i < SnakeLen; i++){
      if(Snake_X[0] == Snake_X[i] && Snake_Y[0] == Snake_Y[i]){
        SnakeRun = false;
      }
    }

    tft.fillCircle(FoodX, FoodY, 3, TFT_BLUE);
    //tft.fillRect(xx , yy , 5, 5, TFT_BLACK);
    tft.fillCircle(xx, yy, 3, TFT_BLACK);
    for(int i = 0; i < SnakeLen; i++){
       //tft.fillRect(Snake_X[i] , Snake_Y[i] , 5, 5, TFT_GREEN);
       tft.fillCircle(Snake_X[i], Snake_Y[i], 3, TFT_GREEN);
    }
  }
}








void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  if (!SPIFFS.begin()) {
    SPIFFS.format();
    SPIFFS.begin();
  }

  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    tft.setTouch(calData);
  } else {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
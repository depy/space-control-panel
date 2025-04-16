#include <SD.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4
#define SD_CS     5

#define SCREEN_W 240
#define SCREEN_H 320 
#define IMG_W 240
#define IMG_H 160

#define BUFFER_SIZE IMG_W * IMG_H

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
uint16_t buffer[BUFFER_SIZE];

File bmpFile;

struct Screen {
  char* imgFilename;
  char* title;
  char* diameterInfo;
  char* orbitalPeriodInfo;
  char* gravityInfo;
  char* tempInfo;
  char* moonsInfo;
  uint16_t titleColor;
};

struct Screen mercury = {
  "/mercury.bmp",
  "Merkur",
  "Premer: 4879km",
  "Orbita: 88d",
  "Gravitacija: 3.7",
  "Temp: -180 - 430C",
  "Lune: 0",
  0xBDF7
};

struct Screen venus = {
  "/venus.bmp",
  "Venera",
  "Premer: 12104km",
  "Orbita: 225d",
  "Gravitacija: 8.9",
  "Temp: 440 - 480C",
  "Lune: 0",
  0xFEA0
};

struct Screen earth = {
  "/earth.bmp",
  "Zemlja",
  "Premer: 12756km",
  "Orbita: 365d",
  "Gravitacija: 9.8",
  "Temp: -88 - 58C",
  "Lune: 1 Luna",
  0x64ff
};

struct Screen mars = {
  "/mars.bmp",
  " Mars",
  "Premer: 6782km",
  "Orbita: 687d",
  "Gravitacija: 3.7",
  "Temp: -150 - 20C",
  "Lune: 2\n Fobos Deimos",
  0xCAAA
};

struct Screen jupiter = {
  "/jupiter.bmp",
  "Jupiter",
  "Premer: 142984km",
  "Orbita: 4332d",
  "Gravitacija: 23.1",
  "Temp: ~ -110C",
  "Lune: 95  Io Evropa\n Ganimed Kalisto",
  0xFEEF
};

struct Screen saturn = {
  "/saturn.bmp",
  "Saturn",
  "Premer: 120536km",
  "Orbita: 10759d",
  "Gravitacija: 9.0",
  "Temp: ~ -140C",
  "Lune: 274  Titan\n Mimas Japet Rea",
  0xE6D6
};

struct Screen uranus = {
  "/uranus.bmp",
  " Uran",
  "Premer: 51118km",
  "Orbita: 30688d",
  "Gravitacija: 8.7",
  "Temp: ~ -200C",
  "Lune: 28",
  0xB77B
};

struct Screen neptune = {
  "/neptune.bmp",
  "Neptun",
  "Premer: 49528km",
  "Orbita: 60182d",
  "Gravitacija: 11.0",
  "Temp: ~ -200C",
  "Lune: 16",
  0x7E7E
};

void drawBMP(char* filename) {
  bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.println("Failed to open image.");
    return;
  }

  bmpFile.seek(54);
  for(int i=0; i< IMG_W * IMG_H; i++) {
    uint8_t pixel[3];
    bmpFile.read(pixel, 3);

    uint16_t color = tft.color565(pixel[2], pixel[1], pixel[0]);
    buffer[BUFFER_SIZE-1-i] = color;
  };

  tft.drawRGBBitmap(0, 0, buffer, 240, 160);
  bmpFile.close();
}

void drawScreen(Screen scr) {
  drawBMP(scr.imgFilename);

  uint16_t color = tft.color565(0, 0, 0);
  for(int i=0; i< IMG_W * IMG_H; i++) {
    buffer[BUFFER_SIZE-1-i] = color;
  };
  tft.drawRGBBitmap(0, 160, buffer, 240, 160);

  tft.setCursor(65, 160);
  tft.setTextColor(scr.titleColor);
  tft.setTextWrap(false);
  tft.setTextSize(3);
  tft.print(scr.title);

  tft.drawFastHLine(00, 185, 240, 0x6b6d);

  tft.setCursor(5, 200);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.diameterInfo);
  
  tft.setCursor(5, 220);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.orbitalPeriodInfo);

  tft.setCursor(5, 240);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.gravityInfo);

  tft.setCursor(5, 260);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.tempInfo);

  tft.setCursor(5, 280);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(2);
  tft.print(scr.moonsInfo);
}

void setup(void) {
  Serial.begin(115200); 

  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR: SD card initialization failed!");
    while(1);
  }
  Serial.println("SD card initialized...");

  tft.init(240, 320);    // Init ST7789 display 135x240 pixel
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  drawScreen(mercury);
  delay(3000);
  drawScreen(venus);
  delay(3000);
  drawScreen(earth);
  delay(3000);
  drawScreen(mars);
  delay(3000);
  drawScreen(jupiter);
  delay(3000);
  drawScreen(saturn);
  delay(3000);
  drawScreen(uranus);
  delay(3000);
  drawScreen(neptune);
  delay(3000);
}
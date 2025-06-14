#include <SD.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#define TFT_SCLK 18
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_CS   17
#define TFT_RST  16
#define TFT_DC    4
#define SD_CS     5

#define NEXT_PLANET_BTN_PIN 22

#define SCREEN_W 240
#define SCREEN_H 320 
#define IMG_W 240
#define IMG_H 160

#define BUFFER_SIZE IMG_W * IMG_H

// Screen data rendered to the screen for each planet
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

// ----- Define a screen per planet ------

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
  "Lune: 1",
  0x64ff
};

struct Screen mars = {
  "/mars.bmp",
  " Mars",
  "Premer: 6782km",
  "Orbita: 687d",
  "Gravitacija: 3.7",
  "Temp: -150 - 20C",
  "Lune: 2",
  0xCAAA
};

struct Screen jupiter = {
  "/jupiter.bmp",
  "Jupiter",
  "Premer: 142984km",
  "Orbita: 4332d",
  "Gravitacija: 23.1",
  "Temp: ~ -110C",
  "Lune: 95",
  0xFEEF
};

struct Screen saturn = {
  "/saturn.bmp",
  "Saturn",
  "Premer: 120536km",
  "Orbita: 10759d",
  "Gravitacija: 9.0",
  "Temp: ~ -140C",
  "Lune: 274",
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

// ----- Variables -----

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
File bmpFile;
uint16_t buffer[BUFFER_SIZE];

uint16_t last_button_time = 0; 
bool drawing = false;
bool nextPlanetPressed = false;
int currentScreen = 0;
Screen screens[8] = {mercury, venus, earth, mars, jupiter, saturn, uranus, neptune};

int pwrLEDsPins[8] = {32, 33, 25, 26, 27, 14, 12, 13};
int pwrPotPin = 34;
TaskHandle_t pwrTaskHandle;

// ----- Functions -----

// Reads the BMP file from SD card and renders it to the screen
void drawBMP(char* filename) {
  bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.println("Failed to open image.");
    return;
  }

  // Skip BMP header
  bmpFile.seek(54);

  // Read the image to a buffer first
  for(int i=0; i< IMG_W * IMG_H; i++) {
    // Read 3 bytes (24-bit BMP)
    uint8_t pixel[3];
    bmpFile.read(pixel, 3);

    // Convert 24-bit color (RGB 888) to 16-bit color (RGB 565)
    // and put pixels in reverse order in buffer to mirror the image
    uint16_t color = tft.color565(pixel[2], pixel[1], pixel[0]);
    buffer[BUFFER_SIZE-1-i] = color;
  };

  // Draw the image from the buffer
  tft.drawRGBBitmap(0, 0, buffer, 240, 160);
  bmpFile.close();
}

// Draws the whole screen
void drawScreen(Screen scr) {
  drawBMP(scr.imgFilename);

  // Fill the buffer with black color to clear text before render new text
  uint16_t color = tft.color565(0, 0, 0);
  for(int i=0; i< IMG_W * IMG_H; i++) {
    buffer[i] = color;
  };
  // Clear the bottom half of the screen with black color from buffer
  tft.drawRGBBitmap(0, 160, buffer, 240, 160);

  // Print title
  tft.setCursor(65, 160);
  tft.setTextColor(scr.titleColor);
  tft.setTextWrap(false);
  tft.setTextSize(3);
  tft.print(scr.title);

  // Print dividing line
  tft.drawFastHLine(00, 185, 240, 0x6b6d);

  // Print diameter info
  tft.setCursor(5, 202);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.diameterInfo);
  
  // Print orbital info
  tft.setCursor(5, 224);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.orbitalPeriodInfo);

  // Print gravity info
  tft.setCursor(5, 246);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.gravityInfo);

  // Print temperature info
  tft.setCursor(5, 268);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(2);
  tft.print(scr.tempInfo);

  // Print moons info
  tft.setCursor(5, 290);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(2);
  tft.print(scr.moonsInfo);
}

void IRAM_ATTR nextPlanet() {
  if(millis() - last_button_time > 50) {
    nextPlanetPressed = true;
  }
}

void pwrCtrlTask(void *param) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  int currentLEDs = 0;

  int onThresholds[8] = {200, 700, 1200, 1700, 2200, 2700, 3200, 3700};
  int offThresholds[8] = {100, 800, 1100, 1800, 2100, 2600, 3100, 3600};

  while(1) {
    int pwr = 4096 - analogRead(pwrPotPin);

    if(currentLEDs < 8 && pwr >= onThresholds[currentLEDs]) {
      currentLEDs += 1;
    }

    if(currentLEDs > 0 && pwr < offThresholds[currentLEDs - 1]) {
      currentLEDs -= 1;
    }

    for(int i = 0; i<currentLEDs; i++) {
      digitalWrite(pwrLEDsPins[i], LOW);
    }

    for(int i = currentLEDs; i < 8; i++) {
      digitalWrite(pwrLEDsPins[i], HIGH);
    }

    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(20));
  }
}



// ----- Setup and loop

void setup(void) {
  Serial.begin(115200); 

  pinMode(pwrPotPin, INPUT);
  for(int i=0; i<8; i++) {
    pinMode(pwrLEDsPins[i], OUTPUT);
  }
  
  xTaskCreatePinnedToCore(
        pwrCtrlTask,
        "PotReader",
        2048,        // Stack size
        NULL,        // Parameters  
        2,           // Priority
        &pwrTaskHandle,
        1            // Core
    );

  // SD card init
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR: SD card initialization failed!");
    //while(1);
  }
  Serial.println("SD card initialized...");

  // Display init
  tft.init(240, 320);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  
  // Next planet button settings
  pinMode(NEXT_PLANET_BTN_PIN, INPUT_PULLUP);
  attachInterrupt(NEXT_PLANET_BTN_PIN, nextPlanet, RISING);

  drawScreen(screens[currentScreen]);
}

void loop() {
  // Run only if button was pressed and drawing is done
  if(nextPlanetPressed && !drawing) {
    drawing = true;
    currentScreen++;
    if(currentScreen>7) { 
      currentScreen = 0; 
    }
    drawScreen(screens[currentScreen]);
    drawing = false;
    nextPlanetPressed = false;
  }
}
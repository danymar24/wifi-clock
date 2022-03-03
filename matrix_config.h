#include <MatrixHardware_ESP32_V0.h>    
#include <Adafruit_GFX.h>
#include <SmartMatrix_GFX.h>
#include <SmartMatrix.h>
#include <Arduino_JSON.h>

const uint16_t kMatrixWidth = 64;       // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 32;      // Set to the height of your display
const uint8_t kRefreshDepth = 36;       // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;   // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);        // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

const int defaultBrightness = (15*255)/100;     // dim: 35% brightness

SMARTMATRIX_ALLOCATE_BUFFERS(matrixLayer, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

#define mw kMatrixWidth
#define mh kMatrixHeight
#define NUMMATRIX (kMatrixWidth*kMatrixHeight)

CRGB *matrixleds;

void show_callback();

SmartMatrix_GFX *matrix = new SmartMatrix_GFX(matrixleds, mw, mh, show_callback);
// Sadly this callback function must be copied around with this init code
void show_callback() {
    backgroundLayer.swapBuffers(true);
    matrixleds = (CRGB *)backgroundLayer.backBuffer();
    matrix->newLedsPtr(matrixleds);
}

void setupMatrix() {
  // setup matrix
  matrixLayer.addLayer(&backgroundLayer);
  matrixLayer.begin();
  matrixLayer.setBrightness(defaultBrightness);
  // This sets the neomatrix and LEDMatrix pointers
  show_callback();
  matrixLayer.setRefreshRate(240);
  backgroundLayer.enableColorCorrection(true);
  
  // Time for serial port to work?
  delay(1000);
  backgroundLayer.fillScreen( {0x80, 0x80, 0x80} );
  backgroundLayer.swapBuffers();
}

/**
 * @function displayText - accepts char* text
 */
void displayText(char* text, uint8_t size, uint8_t posX, uint8_t posY, int color = LED_BLUE_HIGH) {
  // clear screen before writing new text
  matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
  matrix->setTextSize(size);
  matrix->setRotation(0);
  
  yield();
  matrix->setCursor(posX,posY);
  matrix->setTextColor(color);
  matrix->print(text);
  
}

/**
 * @function displayText - accepts uint8_t text
 */
void displayText(uint8_t text, uint8_t size, uint8_t posX, uint8_t posY, int color = LED_BLUE_HIGH) {
  
  matrix->setTextWrap(false);
  matrix->setTextSize(size);
  matrix->setRotation(0);
  
  yield();
  matrix->setCursor(posX,posY);
  matrix->setTextColor(color);
  matrix->print(text);
  
}

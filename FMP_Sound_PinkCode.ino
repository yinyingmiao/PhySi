#include <FastLED.h>

// Configuration
#define LED_PIN 5       // Pin where the LED strip is connected
#define NUM_LEDS 10     // Number of LEDs in the strip
#define BRIGHTNESS 200  // Overall brightness (0 to 255)
#define COLOR_DELAY 50  // Delay between color transitions (ms)

CRGB leds[NUM_LEDS];

void setup() {
  // Initialize LED strip
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  // Cycle through a range of pink hues
  for (int hue = 220; hue <= 255; hue++) { // Pink hues are typically in this range
    fill_solid(leds, NUM_LEDS, CHSV(hue, 200, BRIGHTNESS)); // Set hue, medium saturation, and brightness
    FastLED.show();
    delay(COLOR_DELAY); // Add delay for a smooth transition
  }

  for (int hue = 255; hue >= 220; hue--) {
    fill_solid(leds, NUM_LEDS, CHSV(hue, 200, BRIGHTNESS));
    FastLED.show();
    delay(COLOR_DELAY);
  }
}

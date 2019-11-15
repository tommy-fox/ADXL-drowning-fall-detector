/**********************************************************************************
 * Tommy Fox
 * Real Time Embedded Systems Final Project
 * 8/30/2019
 * 
 * This program congifures an ADXL345 accelerometer and Teensy 3.2 controller
 * to detect drowning as well as falling.
 * 
 * Double Tap and Free Fall ADXL interrupts are utilized for this functionality.
 * Tap settings are configured so that if device is worn around the wrist,
 * drowning is detected by rapid hand motion, but not triggered by normal swimming.
 * 
 * Double Tap also participates with the Free Fall interrupt in falling detection.
 **********************************************************************************/
#include <Adafruit_ADXL345_U.h>

// Instance of ADXL345()
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Settings for blinking the LED
uint8_t LED_PIN = 13;
uint8_t i = 0;
uint8_t NUM_BLINKS = 3;

// Tap settings
uint8_t TAP_THRESH = 110; // 48 = 3g
uint8_t TAP_DUR = 16;     // 10ms
uint8_t TAP_LATENT = 16;  // 20ms
uint8_t TAP_WINDOW = 128; // 160ms

// Free Fall settings
uint8_t FF_THRESH = 8;    // (5 - 9) recommended
uint8_t FF_DUR = 64;      // (20 - 70) recommended

void setup(void)
{

  // Check sensor initialization
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  // Join the serial bus at 9600 baud
  Serial.begin(9600);
  Serial.println("Accelerometer Test");

  if (!accel.begin())
  {
    Serial.println("No ADXL345 detected ... Check your wiring.");
    while (1);
  }
  else
    Serial.println("Accelerometer is ready.");

  // Initialize LED pin 
  pinMode(LED_PIN, OUTPUT);

  // Enable full resolution on ADXL
  accel.setRange(ADXL345_RANGE_16_G);

  // Set data rate
  accel.setDataRate(ADXL345_DATARATE_100_HZ);

   // Disable interrupts before configuration
  accel.writeRegister(ADXL345_REG_INT_ENABLE, 0b00000000);

  // Set tap threshold
  accel.writeRegister(ADXL345_REG_THRESH_TAP, TAP_THRESH);

  // Set tap duration
  accel.writeRegister(ADXL345_REG_DUR, TAP_DUR);

  // Set tap latency
  accel.writeRegister(ADXL345_REG_LATENT, TAP_LATENT);

  // Set tap window
  accel.writeRegister(ADXL345_REG_WINDOW, TAP_WINDOW);

  // Set free fall threshold
  accel.writeRegister(ADXL345_REG_THRESH_FF, FF_THRESH);

  // Set free fall duration
  accel.writeRegister(ADXL345_REG_TIME_FF, FF_DUR);

  // Map all interrupts to INT1 pin
  accel.writeRegister(ADXL345_REG_INT_MAP, 0b00000000);

  // Enable x-, y-, and z- participation in tap detection
  accel.writeRegister(ADXL345_REG_TAP_AXES, 0b00000111);

  // Enable Double Tap and Free Fall interrupts
  accel.writeRegister(ADXL345_REG_INT_ENABLE, 0b00100100);
}

void detect(void)
{ 
    // Detect Free Fall or Double Tap
    // Bit clears after it is read
    if ((accel.readRegister(ADXL345_REG_INT_SOURCE) & 0b00000100) ||
        (accel.readRegister(ADXL345_REG_INT_SOURCE) & 0b00100000))
  {      
    for (i = NUM_BLINKS; i > 0; i--)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }
  }
}

void loop(void)
{
  detect();
}

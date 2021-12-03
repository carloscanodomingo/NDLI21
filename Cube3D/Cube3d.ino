

/**
  NDLI21 Project TIC019
  Name: 3D_cube_led.h
  Purpose: Packet the interface between main code and Specific FastLED implementation for the 3d cube.

  @author Carlos Cano & Manuel Soler
  @version 0.1 19/09/2021

*/

#include <FastLED.h>
#include <IRremote.h>
#include <elapsedMillis.h>
 //#include "lib8tion.h"
 #include <HCSR04.h>
 #include "PIN_DEFINITION.h"
#include "3D_cube_leds.h"
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_I2CDevice.h>
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include "images.h"
#define CUSTOM_SETTINGS
#define INCLUDE_COLORDETECTOR_MODULE
#include <DabbleESP32.h>
#include <JC_Button.h>
#include "AiEsp32RotaryEncoder.h"
#include <math.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>



#define BRIGHTNESS  64


#define START_STATE 1

// COLUMN DEFINES
#define LED_Y           7
#define LED_X           7
#define LED_Z           3
#define NUM_LEDS LED_X * LED_Y * LED_Z

#define NO_ERROR          0
#define ERROR_ID          9
#define ERROR_COLOR_ERROR 10

// MDOE DEFINITIONS
#define EXAMPLE_MODE    0;
#define START_MODE EXAMPLE_MODE;


#include <AlignedJoy.h>


// CLASS CONSTRUCTOR
// new joystick object
AlignedJoy joystick_1(PIN_JOY1_X, PIN_JOY1_Y);

#define IR_BUTTONS 17


// MODE 7
#define NUM_REPEAT_ULTRASONIC_SENSOR 40
#define DISTANCE_BW_SENSORS_CM  26
#define ROTARY_ENCODER_STEPS 4
#define ZONE_SIZE 3
#define CLOSE_DISTANCE  95          
#define FAR_DISTANCE    135
#define MAX_DISTANCE    220
#define DIST_DIFFERENCE 20
 


#define START_DIFICULTY_LEVEL 8


// MODE 9
#define MAX_RPM               300


Adafruit_MPU6050 mpu;
typedef enum X_POSITION {
  LEFT = 0,
  CENTER = 2,
  RIGHT = 4,
};
 
typedef enum Y_POSITION {
  CLOSE = 0,
  MIDDLE = 2,
  FAR = 4,
};
 
typedef struct LED_ZONES {
  X_POSITION x_zone;
  Y_POSITION y_zone;
};
 
LED_ZONES previous_zones;
typedef enum Z_POSITION {
  Z_TOP,
  Z_MID,
  Z_BOT,
};
 
typedef struct LED_ZONES_3D {
  X_POSITION x_zone;
  Y_POSITION y_zone;
  Z_POSITION z_zone;
};
 
#define ANGLE_THRESHOLD 25
#define Z_THRESHOLD 3
 

unsigned dificulty_level = START_DIFICULTY_LEVEL;

// Replace the next variables with your SSID/Password combination
const char* ssid = "Mi Note 10";
const char* password = "carloscano";

const char* mqtt_server = "192.168.43.1";

WiFiClient espClient;
PubSubClient client(espClient);
// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
//instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);


// MODE 7 ULTRASONIC SENSORS
UltraSonicDistanceSensor distanceSensor_left(SR04_TRIGGER_LEFT_PIN, SR04_ECHO_LEFT_PIN, MAX_DISTANCE, 0);  
UltraSonicDistanceSensor distanceSensor_right(SR04_TRIGGER_RIGHT_PIN, SR04_ECHO_RIGHT_PIN, MAX_DISTANCE, 0); 

// Remote control codes
unsigned  ir_codes[IR_BUTTONS] = {0xFFA25D, 0xFF629D, 0xFFE21D, 0xFF22DD, 0xFF02FD, 0xFFC23D, 0xFFE01F, 0xFFA857, 0xFF906F, 0xFF6897, 0xFF9867, 0xFFB04F, 0xFF18E7, 0xFF10EF, 0xFF38C7, 0xFF5AA5, 0xFF4AB5};
char    ir_keys[IR_BUTTONS]  =  {'1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#', 'U', 'L','O','R', 'D'};


IRrecv irrecv(IR_RECV_PIN);     
decode_results results;   


int selected_mode = START_MODE;
unsigned current_color_mode_4;
// MODE propagate
elapsedMillis since_mode_2;
unsigned since_min_mode_2 = 100; // BT

// MODE propagate
elapsedMillis since_mode_3;
unsigned since_min_mode_3 = 75; // BT

// MODE propagate
elapsedMillis since_mode_4;
unsigned since_min_mode_4 = 75; // BT

// Bluetooht modes
elapsedMillis since_bt;
unsigned since_max_bt = 100; // BT

// MODE 2d location 
elapsedMillis since_mode_7;
unsigned since_min_mode_7 = 100; // BT

// MODE 2d location 
elapsedMillis since_mode_8;
unsigned since_min_mode_8 = 1000; // ACC

// MODE 2d location 
elapsedMillis since_mode_9;
unsigned since_min_mode_9 = 300; // BT

elapsedMillis since_testing;
unsigned  SEPARATION_MS_TEST  = 2000;

CubeLeds cube_led;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
Button myBtn(27,50);       // define the button
void initialSetup()
{
  ColorDetector.setGridSize(GRID_1x1);
  ColorDetector.setCalculationMode(DOMINANT);
  ColorDetector.setColorScheme(RGB_24BIT);
}
void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void do_nothing()
{
  return;
};
 void setup() {
   delay(3);
  Serial.begin(115200);  
  delay(3);
  Serial.println("STARTING");
  irrecv.enableIRIn();
  

  selected_mode = START_STATE;
  
  Serial.println("AFTER DABBLE");
  ColorDetector.sendSettings(&initialSetup);  
  Serial.println("AFTER SETUP");
  cube_led.begin(LED_X, LED_Y, LED_Z ); 
  Serial.println("After cube begin");

  tft.init(240, 240, SPI_MODE2); 
  // optimized lines
  testlines(ST77XX_BLUE);  
  delay(500);
  tft.drawRGBBitmap(0,0,ual_logo,240,240);
  myBtn.begin();
    //we must initialize rotary encoder
  rotaryEncoder.begin();

  //we must initialize rotary encoder
  rotaryEncoder.begin();



  rotaryEncoder.setup(
        [] { rotaryEncoder.readEncoder_ISR(); },
        [] { do_nothing(); });
      //set boundaries and if values should cycle or not
  //in this example we will set possible values between 0 and 1000;
  bool circleValues = true;
  rotaryEncoder.setBoundaries(0, 100000, circleValues);  //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  /*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
  //rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoder.setAcceleration(250);  //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration


  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

typedef struct config_mode_2_t{
  unsigned x;
  unsigned y;
  bool col_selected[LED_X * LED_Y];
  CHSV colors[LED_X * LED_Y];
};

void tft_select_mode(unsigned mode,const char * title, const char * description)
{
  tft.fillScreen(ST77XX_WHITE);
  tft.setTextWrap(false);
  tft.setCursor(0, 15);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(3);
  tft.print("Mode: ");
  tft.println(mode);
  tft.setTextSize(1);
  tft.println("");
  tft.setTextColor(ST77XX_ORANGE );
  tft.setTextSize(2);
  tft.print("- ");
  tft.print(title);
  tft.println(" -");
  tft.setCursor(0, 80);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK );
  tft.println(description);
  tft.drawRGBBitmap(0,140,ual_logo_h,240,100);
}
void tft_update_description(const char * description)
{
  tft.fillRect(0, 80, 240, 50,ST77XX_WHITE);
  tft.setCursor(0, 80);
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_BLACK );
  tft.println(description);
  
}

void tft_pride(const char * description)
{

  tft.drawRGBBitmap(0,0,pride_logo,240,140);
    tft.setCursor(0, 30);
  tft.setTextSize(4);
  tft.setTextColor(ST77XX_WHITE );
  tft.println(description);
  
}
void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
    delay(5);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
    delay(5);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
    delay(5);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
    delay(5);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
    delay(20);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
    delay(10);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
    delay(10);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
    delay(10);
  }
}
//////////////////////////////////
//        MODE CONFIGURATION    //
//////////////////////////////////
//////////////////////////////////////
//                                  //
//            MODE  OK               //
//          RESTART               //
//                                  //
//////////////////////////////////////
void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
  
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    SingleLed led;
    cube_led.get_led(pixelnumber, &led);
    CHSV blend_color;
    nblend( blend_color, CHSV( hue8, sat8, bri8), 64);
    led.color = blend_color;
    led.state = LED_SOLID;
    cube_led.set_led(pixelnumber, led);

  }
}


//////////////////////////////////////
//                                  //
//            MODE 1                //
//                                  //
//////////////////////////////////////
void initialize_mode_1(CHSV color)
{
  tft_select_mode(1, "SOLID", "");
  cube_led.set_leds_color_solid(color);
}
void run_mode_1(CHSV color)
{
  cube_led.set_leds_color_solid(color);
}


//////////////////////////////////////
//                                  //
//            MODE 2    
//           JOYSTICK               //
//////////////////////////////////////
struct {
  int x;
  int y;
  SingleLed columns[LED_X * LED_Y];
} config_mode_2;

struct {
  int x;
  int y;
} m2_joystick_pos;
void initialize_mode_2()
{
  tft_select_mode(2, "JOYSTICK", "");
  SingleLed current_led = SingleLed{COLOR_OFF, LED_OFF};
  joystick_1.setCalibratedPoint(X, MIN, 0);
  joystick_1.setCalibratedPoint(X, MID, 4095 / 2 );
  joystick_1.setCalibratedPoint(X, MAX, 4095);
  joystick_1.setCalibratedPoint(Y, MIN, 0);
  joystick_1.setCalibratedPoint(Y, MID, 4095 / 2 );
  joystick_1.setCalibratedPoint(Y, MAX, 4095);
  m2_joystick_pos.x = 1;
  m2_joystick_pos.y = 1;
  for (int x = 0; x < LED_X; x++){
    for(int y = 0; y < LED_Y; y++)
    {
      config_mode_2.columns[x + y * LED_X] = current_led;
    }
  }
  since_mode_2 = 0;
}
#define MAX_JOY_VALUE 7
int adapt_joystick(unsigned value)
{
  if(value == 0)
  {
    return -1;
  }else if(value == MAX_JOY_VALUE)
  {
      return 1;
  }else {
    return 0;
  }
}


void run_mode_2(CHSV main_color, CHSV alt_color)
{
  myBtn.read();
    if (myBtn.wasReleased())    // if the button was released, change the LED state
  {
      Serial.println("was release");
        SingleLed current_led = SingleLed{cube_led.get_main_color(), LED_SOLID};
        config_mode_2.columns[config_mode_2.x + config_mode_2.y * LED_X] = current_led;
  }
  // MODE propagate
  if(since_mode_2 < since_min_mode_2)
    return;
  since_mode_2 = 0;
  int current_joystick_x = adapt_joystick(joystick_1.read(X,0,MAX_JOY_VALUE));
  int current_joystick_y = adapt_joystick(joystick_1.read(Y,0,MAX_JOY_VALUE));
  Serial.print("Read Position (");
  Serial.print(joystick_1.read(X,0,MAX_JOY_VALUE));
  Serial.print(",");
  Serial.print(joystick_1.read(Y,0,MAX_JOY_VALUE));
  Serial.println(")");
  if ( current_joystick_x != m2_joystick_pos.x  || current_joystick_y != m2_joystick_pos.y)
  {
    if (current_joystick_x == 0 && current_joystick_y == 0)
    {
      config_mode_2.x = ((( config_mode_2.x + m2_joystick_pos.x)) + LED_X )% LED_X;
      config_mode_2.y = ((( config_mode_2.y + m2_joystick_pos.y)) + LED_Y )% LED_Y;
      Serial.print("New Position (");
      Serial.print(config_mode_2.x);
      Serial.print(",");
      Serial.print(config_mode_2.y);
      Serial.println(")");
    }
     m2_joystick_pos.x = current_joystick_x;
     m2_joystick_pos.y = current_joystick_y;     
  }

  cube_led.set_leds_off();
  for (int x = 0; x < LED_X; x++){
    for (int y = 0; y < LED_Y; y++){
          cube_led.set_led_column(x, y,&(config_mode_2.columns[x + y * LED_X]), 1);
      }
    }
  cube_led.select_column(config_mode_2.x, config_mode_2.y , main_color);
}
//////////////////////////////////////
//                                  //
//            MODE 3                //
//                                  //
//////////////////////////////////////
uint8_t color_index_mode_3;
CHSVPalette16 current_palette_mode_3;
void initialize_mode_3()
{
   tft_select_mode(3,"Cloudy", "What a weather!");
    uint8_t color_index_mode_3 = 0;
       current_palette_mode_3 = CHSVPalette16(
                                   COLOR_GREEN,  COLOR_GREEN,  COLOR_BLACK,  COLOR_BLACK,
                                   COLOR_PURPLE, COLOR_PURPLE, COLOR_BLACK,  COLOR_BLACK,
                                   COLOR_GREEN,  COLOR_GREEN,  COLOR_BLACK,  COLOR_BLACK,
                                   COLOR_PURPLE, COLOR_PURPLE, COLOR_BLACK,  COLOR_BLACK );

}
void run_mode_3()
{
// MODE propagate
  if(since_mode_3 < since_min_mode_3)
    return;
  since_mode_3 = 0;

    for( int i = 0; i < NUM_LEDS; ++i) {
      
      SingleLed led;
      
      led.color = ColorFromPalette( current_palette_mode_3, color_index_mode_3, 255, NOBLEND);
      led.state = LED_SOLID;
      cube_led.set_led(i, led);
      color_index_mode_3 += 3;
    }
}
//////////////////////////////////////
//                                  //
//            MODE 4                //
//      OLD- PULSE                  //
//////////////////////////////////////
CHSVPalette16 currentPalette;
uint8_t color_index_mode_4;
void initialize_mode_4()
{
  uint8_t color_index_mode_4 = 0;
  fill_solid( currentPalette, 16, COLOR_BLACK);
  tft_select_mode(4,"Blade Runner", "COLORFUL");
    // and set every fourth one to white.
    currentPalette[0] = COLOR_WHITE;
    currentPalette[4] = COLOR_WHITE;
    currentPalette[8] = COLOR_WHITE;
    currentPalette[12] = COLOR_WHITE;
}

void run_mode_4(){

// MODE propagate
  if(since_mode_4 < since_min_mode_4)
    return;
  since_mode_4 = 0;

    for( int i = 0; i < NUM_LEDS; ++i) {
      
      SingleLed led;
      
      led.color = ColorFromPalette( currentPalette, color_index_mode_4, 255, LINEARBLEND);
      led.state = LED_SOLID;
      cube_led.set_led(i, led);
      color_index_mode_4 += 3;
  }
}
//////////////////////////////////////
//                                  //
//            MODE 5                //
//          BLUETOOTH               //
//                                  //
//////////////////////////////////////


void initialize_mode_5()
{
  tft_select_mode(5,"Pick Color BT", "");
  cube_led.set_leds_off();
  Dabble.begin("3D CUBE UAL");     //set bluetooth name of your device
  Serial.println("Configuring Dabble...");
  since_bt = 0;
}

void run_mode_5(){
  if (since_bt < since_max_bt)
  {
    return;
  }
  since_bt = 0;
  Dabble.processInput(); 
  if(Dabble.isAppConnected() == false)
  {
    Serial.println("Waiting BLE connection");
    return;
   }
   
   //ColorDetector.sendSettings(&initialSetup);  
   //upload code and open serial monitor (reset your board once if you cannot see any message printed on Serial Monitor)
  //Dabble.processInput();  //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  unsigned red_color = ColorDetector.getRedColor();
  unsigned blue_color = ColorDetector.getBlueColor();
  unsigned green_color  = ColorDetector.getGreenColor();
  CRGB color_rgb = CRGB(red_color, green_color, blue_color);
  CHSV color_hsv = rgb2hsv_approximate(color_rgb);//CHSV(hue_hsv, sat_hsv, lum_hsv);
  cube_led.set_leds_color_solid(color_hsv);
}

//////////////////////////////////////
//                                  //
//            MODE  6               //
//          TESTING                 //
//                                  //
//////////////////////////////////////
unsigned testing_x, testing_y;
unsigned current_color_mode_6;
void initialize_mode_6()
{
  tft_select_mode(6,"Propagate col", "");
  cube_led.set_leds_off();
  current_color_mode_6 = 0;
}

void run_mode_6(){

/**
	Check status of the column by visual instpection
	@param x				x position in the 2d led map.
	@param y				y position in the 2d led map.
	@param chsv_color 		Led Color to be set in the columns.
	@return The error code.
	*/
  if (since_testing < SEPARATION_MS_TEST)
    return;
  since_testing = 0;
  
  current_color_mode_6 = (current_color_mode_6 + 30) % 255;
  SingleLed led = SingleLed{CHSV(current_color_mode_6, 255 - 10, 255), LED_SOLID};
  if ( cube_led.propagate_column(&led, 1) != 0)
  {
    Serial.println("ERROR propagate columns");
  }   
}
/////////////////////////////////////
//                                  //
//            MODE  7               //
//          DISTANCE SENSOR         //
//                                  //
//////////////////////////////////////

 
float get_min (float a, float b) {
  if ( a < b )
    return a;
 
  else 
    return b;
 
}
 
void initialize_mode_7()
{
  tft_select_mode(7,"Distance", "");
  cube_led.set_leds_off();
}
 
void run_mode_7(){
  float current_value = 0;
  float distance_left = 0;
  float distance_right = 0;
  float ratio;
  //float min_measure;
  //float mid_distance = MAX_DISTANCE / 2;
  float avg_distance; 
 
  unsigned count_left = 0;
  unsigned count_right = 0;
 
  SingleLed current_led = SingleLed{cube_led.get_main_color(), LED_SOLID};
  LED_ZONES config_zones;
 
  
  // Prevents frequent measurements
  if (since_mode_7 < since_min_mode_7)
    return;
  since_mode_7 = 0;
 
  // Measure and average the samples
  for(int i=0; i < NUM_REPEAT_ULTRASONIC_SENSOR; i++)
  {
    current_value = distanceSensor_left.measureDistanceCm();
    // -1 if the distance is to high
    if(current_value > 0)
    {
      distance_left += current_value;
      count_left++;
    }
    current_value = distanceSensor_right.measureDistanceCm();
    if(current_value > 0)
    {
      distance_right += current_value;
      count_right++;
    }
  }
 
  if ( count_left == 0 ) 
    distance_left = MAX_DISTANCE;
 
  if ( count_right == 0 )
    distance_right = MAX_DISTANCE;
 
  if ( distance_left == MAX_DISTANCE && distance_right == MAX_DISTANCE ) {
    Serial.println("Out of range.");
    return;
  }
 
  if ( count_left > 0 )
    distance_left = distance_left / count_left;
 
  if ( count_right > 0 )
    distance_right = distance_right / count_right;
 
  //Zone codification
    // CLOSE ZONE
  if( distance_left <= CLOSE_DISTANCE){
    if(distance_right <= CLOSE_DISTANCE)
    {
      config_zones.x_zone = CENTER;
      config_zones.y_zone = CLOSE;
    }else{
      config_zones.x_zone = LEFT;
      config_zones.y_zone = CLOSE;
    }
  }else if(distance_right <= CLOSE_DISTANCE)
  {
      config_zones.x_zone = RIGHT;
      config_zones.y_zone = CLOSE;
   }else if ( distance_left == MAX_DISTANCE ) 
  {
      config_zones.x_zone = RIGHT;
      config_zones.y_zone = FAR;
  }else if( distance_right == MAX_DISTANCE)
  {
      config_zones.x_zone = LEFT;
      config_zones.y_zone = FAR;
  }else 
  {
 
    avg_distance = ( distance_left + distance_right ) / 2;
 
    if (avg_distance > FAR_DISTANCE) 
    {
      config_zones.y_zone = FAR;
      config_zones.x_zone = CENTER;
    }
    else 
    {
      config_zones.y_zone = MIDDLE;
 
    if ( distance_left - distance_right > DIST_DIFFERENCE )
      config_zones.x_zone = RIGHT;
 
    else if ( distance_right - distance_left > DIST_DIFFERENCE )
      config_zones.x_zone = LEFT;
    else
      config_zones.x_zone = CENTER;
    }
  }

  Serial.print(distance_left);
  Serial.print(", ");
  Serial.print(distance_right);
  Serial.print(" - ");
  Serial.print(config_zones.x_zone);
  Serial.print(", ");
  Serial.print(config_zones.y_zone);
  Serial.println(". ");
 
  if ( ( config_zones.x_zone == previous_zones.x_zone ) && ( config_zones.y_zone == previous_zones.y_zone ) ) {
    //zone check successful! change!
    cube_led.set_leds_off();
    for (int x = 0; x < ZONE_SIZE; x++) {
      for (int y = 0; y < ZONE_SIZE; y++) {
        //x and y are inverted due to led cube position in box!!!
        
        cube_led.set_led_column( y + config_zones.y_zone, x + config_zones.x_zone, &current_led, 1);
      }
    } 
  }
 
  else {
    if ( config_zones.x_zone != previous_zones.x_zone )
      previous_zones.x_zone = config_zones.x_zone;
 
    if ( config_zones.y_zone != previous_zones.y_zone )
      previous_zones.y_zone = config_zones.y_zone;
  }
}
 

//////////////////////////////////////
//                                  //
//            MODE  8               //
//          MPU                     //
//                                  //
//////////////////////////////////////

void initialize_mode_8()
{
  tft_select_mode(8,"Gyro, Acc, Temp", "");
  cube_led.set_leds_off();
}

void run_mode_8(){

    // Not allow very frequenct measurements
    if (since_mode_8 < since_min_mode_8)
      return;
    since_mode_8 = 0;

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

       float ax = a.acceleration.x;
   float ay = a.acceleration.y;
   float az = a.acceleration.z;
   //Calcular los angulos de inclinacion
   float accel_ang_x = atan(ax / sqrt(pow(ay, 2) + pow(az, 2)))*(180.0 / 3.14);
   float accel_ang_y = atan(ay / sqrt(pow(ax, 2) + pow(az, 2)))*(180.0 / 3.14);


    
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    
    // Convert the value to a char array
    char tempString[8];
    dtostrf(ax, 1, 2, tempString);
    client.publish("esp32/ax", tempString);

      dtostrf(ay, 1, 2, tempString);
    client.publish("esp32/ay", tempString);

    
      dtostrf(az, 1, 2, tempString);
    client.publish("esp32/az", tempString);

        dtostrf(accel_ang_x, 1, 2, tempString);
    client.publish("esp32/angx", tempString);

    
        dtostrf(accel_ang_y, 1, 2, tempString);
    client.publish("esp32/angy", tempString);

      
        dtostrf(g.gyro.x, 1, 2, tempString);
    client.publish("esp32/rotx", tempString);

        dtostrf(g.gyro.y, 1, 2, tempString);
    client.publish("esp32/roty", tempString);
    
        dtostrf(g.gyro.z, 1, 2, tempString);
    client.publish("esp32/rotz", tempString);


      dtostrf(temp.temperature, 1, 2, tempString);
    client.publish("esp32/temp", tempString);


    LED_ZONES_3D config_mode_8;
 
      //z zone assign
      if ( az - Z_THRESHOLD > 0 ) //is positive and over threshold
        config_mode_8.z_zone = Z_TOP;
     
      else if ( az + Z_THRESHOLD < 0 ) //is negative and under threshold
        config_mode_8.z_zone = Z_BOT;
     
      else
        config_mode_8.z_zone = Z_MID;
     
      //x zone assign
      if ( accel_ang_x > ANGLE_THRESHOLD )  //is positive (leaning right) and over threshold
        config_mode_8.x_zone = RIGHT;
     
      else if ( accel_ang_x < - ANGLE_THRESHOLD ) //is negative (leaning left) and under threshold
        config_mode_8.x_zone = LEFT;
     
      else 
        config_mode_8.x_zone = CENTER;
     
      //y zone assign
      if ( accel_ang_y > ANGLE_THRESHOLD )  //is positive (leaning front) and over threshold
        config_mode_8.y_zone = CLOSE;
     
      else if ( accel_ang_y < - ANGLE_THRESHOLD ) //is negative (leaning back) and under threshold
        config_mode_8.y_zone = FAR;
     
      else
        config_mode_8.y_zone = MIDDLE;
        SingleLed current_led = SingleLed{cube_led.get_main_color(), LED_SOLID};
        for (int x = 0; x < ZONE_SIZE; x++)
        {
          for (int y = 0; y < ZONE_SIZE; y++)
          {
            unsigned led_id = config_mode_8.z_zone + (LED_Z * ((x + config_mode_8.x_zone) + (y + config_mode_8.y_zone) * LED_X));
            cube_led.set_led( led_id, current_led);
          }
        }

}

//////////////////////////////////////
//                                  //
//            MODE 9                //
//          Rotary Encoder          //
//                                  //
//////////////////////////////////////
int last_value;
unsigned last_millis = 0;
unsigned finish_mode_9;
#define MAX_SECONDS 10
 unsigned maximum_level_mode_9;
unsigned get_level(unsigned speed)
{

}
#define MAX_SPEED_VALUE 1024
#define NUMBER_SPEED_COLOR_LEVELS  3
#define NUMBER_SPEED_LEVELS  (NUMBER_SPEED_COLOR_LEVELS * LED_Z) + 1
void set_level_color(unsigned level)
{
  CHSV level_color[] = {COLOR_OFF, COLOR_YELLOW, COLOR_ORANGE, COLOR_RED};
  if(level > maximum_level_mode_9){
      maximum_level_mode_9 = level;
    }
  // ToDo change to an arbritrary number of LEDS_Z
  if ( level == 0){
    cube_led.set_leds_off();
    Serial.println("PUT MORE EFFORT!!!");
  }else if (level < NUMBER_SPEED_COLOR_LEVELS + 1)
  {
    cube_led.set_level_color_solid(2, level_color[level]);
    cube_led.set_level_color_solid(1, COLOR_OFF);
    cube_led.set_level_color_solid(0, COLOR_OFF);
    Serial.print("ENTER IN FIRST LEVEL, STRENGTH LEVEL: ");
    Serial.println(level);
  }else if(level < (2 * NUMBER_SPEED_COLOR_LEVELS) + 1)
  {
    cube_led.set_level_color_solid(2, COLOR_RED);
    cube_led.set_level_color_solid(1, level_color[level - 4]);
    cube_led.set_level_color_solid(0, COLOR_OFF);
    Serial.print("ENTER IN SECOND LEVEL, STRENGTH LEVEL: ");
    Serial.println(level);
  } else if (level < (3 * NUMBER_SPEED_COLOR_LEVELS) + 1){
    cube_led.set_level_color_solid(2, COLOR_RED);
    cube_led.set_level_color_solid(1, COLOR_RED);
    cube_led.set_level_color_solid(0, level_color[level - 7]);
    Serial.print("ENTER IN THIRD LEVEL, STRENGTH LEVEL: ");
    Serial.println(level);
  } else {
    cube_led.set_level_color_solid(2, COLOR_RED);
    cube_led.set_level_color_solid(1, COLOR_RED);
    cube_led.set_level_color_solid(0, COLOR_RED);
    Serial.print("It has never been happened: - ");
    Serial.println(level);
  }
  }
  bool finish_pride;
void initialize_mode_9()
{
  tft_select_mode(9,"Speed test", "Time:\n 10s");
  finish_mode_9 = false;
  finish_pride = false;
  cube_led.set_leds_off();
  last_millis = 0;
  maximum_level_mode_9 = 0;
  
}

unsigned start_time_seconds;
unsigned last_second_show;


void run_mode_9(){
  if (finish_mode_9 == true)
    {
      if (finish_pride == true)
        pride();
    
    return;
    }
    
    // Not allow very frequenct measurements
    if (since_mode_9 < since_min_mode_9)
      return;
    since_mode_9 = 0;

  //dont print anything unless value changed
  if (!rotaryEncoder.encoderChanged()) {
    return;
  }
  if (last_millis == 0)
  {
      maximum_level_mode_9 = 0;
      last_second_show = MAX_SECONDS;
      start_time_seconds = millis() / 1000;
      last_millis = millis();
  }
  char to_print_mode_9[256];
  int change = ((rotaryEncoder.readEncoder() - last_value));
  unsigned temp_last_value = last_value;
  last_value = rotaryEncoder.readEncoder();
  if (change < 0)
    return;
  Serial.print("Change: ");
  Serial.print(change);
  Serial.print(" Last_value: ");
  Serial.print(last_value);
  Serial.print(" Current value: ");
  Serial.print(rotaryEncoder.readEncoder());
  last_value = rotaryEncoder.readEncoder();

  Serial.print(" Diff Time: ");
  Serial.print((millis() - last_millis));
  Serial.print(" Speed: ");

  unsigned speed = (unsigned) (((float)((float)change / (millis() - last_millis)) / dificulty_level) * MAX_SPEED_VALUE );
  Serial.println(speed);
  unsigned level = map(speed, 0, MAX_SPEED_VALUE / 2, 0, NUMBER_SPEED_LEVELS - 1);
  set_level_color(level);

  if(level > maximum_level_mode_9){
    maximum_level_mode_9 = level;
  }

  unsigned second_passed = (millis()/1000) - start_time_seconds;
  unsigned second_remaining = MAX_SECONDS - second_passed;
  if (second_passed >= MAX_SECONDS)
  {
     
     if(maximum_level_mode_9  >= NUMBER_SPEED_LEVELS)
     {
      pride();
      finish_pride = true;
        set_level_color(maximum_level_mode_9);
       sprintf(to_print_mode_9, "MAXIMUM \n STRENGTH!!! \n", maximum_level_mode_9 + 1);
       tft_pride(to_print_mode_9);
     }else
     {
      set_level_color(maximum_level_mode_9);
       sprintf(to_print_mode_9, "FINISH!!! \n STRENGTH: %u!!!", maximum_level_mode_9 + 1);
       tft_update_description(to_print_mode_9);
     }
    finish_mode_9 = true;
  }else if (second_remaining != last_second_show) {
    sprintf(to_print_mode_9, "Time:\n %d", second_remaining);
    tft_update_description(to_print_mode_9);
    last_second_show = second_remaining;

  }
  last_millis = millis();
}

void initialize_mode_OK()
{

  tft_pride("START \n AGAIN");
  
  cube_led.set_leds_off();
}

void run_mode_OK(){
  pride();
}



char get_key(unsigned ir_code)
{
  for (int i = 0; i < IR_BUTTONS; i++)
  {
    if (ir_codes[i] == ir_code)
    {
      return ir_keys[i];
    }
  }
  return '-';
}

void loop() {

     if (!client.connected()) {
      reconnect();
    }
    client.loop();
    if (irrecv.decode(&results))// Returns 0 if no data ready, 1 if data ready.     
    {     
       irrecv.resume();    
       Serial.println((int)results.value , HEX);
       char key = get_key(results.value );
       switch(key){
            case '1': // MODE 1
            if (selected_mode != 1)
            {
              initialize_mode_1(cube_led.get_main_color());
              Serial.println("Selected Mode 1");
            }
            selected_mode = 1;

            break;
            case '2': // MODE 2
            if (selected_mode != 2)
            {
              initialize_mode_2();
              cube_led.set_leds_off();
              Serial.println("Selected Mode 2");
            }
            selected_mode = 2;
            break;

            case '3': // NUM 3
            if (selected_mode != 3)
            {
              initialize_mode_3();
              Serial.println("Selected Mode 3");
            }
            selected_mode = 3;
            break;
            case '4': // NUM 4
            if (selected_mode != 4)
            {
              initialize_mode_4();
              Serial.println("Selected Mode 4 - PULSE DETECTION");
            }
            selected_mode = 4;
            break;
            case '5': // NUM 4
            if (selected_mode != 5)
            {
              initialize_mode_5();
              Serial.println("Selected Mode 5 - BLUETOOTH");
            }
            selected_mode = 5;
            break;
            case '6': // TEST COLUMNS
            if (selected_mode != 6)
            {
              initialize_mode_6();
              Serial.println("Selected Mode 6 ");
            }
            selected_mode = 6;
            break;                                //REMEMBER ToDo the fifth of november!!!
            case '7': // ULtraSound
            if (selected_mode != 7)
            {
              initialize_mode_7();
              selected_mode = 7;
              Serial.println("Selected Mode 7 ");
            }
            break;                                //REMEMBER ToDo the fifth of november!!!
            case '8': // Sensores
            if (selected_mode != 8)
            {
              initialize_mode_8();
              selected_mode = 8;
              Serial.println("Selected Mode 8 ");
            }
            break;  
            case '9': // SPEED TEST
            if (selected_mode != 9)
            {
              initialize_mode_9();
              Serial.println("Selected Mode 9 ");
            }
            selected_mode = 9;
            break;
            case 'L': // LEFT ARROW
            cube_led.modify_led_colors(COLOR_DECREASE);
            break;
            case 'O': // LEFT ARROW
            if (selected_mode != 99)
            {
              initialize_mode_OK();
              selected_mode = 99;
            }
            break;
            case 'R': // LEFT ARROW
            cube_led.modify_led_colors(COLOR_INCREASE);
            break;

        }
    }    
    
  switch(selected_mode)
  {
    case 1:
    run_mode_1(cube_led.get_main_color());
    break;
    case 2:
    run_mode_2(cube_led.get_main_color(), cube_led.get_alt_color());
    break;
    case 3:
    run_mode_3();
    //Serial.println("Enter Mode 3");
    break;
    case 4:
    run_mode_4();
    break;
    case 5:
    run_mode_5();
    break;
    case 6:
    run_mode_6();
    break;
    case 7:
    run_mode_7();
    break;
    case 8:
    run_mode_8();
    break;
    case 9:
    run_mode_9();
    break;
    case 99:
    run_mode_OK();
    break;
   }
   

  cube_led.led_tick();

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

// MEssage:

  // Feel free to add more if statements to control more GPIOs with MQTT
  if (String(topic) == "esp32/dificulty") {
    char char_vector[10];
    String(topic).toCharArray(char_vector, 10);
    int temp_dificulty = atoi(char_vector);
    if (temp_dificulty > 0 )
    {
      dificulty_level = temp_dificulty;
      }
  }
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/colour") {
       int nd; // , locations
   //hsv(88, 27%, 88%)
   nd = messageTemp.indexOf('(');  //finds location of first ,
   messageTemp = messageTemp.substring( nd + 1);
   Serial.print("Remove until (");
   Serial.println(messageTemp);
   nd = messageTemp.indexOf(',');  //finds location of first ,
   char char_vector[10];
   messageTemp.substring(0, nd).toCharArray(char_vector, 10);
   Serial.print("Character");
   Serial.println(char_vector);
   int hue = atoi(char_vector);
   messageTemp = messageTemp.substring(nd + 2);
   nd = messageTemp.indexOf('%');  //finds location of first ,
   messageTemp.substring(0, nd).toCharArray(char_vector, 10);
   int sat = atoi(char_vector);
   messageTemp = messageTemp.substring(nd + 2);
   nd = messageTemp.indexOf('%');  //finds location of first ,
   messageTemp.substring(0, nd).toCharArray(char_vector, 10);
   int light  = atoi(char_vector);
   Serial.print("hue: ");
   Serial.print(hue);
   Serial.print(" sat: ");
   Serial.print(sat);
    Serial.print(" light: ");
   Serial.println(light);
   cube_led.set_main_color(CHSV(hue * 255 / 360, sat * 255 / 100, light * 255 / 100));
  }
}

void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/colour");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

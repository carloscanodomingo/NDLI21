// IR PIN DEFINE
#define IR_RECV_PIN           14 // the pin where you connect the output pin of IR sensor   

// CUBE LED DEFINES 
#define CUBE_LED_PIN   	      16

// ULTROSONIC DEFINES 
#define SR04_TRIGGER_LEFT_PIN     5
#define SR04_TRIGGER_RIGHT_PIN    13
#define SR04_ECHO_LEFT_PIN    17
#define SR04_ECHO_RIGHT_PIN   19


#define TFT_MOSI_PIN 		 23  // SDA Pin on ESP32
#define TFT_SCLK_PIN		 18  // SCL Pin on ESP32
#define TFT_CS_PIN		 	15  // Chip select control pin
#define TFT_DC_PIN		     2  // Data Command control pin
#define TFT_RST_PIN  		     4  // Reset pin (could connect to RST pin)

#define MCU6050_SCL         22 
#define MCU6050_SCA         21
#define PIN_JOY1_X   12   //(up  down)
#define PIN_JOY1_Y   13  //(left  right)

//Rotary encoder side    MICROCONTROLLER side  
//-------------------    ---------------------------------------------------------------------
//CLK (A pin)            any microcontroler intput pin with interrupt -> in this example pin 32
//DT (B pin)             any microcontroler intput pin with interrupt -> in this example pin 21
//SW (button pin)        any microcontroler intput pin with interrupt -> in this example pin 25
//GND - to microcontroler GND
//VCC                    microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1) 
#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 34
#define ROTARY_ENCODER_BUTTON_PIN 25
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

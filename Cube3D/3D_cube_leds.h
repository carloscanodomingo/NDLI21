/**
  NDLI21 Project TIC019
  Name: 3D_cube_led.h
  Purpose: Packet the interface between main code and Specific FastLED implementation for the 3d cube.

  @author Carlos Cano & Manuel Soler
  @version 0.1 19/09/2021

*/

#include <FastLED.h>
#include <elapsedMillis.h>

#define NO_ERROR          				0
#define ERROR_CUBE_LED          	1
#define ERROR_CUBE_ID         	 	9
#define ERROR_NOT_INIT						3
#define ERROR_COLOR_ERROR 				10
#define ERROR_NUMBER_LED					11
#define CUBE_LED_TYPE  						WS2811
#define CUBE_COLOR_ORDER					BRG
#define MAX_HUE 									255
#define MIN_HUE 									0
#define COLOR_MODIFIER_RATE 			25
#define DECREMENT_LOW_BNRIGHTNESS 10


// NOT here -> CUbe class
#define MILLIS_PER_SECOND   		1000

#define BLINK_SEPARATION_MS 		MILLIS_PER_SECOND / 5
#define FPS 										30
#define SEPARATION_MS_FPS				MILLIS_PER_SECOND / FPS



typedef enum LED_STATE {
	LED_OFF,
	LED_SOLID,
	LED_BLINK,
};

typedef enum LED_BLINK_STATE {
	LED_BLINK_ON,
	LED_BLINK_OFF,
} ;

typedef enum COLOR_MODIFIER {
	COLOR_DECREASE,
	COLOR_INCREASE
} ;
typedef struct SingleLed
{
	CHSV 			color;
	LED_STATE state;

};
const CHSV COLOR_OFF = CHSV(0, 0, 0);
const CHSV COLOR_BLACK = CHSV(0, 0, 0);
const CHSV COLOR_RED = CHSV(HUE_RED , 255, 255);
const CHSV COLOR_GREEN = CHSV(HUE_GREEN , 255, 255);
const CHSV COLOR_BLUE = CHSV(HUE_BLUE, 255, 255);
const CHSV COLOR_AQUA = CHSV(HUE_AQUA , 255, 255);
const CHSV COLOR_PINK = CHSV(HUE_PINK, 255, 255);
const CHSV COLOR_YELLOW = CHSV(HUE_YELLOW , 255, 255);
const CHSV COLOR_ORANGE = CHSV(HUE_ORANGE , 255, 255);
const CHSV COLOR_WHITE = CHSV(0 , 0, 255);
const CHSV COLOR_PURPLE = CHSV(HUE_PURPLE , 255, 255);


const CHSV MAIN_CURRENT_COLOR = COLOR_RED; //CHSV(180, 255 / 2, 255);
const CHSV ALT_CURRENT_COLOR = CHSV(90, 255 / 2, 255);




/**
    Class for containing all the interfaces with the Specific
	cube 3D cube implementation for the NDLI21 project.
 */
class CubeLeds
{
private:


	// PROTECTED
	// Do not modify this variable ultil led_show
	// Stability problem
	CRGB * led_color_rgb;
	// Array with description for every leds in the cube 3d
	CHSV * led_color_hsv;
	// Array with the state description of every leds in the 3d cube.
	LED_STATE * led_state;
	// values to be passed describing the 3D structure of the 3d Cube.
	unsigned LEDS_X, LEDS_Y, LEDS_Z;
	unsigned number_of_leds;
	elapsedMillis since_blink;
	elapsedMillis since_tick;
	elapsedMillis since_testing;
	bool isInit;
	// Globar blink state, it has to be modify just in the show funciton.
	LED_BLINK_STATE current_blink_state;
	// When the color is not representative, this values are used.
	CHSV main_color;
	CHSV alt_color;

public:
	// Only constructor of the cube 3d.
	void begin(unsigned leds_x, unsigned  leds_y, unsigned  leds_z) {
		Serial.println("Cube led constractor start");
		this->LEDS_X = leds_x;
		this->LEDS_Y = leds_y;
		this->LEDS_Z = leds_z;
		this->number_of_leds = leds_x * leds_y * leds_z;
		this->led_color_hsv = new CHSV[this->number_of_leds];
		this->led_color_rgb = new CRGB[this->number_of_leds];
		this->led_state = new LED_STATE[this->number_of_leds];
		this->isInit = true;
		this->current_blink_state = LED_BLINK_OFF;
		this->main_color = MAIN_CURRENT_COLOR;
		this->alt_color = ALT_CURRENT_COLOR;
		this->since_blink = 0;
		this->since_tick = 0;
		this->since_testing = 0;
		FastLED.addLeds<CUBE_LED_TYPE, CUBE_LED_PIN, CUBE_COLOR_ORDER>(this->led_color_rgb, this->number_of_leds).setCorrection( TypicalLEDStrip );
		FastLED.setBrightness(100);

		this->set_leds_off();
		Serial.println("Cube led constructor finish");
	}
	CHSV get_main_color()
	{
		if (!this->isInit)
			return COLOR_OFF;
		return this->main_color;
	}
  void set_main_color(CHSV led_colour)
  {

    this->main_color = led_colour;
  }
	CHSV get_alt_color()
	{
		if (!this->isInit)
			return COLOR_OFF;
		return this->alt_color;
	}
	void modify_led_colors(enum COLOR_MODIFIER color_modifier)
	{
		if (!this->isInit)
			return;

		switch (color_modifier)
		{
		case COLOR_DECREASE:
			this->main_color.hue = ((this->main_color.hue + MAX_HUE) - COLOR_MODIFIER_RATE) % MAX_HUE;
			this->alt_color.hue  = ((this->alt_color.hue + MAX_HUE) - COLOR_MODIFIER_RATE) 	% MAX_HUE;
			break;
		case COLOR_INCREASE:
			this->main_color.hue = (this->main_color.hue + COLOR_MODIFIER_RATE) % MAX_HUE;
			this->alt_color.hue  = (this->alt_color.hue + COLOR_MODIFIER_RATE) 	% MAX_HUE;
			break;
		}
	}
	/**
	Set a specifc led with a hsv color
	The color is storage in the interfaces hsv array.

	@param led_id 			Position in the led array.
	@param chsv_color 		Led Color to be set.
	@param LED_STATE 		Led State to be set.
	@return The error code.
	*/
	unsigned set_led(unsigned led_id, SingleLed led)
	{
		unsigned error_code = check_id(led_id);
		if (error_code != NO_ERROR)
		{
			return error_code;
		}
		this->led_color_hsv[led_id] = led.color;
		this->led_state[led_id]     = led.state;

		return NO_ERROR;
	}

  unsigned get_led(unsigned led_id, SingleLed * led )
	{
		unsigned error_code = check_id(led_id);
		if (error_code != NO_ERROR)
		{
			return error_code;
		}
    led->color = this->led_color_hsv[led_id],
    led->state = this->led_state[led_id];

		return NO_ERROR;
	}
	/**
	Propgate leds from the beggining. The new led in the first position has to be
	pass to the funcion
	@param chsv_color 		Led Color to be set.
	@param LED_STATE 		Led State to be set.
	@return The error code.
	*/
	unsigned propagate_led(SingleLed led)
	{
		Serial.println(this->number_of_leds);
		for (int led_id = this->number_of_leds - 1; led_id >= 0; led_id--)
		{

			this->led_color_hsv[led_id + 1] = this->led_color_hsv[led_id];
			this->led_state[led_id + 1] = this->led_state[led_id];
		}
		this->led_color_hsv[0] = led.color;
		this->led_state[0] = led.state;
		return NO_ERROR;
	}

	/**
	Propgate leds from the beggining. The new led in the first position has to be
	pass to the funcion
	@param chsv_color 		Led Color to be set.
	@param LED_STATE 		Led State to be set.
	@return The error code.
	*/
	unsigned propagate_column(SingleLed * led, unsigned number_of_leds)
	{
		unsigned error_code;
		if (number_of_leds == 1)
		{
			for (int z = 0; z < this->LEDS_Z ; z++)
			{
				Serial.println("Propagating leds");
				error_code = this->propagate_led(led[0]);
				if (error_code != NO_ERROR)
				{
					return error_code;
				}
			}
		} else if (number_of_leds == this->LEDS_Z)
		{
			for (int z = 0; z < this->LEDS_Z ; z++)
			{
				error_code = this->propagate_led(led[z]);
				if (error_code != NO_ERROR)
				{
					return error_code;
				}
			}
		} else {
			return ERROR_NUMBER_LED;
		}
	}
	 // Can be done just using % and accept any albitrary number of leds
	unsigned propagate_row(SingleLed * led, unsigned number_of_leds)
	{
		unsigned error_code;
		if (number_of_leds == 1)
		{
			// Just pass one led
			for (int y = 0; y < this->LEDS_Y ; y++)
			{
				error_code = this->propagate_column(led, number_of_leds);
				if (error_code != NO_ERROR)
				{
					return error_code;
				}
			}
			// pass one column
		} else if (number_of_leds == this->LEDS_Z)
		{
			for (int y = 0; y < this->LEDS_Y ; y++)
			{
				for (int z = 0; z < this->LEDS_Z ; z++)
				{
					error_code = this->propagate_led(led[z]);
					if (error_code != NO_ERROR)
					{
						return error_code;
					}
				}
			}
			// pass one row
		} else if (number_of_leds == this->LEDS_Z * LEDS_Y)
		{
			for (int y = 0; y < this->LEDS_Y ; y++)
			{
				for (int z = 0; z < this->LEDS_Z ; z++)
				{
					error_code = this->propagate_led(led[(y * this->LEDS_Z) + z]);
					if (error_code != NO_ERROR)
					{
						return error_code;
					}
				}
			}
		} else {
			return ERROR_NUMBER_LED;
		}
	}
	/**
	Set a specifc column with a hsv color for the whoile column.
	The color is storage in the interfaces hsv array.

	@param x				x position in the 2d led map.
	@param y				y position in the 2d led map.
	@param chsv_color 		Led Color to be set in the column.
	@param LED_STATE 		Led State to be set in the column.
	@return The error code.
	*/
	unsigned set_led_column(unsigned x, unsigned y, SingleLed * led, unsigned number_of_leds)
	{
		if (!this->isInit)
			return ERROR_NOT_INIT;

		unsigned led_id = this->LEDS_Z * (x +  (y * this->LEDS_X));
		unsigned error_code;

		if (number_of_leds == 1)
		{
			for (int z = 0; z < this->LEDS_Z ; z++)
			{
				error_code = this->set_led(led_id + z, led[0]);
				if (error_code != NO_ERROR)
				{
					return error_code;
				}
			}
		} else if (number_of_leds == this->LEDS_Z)
		{
			for (int z = 0; z < this->LEDS_Z ; z++)
			{
				error_code = this->set_led(led_id + z, led[z]);
				if (error_code != NO_ERROR)
				{
					return error_code;
				}
			}
		} else {
			return ERROR_NUMBER_LED;
		}
		return NO_ERROR;
	}

	/**
	Set column blinking and low the brightness
	@param x				x position in the 2d led map.
	@param y				y position in the 2d led map.
	@param chsv_color 		Led Color to be set in the columns.
	@return The error code.
	*/
	unsigned select_column(unsigned x, unsigned y, CHSV hsv_color)
	{
		if (!this->isInit)
			return ERROR_NOT_INIT;
		unsigned ret;

		// Set central Column
		SingleLed led;
		led.color = hsv_color;
		led.state = LED_SOLID;
		if (set_led_column(x, y, &led, 1) != NO_ERROR)
			return ERROR_CUBE_LED;

		// Set surronded led if off
		if (x == 0)
		{
			set_column_blink_check_off( x + 1, y, hsv_color);
		} else if (x == this->LEDS_X - 1) {
			set_column_blink_check_off( x - 1, y, hsv_color);
		} else {
			set_column_blink_check_off( x - 1, y, hsv_color);
			set_column_blink_check_off( x + 1, y, hsv_color);
		}
		// Set surronded led if off
		if (y == 0)
		{
			set_column_blink_check_off( x, y + 1, hsv_color);
		} else if (y == this->LEDS_Y - 1) {
			set_column_blink_check_off( x, y - 1, hsv_color);
		} else {
			set_column_blink_check_off( x, y - 1, hsv_color);
			set_column_blink_check_off( x, y + 1, hsv_color);
		}

		return NO_ERROR;
	}
	/**
	Set all leds with the same color and same state
	@param chsv_color 		Led Color to be set in the columns.
	@param LED_STATE 		Led State to be set in the column.
	@return The error code.
	*/
	unsigned set_leds_color(SingleLed led)
	{
		if (!this->isInit)
			return ERROR_NOT_INIT;
		for (int led_id = 0; led_id < this->number_of_leds; led_id++)
		{

			if (this->set_led(led_id, led))
			{
				return ERROR_CUBE_LED;
			}
		}
		return NO_ERROR;
	}
	/**
	Set to OFF to all leds

	@return The error code.
	*/
	unsigned set_leds_off()
	{
		SingleLed led = {COLOR_OFF, LED_OFF};
		if (set_leds_color(led) != 0)
			return ERROR_CUBE_LED;

		return NO_ERROR;
	}
  	/**
	Set all leds with the same color and same state
	@param chsv_color 		Led Color to be set in the columns.
	@param LED_STATE 		Led State to be set in the column.
	@return The error code.
	*/
	unsigned set_level_color_solid(unsigned z, CHSV color)
	{
    int correct_z = (this->LEDS_Z - 1) - z;
		SingleLed led = SingleLed{color, LED_SOLID};
		for (int led_id = 0; led_id < this->number_of_leds; led_id++)
		{
      if(( led_id % this->LEDS_Z) == z) {
        Serial.print("Set led: ");
        Serial.print(led_id);
        if (this->set_led(led_id, led))
        {
          return ERROR_CUBE_LED;
        }
      }
		}
		return NO_ERROR;
	}
	/**
	Set all leds with the same color and same state
	@param chsv_color 		Led Color to be set in the columns.
	@param LED_STATE 		Led State to be set in the column.
	@return The error code.
	*/
	unsigned set_leds_color_solid(CHSV color)
	{
		SingleLed led = SingleLed{color, LED_SOLID};
		for (int led_id = 0; led_id < this->number_of_leds; led_id++)
		{
			if (this->set_led(led_id, led))
			{
				return ERROR_CUBE_LED;
			}
		}
		return NO_ERROR;
	}

	void led_tick()
	{
		if (!this->isInit)
			return;

		bool change_blink = false;

		// Update leds if the time per frame current is higher than the stablished
		if ( since_tick < SEPARATION_MS_FPS)
		{
			return;
		}
		// Restar current time per frame
		since_tick = 0;

		// check whether is time to blink blinked leds
		if (since_blink > BLINK_SEPARATION_MS)
		{
			since_blink = 0;
			change_blink = true;
			switch (this->current_blink_state)
			{
			case LED_BLINK_ON:
				this->current_blink_state  = LED_BLINK_OFF;
				break;
			case LED_BLINK_OFF:
				this->current_blink_state  = LED_BLINK_ON;
				break;
			}
		}

		// Copy the CHSV in RGB output LEDS
		for (int led_id = 0; led_id < this->number_of_leds; led_id++)
		{
			switch (this->led_state[led_id])
			{
			// Set black color to off leds
			case LED_OFF:
				this->led_color_rgb[led_id] = COLOR_OFF;
				break;
			case LED_SOLID:

				this->led_color_rgb[led_id] = this->led_color_hsv[led_id];
				break;
			// Alternate blink leds
			case LED_BLINK:
				if (change_blink == true)
				{
					switch (this->current_blink_state)
					{
					case LED_BLINK_ON:
						this->led_color_rgb[led_id] = this->led_color_hsv[led_id];

						break;
					case LED_BLINK_OFF:
						this->led_color_rgb[led_id] = COLOR_OFF;

						break;
					}
					break;
				}
			}
		}
		// Use FastLED library to send data to LEDS
		FastLED.show();
	}
private:
	/**
	Check if the led id is valid
	@param led_id				Position in the led array.
	@return The error code.
	*/
	inline unsigned check_id(unsigned led_id)
	{
		unsigned error_id = NO_ERROR;

		if (led_id < 0 || led_id >= this->number_of_leds)
		{
			char cadena[24];
			sprintf(cadena, "Error in id: %u", led_id);
			Serial.println(cadena);
			return ERROR_CUBE_ID;
		}
		return error_id;
	}

	/**
	Check if the led id is valid
	@param led_id				Position in the led array.
	@param chsv_color 		Led Color to be get.
	@return The error code.
	*/
	unsigned get_color(unsigned led_id, CHSV * hsv_color)
	{
		unsigned ret = check_id(led_id);
		if (ret != 0)
		{
			return ret;
		}
		memcpy(hsv_color, &(this->led_color_hsv[led_id]), sizeof(CHSV));
		//*color = this->led_color_hsv[led_id];
		return NO_ERROR;
	}
  LED_STATE get_led_column_state(unsigned x, unsigned y)
  {
    return this->led_state[((x + y * this->LEDS_Y ) * this->LEDS_Z)];
  }
	/**
	Set column blinking and low the brightness
	@param x				x position in the 2d led map.
	@param y				y position in the 2d led map.
	@param chsv_color 		Led Color to be set in the column.
	@return The error code.
	*/
	unsigned set_column_blink_check_off(unsigned x, unsigned y, CHSV color)
	{
		SingleLed led;
		led.color = color;

		led.state = LED_BLINK;
    if (get_led_column_state(x,y) == LED_OFF)
    {
      unsigned ret = set_led_column(x, y, &led, 1);
      if (ret != 0)
      {
        return ret;
      }
    }
		return NO_ERROR;
	}

	// Function to interface with the temporary hsv array
	/**
	Set led intensity
	@param led_id				Position in the led array.
	@param lvl_intensity			value from 0 to 100, brightness
	@param chsv_color 		Led Color to be set in the column.
	@return The error code.
	*/
	unsigned set_intensity(unsigned led_id, unsigned lvl_intensity)
	{
		unsigned ret;
		if (lvl_intensity < 0 || lvl_intensity > 100)
		{
			Serial.print("lvl intensity not correct ");
			Serial.println(lvl_intensity);
			return ERROR_COLOR_ERROR;
		}
		ret = check_id(led_id);
		if (ret != 0)
			return ret;

		this->led_color_hsv[led_id].value = (lvl_intensity * 255) / 100;
		return NO_ERROR;
	}

};

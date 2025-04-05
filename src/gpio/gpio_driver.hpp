#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

#include <iostream>
#include <vector>

#include "../helpers/json_nlohmann.hpp"
using Json_de = nlohmann::json;
#define MAX_PWM 1024 // The user's desired input scale and preferred PWM range


namespace de
{
namespace gpio
{

typedef enum {
    GENERIC     = 0,
    SYSTEM      = 1,
    COUNT       = 2   // Total number of valid GPIO types
} ENUM_GPIO_TYPE;


#ifdef TEST_MODE_NO_WIRINGPI_LINK
#define INPUT 0
#define OUTPUT 1
#define PWM_OUTPUT 2
#endif

/**
    * 
    * *    INPUT			        0
    * *    OUTPUT			        1
    * *    PWM_OUTPUT		        2
    * *    PWM_MS_OUTPUT	        8
    * *    PWM_BAL_OUTPUT           9
    * *    GPIO_CLOCK		        3
    * *    SOFT_PWM_OUTPUT		    4
    * *    SOFT_TONE_OUTPUT	        5
    * *    PWM_TONE_OUTPUT		    6
    * *    PM_OFF		            7   // to input / release line
    *
* **/
typedef struct GPIO{
        uint pin_number;
        uint pin_mode;
        uint pin_value;
        uint pin_pwm_width;
        ENUM_GPIO_TYPE gpio_type;
        std::string pin_name;
    } GPIO;


class CGPIODriver
    {
        public:

            static CGPIODriver& getInstance()
            {
                static CGPIODriver instance;

                return instance;
            }

            CGPIODriver(CGPIODriver const&)              = delete;
            void operator=(CGPIODriver const&)           = delete;

        
        private:

            CGPIODriver() 
            {

            }

            
        public:
            
            ~CGPIODriver ()
            {

            }
        

        public:

            bool init();
            bool uninit();

        public:

            void configurePort (const GPIO & gpio);
            void setPinMode (uint pin_number, uint pin_mode);
            int readPin (uint pin_number);
            void writePin (uint pin_number, uint pin_value);
            void writePWM(const uint pin_number, double freq, uint pin_pwm_width);

            const std::vector<GPIO> getGPIOStatus () const; 

            const GPIO* getGPIOByNumber (uint pin_number) const;
            const GPIO* getGPIOByName (const std::string& pin_name) const;

            void changeGPIOByNumber (uint pin_number, uint pin_value, uint pin_pwm_width);
            
            
        private:

            void removeGPIOByNumber (uint pin_number);
            bool initGPIOFromConfigFile();

            GPIO* _getGPIOByNumber (uint pin_number) const;
            GPIO* _getGPIOByName (const std::string& pin_name) const;

        private:

            std::vector<GPIO> m_gpio_array;

            // Base clock frequency for Raspberry Pi PWM (adjust if different)
            const uint32_t baseClock = 19200000;
            // Hardware limits for the clock divisor (adjust if different)
            const uint32_t MIN_PWM_CLOCK_DIVISOR = 2;
            const uint32_t MAX_PWM_CLOCK_DIVISOR = 4095;
            // Although MAX_PWM is 1024 for input, hardware might support larger range
            // Set this to the actual max supported by pwmSetRange if known and > 1024
            // If unsure, keeping it MAX_PWM limits low frequency but ensures compatibility
            const uint32_t MAX_HW_PWM_RANGE = 4096; // Example: Common max range
            
    };
}
}


#endif

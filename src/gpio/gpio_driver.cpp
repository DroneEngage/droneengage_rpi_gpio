#include <iostream>

#include "../helpers/colors.hpp"
#include "gpio_driver.hpp"
#include "../de_common/configFile.hpp"
#include <wiringPi.h>



bool de::gpio::CGPIODriver::init()
{
    wiringPiSetup () ;
  
    const Json_de& m_jsonConfig = de::CConfigFile::getInstance().GetConfigJSON();
    
    
    
    std::cout << _LOG_CONSOLE_TEXT <<  "Reading Pins field: " << _NORMAL_CONSOLE_TEXT_ << std::endl;


    if (!m_jsonConfig.contains("pins"))
    {
        std::cout << _INFO_CONSOLE_BOLD_TEXT <<  "No pins field in config file to preconfigure!" << _NORMAL_CONSOLE_TEXT_ << std::endl;
        return true;
    }

    const Json_de  pins = m_jsonConfig["pins"];

    
    size_t pinCount = pins.size();
    if (pinCount <= 0)
    {
        std::cout << _INFO_CONSOLE_BOLD_TEXT <<  "No pins to preconfigure!" << _NORMAL_CONSOLE_TEXT_ << std::endl;
    }

    for (const auto& pin : pins) {
        int pinNumber = pin[0];         // First element
        int pinMode = pin[1];           // Second element
        int pinState = pin[2];          // Third element
        
        pinMode (pinNumber, pinMode);
        digitalWrite (pinNumber, pinState);

        // Output the values
        std::cout << _SUCCESS_CONSOLE_BOLD_TEXT_ << "Pin Number: " << _INFO_CONSOLE_BOLD_TEXT << pinNumber 
                  << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Mode: " << _INFO_CONSOLE_BOLD_TEXT  << pinMode 
                  << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", State: " << _INFO_CONSOLE_BOLD_TEXT << pinState << std::endl;
    }

    return true;
}

bool de::gpio::CGPIODriver::uninit()
{

}
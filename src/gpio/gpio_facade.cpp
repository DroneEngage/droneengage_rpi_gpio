#include <iostream>
#include "../helpers/colors.hpp"
#include "../helpers/helpers.hpp"
#include "../defines.hpp"
#ifdef TEST_MODE_NO_WIRINGPI_LINK

#else
#include <wiringPi.h>
#endif

#include "../helpers/helpers.hpp"
#include "gpio_facade.hpp"
#include "gpio_driver.hpp"

using namespace de::gpio;

void CGPIO_Facade::API_sendGPIOStatus(const std::string&target_party_id, const bool internal) const
{
    CGPIODriver& cGPIODriver  = CGPIODriver::getInstance();
    
    const std::vector<GPIO> gpios = cGPIODriver.getGPIOStatus();
    
    // Create a JSON array
    Json_de json_array = Json_de::array();


    for (size_t i = 0; i < gpios.size(); ++i) {
        const GPIO& gpio = gpios[i];

        Json_de json_gpio = {
            {"b", gpio.pin_number},
            {"m", gpio.pin_mode},
            {"t", gpio.gpio_type},
            {"v", gpio.pin_value}
        };
        
        std::cout << json_gpio.dump() << std::endl;
        if (!gpio.pin_name.empty())
        {
            json_gpio["n"] =  gpio.pin_name;
        }
        
        if (gpio.pin_mode == PWM_OUTPUT)
        {
            json_gpio["d"] =  gpio.pin_pwm_width;
        }
        
        json_array.push_back(json_gpio);
    }
    
    Json_de jMsg = 
        {
            {"a", GPIO_ACTION_INFO},
            {"s", json_array}
        };

    #ifdef DDEBUG
        std::cout << "XXXXXXXXXXXXXXXXXXXXX" << jMsg.dump() << std::endl;
    #endif
    
    m_module.sendJMSG (target_party_id, jMsg, TYPE_AndruavMessage_GPIO_STATUS,  internal);
    
}



void CGPIO_Facade::API_sendSingleGPIOStatus(const std::string&target_party_id, const GPIO& gpio, const bool internal) const
{
    CGPIODriver& cGPIODriver  = CGPIODriver::getInstance();
    
    const std::vector<GPIO> gpios = cGPIODriver.getGPIOStatus();
    
    // Create a JSON array
    Json_de json_array = Json_de::array();

    Json_de json_gpio = {
        {"b", gpio.pin_number},
        {"m", gpio.pin_mode},
        {"t", gpio.gpio_type},
        {"v", gpio.pin_value}
    };
        
    std::cout << json_gpio.dump() << std::endl;
    if (!gpio.pin_name.empty())
    {
        json_gpio["n"] =  gpio.pin_name;
    }

    if (gpio.pin_mode == PWM_OUTPUT)
    {
        json_gpio["d"] =  gpio.pin_pwm_width;
    }
        
    json_array.push_back(json_gpio);
    
    Json_de jMsg = 
        {
            {"a", GPIO_ACTION_INFO},
            {"s", json_array}
        };

    #ifdef DDEBUG
        std::cout << "XXXXXXXXXXXXXXXXXXXXX" << jMsg.dump() << std::endl;
    #endif
    
    m_module.sendJMSG (target_party_id, jMsg, TYPE_AndruavMessage_GPIO_STATUS,  internal);
    
}



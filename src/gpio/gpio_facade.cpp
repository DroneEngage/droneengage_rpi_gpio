#include <iostream>
#include "../helpers/colors.hpp"
#include "../helpers/helpers.hpp"
#include "../defines.hpp"

#include "../helpers/helpers.hpp"
#include "gpio_driver.hpp"
#include "gpio_facade.hpp"

using namespace de::gpio;

void CGPIO_Facade::API_sendGPIOStatus(const std::string&target_party_id) const
{
    CGPIODriver& cGPIODriver  = CGPIODriver::getInstance();
    
    const std::vector<GPIO> gpios = cGPIODriver.getGPIOStatus();
    
    // Create a JSON array
    Json_de json_array = Json_de::array();


    for (size_t i = 0; i < gpios.size(); ++i) {
        const GPIO& gpio = gpios[i];

        Json_de json_gpio = {
            {"active", gpio.active},
            {"g", gpio.pin_number},
            {"m", gpio.pin_mode},
            {"v", gpio.pin_value}
        };

        if (!gpio.pin_name.empty())
        {
            json_gpio["n"] =  gpio.pin_name;
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
    
    m_module.sendJMSG (target_party_id, jMsg, TYPE_AndruavMessage_GPIO_ACTION,  true);
    
}



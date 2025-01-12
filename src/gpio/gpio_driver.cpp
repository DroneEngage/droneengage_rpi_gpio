#include <iostream>
#ifdef TEST_MODE_NO_WIRINGPI_LINK
void wiringPiSetup(){}
#define OUTPUT 0
#define INPUT 1
#else
#include <wiringPi.h>
#endif
#include "../helpers/colors.hpp"
#include "../de_common/configFile.hpp"

#include "gpio_driver.hpp"
#include "gpio_facade.hpp"



using namespace de::gpio;

bool CGPIODriver::initGPIOFromConfigFile()
{

    try
    {
    
        const Json_de& m_jsonConfig = de::CConfigFile::getInstance().GetConfigJSON();
        
        std::cout << _LOG_CONSOLE_TEXT <<  "Reading Pins field: " << _NORMAL_CONSOLE_TEXT_ << std::endl;


        if (!m_jsonConfig.contains("pins"))
        {
            std::cout << _INFO_CONSOLE_BOLD_TEXT <<  "No pins field in config file to preconfigure!" << _NORMAL_CONSOLE_TEXT_ << std::endl;
            return true;
        }

        const Json_de  pins = m_jsonConfig["pins"];

        
        size_t pin_count = pins.size();
        if (pin_count <= 0)
        {
            std::cout << _INFO_CONSOLE_BOLD_TEXT <<  "No pins to preconfigure!" << _NORMAL_CONSOLE_TEXT_ << std::endl;
        }

        for (const auto& pin : pins) {
            GPIO gpio;
            
            gpio.active = true;
            gpio.pin_number = pin["gpio"].get<int>();         // First element
            gpio.pin_mode = pin["mode"].get<int>();           // Second element
            gpio.pin_value =  0;
            gpio.pin_name = "";

            if (pin.contains("value") && (gpio.pin_mode != INPUT))
            {
                gpio.pin_value = pin["value"];          // Third element
            }

            if (pin.contains("name"))
            {
                gpio.pin_name = pin["name"].get<std::string>();          // Third element
            }
            
            configurePort(gpio);
            
        }

        return true;

    }
    catch(const std::exception& e)
    {
        return false;
    }
    
}


void CGPIODriver::configurePort(const GPIO & gpio)
{
    // remove node if exists from list.
    removeGPIOByNumber(gpio.pin_number);

    setPinMode (gpio.pin_number, gpio.pin_mode);

    if (gpio.pin_mode == OUTPUT)
    {
        writePin(gpio.pin_number, gpio.pin_value);
    }

    // add node to list.
    m_gpio_array.push_back(gpio);

    // Output the values
    std::cout << _SUCCESS_CONSOLE_BOLD_TEXT_ << "Pin Number: " << _INFO_CONSOLE_BOLD_TEXT << gpio.pin_number 
                    << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Mode: " << _INFO_CONSOLE_BOLD_TEXT  << gpio.pin_mode 
                    << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Initial Value: " << _INFO_CONSOLE_BOLD_TEXT << gpio.pin_value 
                    << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Global Name: " << _INFO_CONSOLE_BOLD_TEXT  << gpio.pin_name 
                    << std::endl;
}


bool CGPIODriver::init()
{
    m_gpio_array.clear();

    wiringPiSetup () ;
  
    initGPIOFromConfigFile();
    
    
    return true;
}

bool CGPIODriver::uninit()
{

}

void CGPIODriver::setPinMode (uint pin_number, uint pin_mode)
{
    #ifdef TEST_MODE_NO_WIRINGPI_LINK
    std::cout << _INFO_CONSOLE_TEXT << ":setPinMode:pin_number" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _INFO_CONSOLE_TEXT << ":pin_number:" << _LOG_CONSOLE_BOLD_TEXT << pin_mode << _NORMAL_CONSOLE_TEXT_ << std::endl;
    return ;
    #else
    pinMode (pin_number, pin_mode);
    #endif
}
            
int CGPIODriver::readPin(uint pin_number)
{
    const GPIO* gpio = getGPIOByNumber (pin_number);
    
    if ((gpio == nullptr) || (gpio->pin_mode != INPUT))return -1;

    #ifdef TEST_MODE_NO_WIRINGPI_LINK
    std::cout << _INFO_CONSOLE_TEXT << ":readPin:" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _NORMAL_CONSOLE_TEXT_ << std::endl;
    return 0; // Emulation
    #else
    return digitalRead (pin_number);
    #endif
    
}

void CGPIODriver::writePin(uint pin_number, uint pin_value)
{
    #ifdef TEST_MODE_NO_WIRINGPI_LINK
    std::cout << _INFO_CONSOLE_TEXT << ":writePin:" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _INFO_CONSOLE_TEXT << ":pin_value:" << _LOG_CONSOLE_BOLD_TEXT << pin_value << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #else
    digitalWrite (pin_number, pin_value);
    #endif
    
}


const std::vector<GPIO> CGPIODriver::getGPIOStatus() const
{
    return m_gpio_array;
}

// Function to get GPIO record by pin_name
const GPIO* CGPIODriver::getGPIOByName(const std::string& pin_name) const {
    
    #ifdef TEST_MODE_NO_WIRINGPI_LINK
    std::cout << _INFO_CONSOLE_TEXT << ":getGPIOByName:" << _LOG_CONSOLE_BOLD_TEXT << pin_name << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif

    if (pin_name == "") return nullptr;

    for (const auto& gpio : m_gpio_array) {
        if (gpio.pin_name == pin_name) {
            return const_cast<GPIO*>(&gpio); // Return the matched GPIO record
        }
    }
    return nullptr; // Return nullptr if not found
}

// Function to get GPIO record by pin_number
const GPIO* CGPIODriver::getGPIOByNumber(uint pin_number) const {
    
    #ifdef TEST_MODE_NO_WIRINGPI_LINK
    std::cout << _INFO_CONSOLE_TEXT << ":getGPIOByNumber:pin_number:" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif
    
    for (const auto& gpio : m_gpio_array) {
        if (gpio.pin_number == pin_number) {
            return const_cast<GPIO*>(&gpio); // Return the matched GPIO record
        }
    }
    return nullptr; // Return nullptr if not found
}


void CGPIODriver::removeGPIOByNumber (uint pin_number)
{
    #ifdef TEST_MODE_NO_WIRINGPI_LINK
    std::cout << _INFO_CONSOLE_TEXT << ":removeGPIOByNumber:pin_number:" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif
    
    for (auto it = m_gpio_array.begin(); it != m_gpio_array.end(); ++it) {
        if (it->pin_number == pin_number) {
            m_gpio_array.erase(it); // Remove the matched GPIO record
            return ;
        }
    }
    return ;
}
            

        
            
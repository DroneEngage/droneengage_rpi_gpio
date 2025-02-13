#include <iostream>
#ifdef TEST_MODE_NO_WIRINGPI_LINK
void wiringPiSetup(){}
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
            
            gpio.pin_number = pin["gpio"].get<int>();         // First element
            gpio.pin_mode = pin["mode"].get<int>();           // Second element
            gpio.pin_value =  0;
            gpio.pin_name = "";
            gpio.gpio_type = ENUM_GPIO_TYPE::GENERIC;

            if (pin.contains("gpio_type"))
            {
                gpio.gpio_type = static_cast<de::gpio::ENUM_GPIO_TYPE>(pin["gpio_type"]);
            }
            
            if (pin.contains("value") && (gpio.pin_mode != INPUT))
            {
                gpio.pin_value = pin["value"];          
            }

            if (pin.contains("width"))
            {
                gpio.pin_pwm_width = pin["width"];
            }

            if (pin.contains("name"))
            {
                gpio.pin_name = pin["name"].get<std::string>();
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

    else if (gpio.pin_mode == PWM_OUTPUT)
    {
        writePWM(gpio.pin_number, gpio.pin_value, gpio.pin_pwm_width);
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
    return true;
}

void CGPIODriver::setPinMode (uint pin_number, uint pin_mode)
{
    #ifdef DEBUG
    std::cout << _INFO_CONSOLE_TEXT << ":setPinMode:pin_number" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _INFO_CONSOLE_TEXT << ":pin_number:" << _LOG_CONSOLE_BOLD_TEXT << pin_mode << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif
    
    #ifndef TEST_MODE_NO_WIRINGPI_LINK
    pinMode (pin_number, pin_mode);
    if (pin_mode == OUTPUT)
    {
        pullUpDnControl(pin_number, PUD_DOWN);
    }

    pullUpDnControl(pin_number, PUD_UP); // Enable pull-up resistor on button

    #endif
}
            
int CGPIODriver::readPin(uint pin_number)
{
    const GPIO* gpio = getGPIOByNumber (pin_number);
    
    if ((gpio == nullptr) || (gpio->pin_mode != INPUT))return -1;
    if (gpio) 
    {
        #ifdef DEBUG
        std::cout << _INFO_CONSOLE_TEXT << ":readPin:" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _NORMAL_CONSOLE_TEXT_ << std::endl;
        #endif

        #ifndef TEST_MODE_NO_WIRINGPI_LINK
        return digitalRead (pin_number);
        #endif
    }
}

void CGPIODriver::writePin(uint pin_number, uint pin_value)
{
    const GPIO* gpio = getGPIOByNumber(pin_number);
    if (gpio) 
    {
        changeGPIOByNumber (pin_number, pin_value, gpio->pin_pwm_width);
        
        #ifdef DEBUG
        std::cout << _INFO_CONSOLE_TEXT << ":writePin:" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _INFO_CONSOLE_TEXT << ":pin_value:" << _LOG_CONSOLE_BOLD_TEXT << pin_value << _NORMAL_CONSOLE_TEXT_ << std::endl;
        #endif
        #ifndef TEST_MODE_NO_WIRINGPI_LINK
        digitalWrite (pin_number, pin_value);
        #endif
    }
}


const std::vector<GPIO> CGPIODriver::getGPIOStatus() const
{
    return m_gpio_array;
}

// Function to get GPIO record by pin_name
const GPIO* CGPIODriver::getGPIOByName(const std::string& pin_name) const 
{
    return _getGPIOByName (pin_name);
}

// Function to get GPIO record by pin_number
const GPIO* CGPIODriver::getGPIOByNumber(uint pin_number) const 
{
    
    return _getGPIOByNumber (pin_number);
}


void CGPIODriver::changeGPIOByNumber (uint pin_number, uint pin_value, uint pin_pwm_width) 
{
    GPIO* gpio = _getGPIOByNumber (pin_number);
    if (gpio)
    {
        gpio->pin_value = pin_value;
        gpio->pin_pwm_width = pin_pwm_width;
    }
}

GPIO* CGPIODriver::_getGPIOByName (const std::string& pin_name) const
{
    #ifdef DEBUG
    std::cout << _INFO_CONSOLE_TEXT << ":getGPIOByName:" << _LOG_CONSOLE_BOLD_TEXT << pin_name << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif

    if (pin_name == "") return nullptr;

    for (const auto& gpio : m_gpio_array) {
        if (gpio.pin_name == pin_name) {
            return (GPIO*)(&gpio); // Return the matched GPIO record
        }
    }
    return nullptr; // Return nullptr if not found
}


GPIO* CGPIODriver::_getGPIOByNumber (uint pin_number) const
{
    #ifdef DEBUG
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
    #ifdef DEBUG
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

void CGPIODriver::writePWM(const uint pin_number, uint freq, uint pin_pwm_width)
{
    const GPIO* gpio = getGPIOByNumber(pin_number);
    if (!gpio || gpio->pin_mode != PWM_OUTPUT) {
        std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Invalid pin or not configured for PWM output." << _NORMAL_CONSOLE_TEXT_ << std::endl;
        return;
    }

    if (freq <= 0 )
    {
        std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Invalid frequency, should be > 0" << _NORMAL_CONSOLE_TEXT_ << std::endl;
        return;
    }
    #define MAX_PWM 1024 
    if (pin_pwm_width > MAX_PWM) pin_pwm_width = MAX_PWM;
    if (pin_pwm_width < 0 ) pin_pwm_width = 0;
    
    // Set PWM frequency (adjust range as needed)
    //  This example uses a simplified approach.  For more precise
    //  frequency control, especially at lower frequencies, consider using
    //  a hardware timer if your platform supports it.
    //
    //  The range of the `pwmSetRange` and the value passed to `pwmWrite`
    //  determine the resolution of the PWM signal.  A larger range
    //  gives finer control over the duty cycle.
    #ifdef DEBUG
        std::cout << _INFO_CONSOLE_TEXT << ":writePWM:pin_number:" << _LOG_CONSOLE_BOLD_TEXT << pin_number 
            << _INFO_CONSOLE_TEXT << ":freq:" << _LOG_CONSOLE_BOLD_TEXT << freq 
            << _INFO_CONSOLE_TEXT << ":pin_pwm_width:" << _LOG_CONSOLE_BOLD_TEXT << pin_pwm_width << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif
    #ifndef TEST_MODE_NO_WIRINGPI_LINK
    pwmSetMode(PWM_MODE_MS); // Example: Using Mark:Space mode.  Experiment with other modes.
    pwmSetRange(MAX_PWM); // Example range. Adjust as needed.
    pwmWrite(pin_number, pin_pwm_width);  // Calculate and set the duty cycle.
    #endif

    changeGPIOByNumber (pin_number, gpio->pin_value, pin_pwm_width);
        
    std::cout << _SUCCESS_CONSOLE_BOLD_TEXT_ << "PWM set on Pin Number: " << _INFO_CONSOLE_BOLD_TEXT << pin_number 
                    << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Frequency: " << _INFO_CONSOLE_BOLD_TEXT  << freq
                    << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", pin_pwm_width: " << _INFO_CONSOLE_BOLD_TEXT << pin_pwm_width
                    << std::endl;
}
            

        
            
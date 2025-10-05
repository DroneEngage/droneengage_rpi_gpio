#include <iostream>
#ifdef TEST_MODE_NO_WIRINGPI_LINK
void wiringPiSetupGpio(){}
#else
#include <wiringPi.h>
#endif
#include "../de_common/helpers/colors.hpp"
#include "../de_common/de_databus/configFile.hpp"

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

        
        if (pins.empty())
        {
            std::cout << _INFO_CONSOLE_BOLD_TEXT <<  "No pins to preconfigure!" << _NORMAL_CONSOLE_TEXT_ << std::endl;
        }

        for (const auto& pin : pins) {
            GPIO gpio;
            
            // Ensure required fields are present
            if (!pin.contains("gpio") || !pin.contains("mode"))
            {
                std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Missing required fields 'gpio' or 'mode' in pin configuration." << _NORMAL_CONSOLE_TEXT_ << std::endl;
                continue;
            }

            gpio.pin_number = pin["gpio"].get<int>();         // First element
            gpio.pin_mode = pin["mode"].get<int>();           // Second element
            gpio.pin_value =  0; // Default value, can be overridden below
            gpio.pin_name = "";
            gpio.gpio_type = ENUM_GPIO_TYPE::GENERIC;

            if (pin.contains("gpio_type"))
            {
                int gpio_type_value = pin["gpio_type"].get<int>();
                if (gpio_type_value >= 0 && gpio_type_value < static_cast<int>(ENUM_GPIO_TYPE::COUNT))
                {
                    gpio.gpio_type = static_cast<ENUM_GPIO_TYPE>(gpio_type_value);
                }
                else
                {
                    std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Invalid gpio_type value for pin " << gpio.pin_number << _NORMAL_CONSOLE_TEXT_ << std::endl;
                    continue;
                }
            }
            
            if (pin.contains("value") && (gpio.pin_mode != INPUT))
            {
                gpio.pin_value = pin["value"].get<int>();
            }

            if (pin.contains("width"))
            {
                gpio.pin_pwm_width = pin["width"].get<int>();
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
        std::cerr << _ERROR_CONSOLE_TEXT_ << "Exception in initGPIOFromConfigFile: " << e.what() << _NORMAL_CONSOLE_TEXT_ << std::endl;
        return false;
    }
    
}

void CGPIODriver::configurePort(const GPIO & gpio)
{
    // Validate pin number
    if (gpio.pin_number > 53) // Raspberry Pi GPIO pins are 0-53
    {
        std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Invalid pin number " << gpio.pin_number << _NORMAL_CONSOLE_TEXT_ << std::endl;
        return;
    }

    // remove node if exists from list.
    removeGPIOByNumber(gpio.pin_number);

    // Set the pin mode
    setPinMode(gpio.pin_number, gpio.pin_mode);
    
    // add node to list.
    m_gpio_array.push_back(gpio);

    // Handle OUTPUT and PWM_OUTPUT modes
    if (gpio.pin_mode == OUTPUT)
    {
        writePin(gpio.pin_number, gpio.pin_value);
    }
    else if (gpio.pin_mode == PWM_OUTPUT)
    {
        writePWM(gpio.pin_number, gpio.pin_value, gpio.pin_pwm_width);
    }

    
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

#ifndef TEST_MODE_NO_WIRINGPI_LINK
    if (wiringPiSetupGpio () == -1)
    {
        std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Unable to setup WiringPi GPIO." << _NORMAL_CONSOLE_TEXT_ << std::endl;
        return false;
    }
#else
    wiringPiSetupGpio ();
#endif

    return initGPIOFromConfigFile();
}

bool CGPIODriver::uninit()
{
    return true;
}

void CGPIODriver::setPinMode (uint pin_number, uint pin_mode)
{
    #ifdef DEBUG
    std::cout << _INFO_CONSOLE_TEXT << ":setPinMode:pin_number" << _LOG_CONSOLE_BOLD_TEXT << pin_number << _INFO_CONSOLE_TEXT << ":pin_mode:" << _LOG_CONSOLE_BOLD_TEXT << pin_mode << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif
    
    #ifndef TEST_MODE_NO_WIRINGPI_LINK
    pinMode (pin_number, pin_mode);
    if (pin_mode == OUTPUT)
    {
        pullUpDnControl(pin_number, PUD_DOWN);
    }
    else if (pin_mode == INPUT)
    {
        pullUpDnControl(pin_number, PUD_UP); // Enable pull-up resistor on button
    }
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

    if (pin_name.empty()) return nullptr;

    for (const auto& gpio : m_gpio_array) {
        if (gpio.pin_name == pin_name) {
            return const_cast<GPIO*>(&gpio); // Return the matched GPIO record
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

void CGPIODriver::writePWM(const uint pin_number, double freq, uint pin_pwm_width)
{
    const GPIO* gpio = getGPIOByNumber(pin_number);
    if (!gpio || gpio->pin_mode != PWM_OUTPUT) {
        std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Invalid pin " << pin_number << " or not configured for PWM output." << _NORMAL_CONSOLE_TEXT_ << std::endl;
        return;
    }

    // Validate frequency input
    if (freq <= 0.0) {
        // Note: Changed check to <= 0.0 as frequency can be fractional
        // Consider adding a practical lower bound check if needed
         if (freq == 0.0) {
             #ifndef TEST_MODE_NO_WIRINGPI_LINK
             // Turn PWM off completely if frequency is zero
             pwmWrite(pin_number, 0);
             #endif
             changeGPIOByNumber(pin_number, 0.0, 0); // Update state
             std::cout << _SUCCESS_CONSOLE_BOLD_TEXT_ << "PWM turned OFF on Pin Number: " << _INFO_CONSOLE_BOLD_TEXT << pin_number << std::endl;
             return;
         } else {
            std::cerr << _ERROR_CONSOLE_TEXT_ << "Error: Invalid frequency (" << freq << "), should be > 0" << _NORMAL_CONSOLE_TEXT_ << std::endl;
            return;
         }
    }

    // Clamp input duty cycle value to the expected 0-1024 range
    if (pin_pwm_width > MAX_PWM) pin_pwm_width = MAX_PWM;
    // Input is uint, so no need to check < 0 unless type changes

    uint32_t pwm_range = MAX_PWM; // Start with preferred range/resolution
    uint32_t clock_divisor = 0;
    double target_divisor;

    // --- Calculate Clock Divisor and Adjust Range if Necessary ---

    if (freq == 50.0) {
        // Special case for Servos: Force range to MAX_PWM (1024)
        pwm_range = MAX_PWM;
        target_divisor = static_cast<double>(baseClock) / (freq * pwm_range);
        clock_divisor = static_cast<uint32_t>(std::round(target_divisor));

        // Clamp divisor just in case calculation yields out-of-bounds for 50Hz (unlikely)
        if (clock_divisor < MIN_PWM_CLOCK_DIVISOR) clock_divisor = MIN_PWM_CLOCK_DIVISOR;
        if (clock_divisor > MAX_PWM_CLOCK_DIVISOR) clock_divisor = MAX_PWM_CLOCK_DIVISOR;

    } else {
        // General case: Try to use MAX_PWM range first
        target_divisor = static_cast<double>(baseClock) / (freq * MAX_PWM);

        if (target_divisor < MIN_PWM_CLOCK_DIVISOR) {
            // Frequency is too high for MAX_PWM range.
            // Fix divisor to minimum and calculate required range.
            clock_divisor = MIN_PWM_CLOCK_DIVISOR;
            double required_range = static_cast<double>(baseClock) / (freq * clock_divisor);
            pwm_range = static_cast<uint32_t>(std::round(required_range));

             // Ensure calculated range is at least minimally useful (e.g., 2)
             if (pwm_range < 2) pwm_range = 2;
             // Ensure calculated range doesn't exceed hardware limits
             if (pwm_range > MAX_HW_PWM_RANGE) pwm_range = MAX_HW_PWM_RANGE;

             // Warning: Resolution is reduced for high frequencies.

        } else if (target_divisor > MAX_PWM_CLOCK_DIVISOR) {
            // Frequency is too low for MAX_PWM range.
            // Fix divisor to maximum and calculate required range.
            clock_divisor = MAX_PWM_CLOCK_DIVISOR;
            double required_range = static_cast<double>(baseClock) / (freq * clock_divisor);
            pwm_range = static_cast<uint32_t>(std::round(required_range));

            // Warn if the required range exceeds the preferred input scale significantly
            if (pwm_range > MAX_PWM * 1.1) { // Arbitrary threshold for warning
                 // Consider adding a debug/info message here about range increase
            }

            // Ensure calculated range is at least minimally useful (e.g., 2)
            if (pwm_range < 2) pwm_range = 2;
            // Ensure calculated range doesn't exceed hardware limits
            if (pwm_range > MAX_HW_PWM_RANGE) {
                pwm_range = MAX_HW_PWM_RANGE;
                // If we hit this limit, the actual frequency will be slightly higher than requested.
                double actual_freq = static_cast<double>(baseClock) / (clock_divisor * pwm_range);
                 std::cerr << _ERROR_CONSOLE_TEXT_ << "Warning: Requested frequency " << freq << " Hz is below the hardware minimum achievable (~"
                           << actual_freq << " Hz). Setting to minimum." << _NORMAL_CONSOLE_TEXT_ << std::endl;
                 freq = actual_freq; // Update freq variable to reflect reality
            }
             // Warning: Resolution might increase beyond MAX_PWM for low frequencies.


        } else {
            // Frequency is achievable with MAX_PWM range.
            pwm_range = MAX_PWM;
            clock_divisor = static_cast<uint32_t>(std::round(target_divisor));
            // No need to clamp divisor here as it's already within bounds by definition
        }
    }

    // --- Calculate final PWM value based on scaled input ---
    // Scale the input width (0-1024) to the calculated pwm_range
    // Use floating point for intermediate calculation for accuracy
    double scaled_value_f = static_cast<double>(pin_pwm_width) * pwm_range / MAX_PWM;
    uint32_t pwm_value = static_cast<uint32_t>(std::round(scaled_value_f));

    // Clamp final value just in case of rounding errors near the boundary
    if (pwm_value > pwm_range) pwm_value = pwm_range;
    // Value is unsigned, minimum is 0


    #ifdef DEBUG
        // More detailed debug output
        std::cout << _INFO_CONSOLE_TEXT << ":writePWM:pin:" << _LOG_CONSOLE_BOLD_TEXT << pin_number
            << _INFO_CONSOLE_TEXT << ":req_freq:" << _LOG_CONSOLE_BOLD_TEXT << freq // Show potentially adjusted freq
            << _INFO_CONSOLE_TEXT << ":input_width:" << _LOG_CONSOLE_BOLD_TEXT << pin_pwm_width
            << _INFO_CONSOLE_TEXT << ":calc_divisor:" << _LOG_CONSOLE_BOLD_TEXT << clock_divisor
            << _INFO_CONSOLE_TEXT << ":calc_range:" << _LOG_CONSOLE_BOLD_TEXT << pwm_range
            << _INFO_CONSOLE_TEXT << ":pwm_value:" << _LOG_CONSOLE_BOLD_TEXT << pwm_value
            << _NORMAL_CONSOLE_TEXT_ << std::endl;
    #endif

    #ifndef TEST_MODE_NO_WIRINGPI_LINK
    // --- Apply Settings using WiringPi ---
    pwmSetMode(PWM_MODE_MS);      // Mark:Space mode is common
    pwmSetClock(clock_divisor);   // Set the calculated clock divisor
    pwmSetRange(pwm_range);       // Set the calculated range (resolution)
    pwmWrite(pin_number, pwm_value); // Set the scaled duty cycle
    #endif

    // Update internal state tracking
    changeGPIOByNumber (pin_number, freq, pin_pwm_width); // Store original requested values or actuals? Decide based on class needs. Storing requested here.

    // Success Output - showing actual calculated parameters might be more informative
    std::cout << _SUCCESS_CONSOLE_BOLD_TEXT_ << "PWM set on Pin: " << _INFO_CONSOLE_BOLD_TEXT << pin_number
              << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Target Freq: " << _INFO_CONSOLE_BOLD_TEXT << freq << " Hz" // Show potentially adjusted freq
              << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Actual Range: " << _INFO_CONSOLE_BOLD_TEXT << pwm_range
              << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Input Width: " << _INFO_CONSOLE_BOLD_TEXT << pin_pwm_width
              << _SUCCESS_CONSOLE_BOLD_TEXT_ << ", Output Value: " << _INFO_CONSOLE_BOLD_TEXT << pwm_value
              << std::endl;

    // Add a specific warning if the target frequency of 0.025 Hz was requested but not achieved
    if (freq < 1.0 && pwm_range == MAX_HW_PWM_RANGE && clock_divisor == MAX_PWM_CLOCK_DIVISOR) { // Heuristic for hitting the low limit
        double min_achievable = static_cast<double>(baseClock) / (MAX_PWM_CLOCK_DIVISOR * MAX_HW_PWM_RANGE);
         if (freq > min_achievable * 1.01) { // Check if we are close to the theoretical limit
             std::cout << _ERROR_CONSOLE_TEXT_ << "Note: Minimum achievable hardware PWM frequency is approx. "
                       << min_achievable << " Hz. Requested frequency may not be accurately generated." << _NORMAL_CONSOLE_TEXT_ << std::endl;
         }
    }
}
            

        
            
#include <iostream>
#include "../de_common/helpers/colors.hpp"
#include "../de_common/helpers/helpers.hpp"
#include "../defines.hpp"

#include "../de_common/de_databus/configFile.hpp"
#include "../de_common/de_databus/localConfigFile.hpp"
#include "gpio_main.hpp"
#include "gpio_facade.hpp"
#include "gpio_driver.hpp"


void de::gpio::CGPIOMain::loopScheduler()
{
    
    while (!m_exit_thread)
    {
        try
        {
            // timer each 10m sec.
            wait_time_nsec (0,10000000);

            m_counter++;

            if (m_counter%1000 ==0)
            {   // each 1000 msec
                CGPIO_Facade::getInstance().API_sendGPIOStatus("", true);
            }
            if (m_counter%10000 ==0)
            {   // each 10000 msec
                CGPIO_Facade::getInstance().API_sendGPIOStatus("", false);
                
            }
        }
        catch (int error)
        {

        }
    }

    return ;
}

bool de::gpio::CGPIOMain::init(const std::string& module_key)
{
    m_module_key = module_key;

    m_gpio_driver.init();
    
    m_exit_thread = false; 


    m_scheduler_thread = std::thread{[&](){ loopScheduler(); }};

    
    return true;
}

bool de::gpio::CGPIOMain::uninit()
{
    m_exit_thread = true;

    // Wait for the thread to finish
    m_scheduler_thread.join();
    
    return true;
}
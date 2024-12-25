#include "../helpers/colors.hpp"
#include "../helpers/helpers.hpp"

#include "../defines.hpp"
#include "../de_common/configFile.hpp"
#include "../de_common/localConfigFile.hpp"
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
                
            }
            if (m_counter%1000 ==0)
            {   // each 10000 msec
                
                
            }
        }
        catch (int error)
        {

        }
    }

    return ;
}

bool de::gpio::CGPIOMain::init()
{
    
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
#ifndef GPIO_PARSER_H_
#define GPIO_PARSER_H_

#include "../helpers/json_nlohmann.hpp"
using Json_de = nlohmann::json;

#include "gpio_facade.hpp"
#include "gpio_driver.hpp"

namespace de
{
namespace gpio
{

    /**
     * @brief This class parses messages received via communicator and executes it.
     * 
     */
    class CGPIOParser
    {
        public:

            static CGPIOParser& getInstance()
            {
                static CGPIOParser instance;

                return instance;
            }

            CGPIOParser(CGPIOParser const&)           = delete;
            void operator=(CGPIOParser const&)       = delete;

        
        private:

            CGPIOParser() 
            {

            }

            
        public:
            
            ~CGPIOParser ()
            {

            }
        
        public:

            void parseMessage (Json_de &andruav_message, const char * message, const int & message_length);
            
        protected:
            void parseRemoteExecute (Json_de &andruav_message);
   

        private:
            de::gpio::CGPIO_Facade& m_gpio_facade = de::gpio::CGPIO_Facade::getInstance();
            de::gpio::CGPIODriver& m_gpio_driver  = de::gpio::CGPIODriver::getInstance();                    
                
    };

}
}

#endif
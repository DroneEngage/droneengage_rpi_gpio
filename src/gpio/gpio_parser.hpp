#ifndef GPIO_PARSER_H_
#define GPIO_PARSER_H_

#include "../de_common/helpers/json_nlohmann.hpp"
using Json_de = nlohmann::json;

#include "../de_common/de_databus/de_message_parser_base.hpp"

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
    class CGPIOParser : public de::comm::CAndruavMessageParserBase
    {
        public:

            static CGPIOParser& getInstance()
            {
                static CGPIOParser instance;

                return instance;
            }

            CGPIOParser(CGPIOParser const &) = delete;
            void operator=(CGPIOParser const &) = delete;

        private:
            CGPIOParser() {}

        public:
            ~CGPIOParser() {}

        protected:
            void parseRemoteExecute(Json_de &andruav_message) override;
            void parseCommand(Json_de &andruav_message, const char *full_message, const int &full_message_length, int messageType, uint32_t permission) override;

    
            
   

        private:
            de::gpio::CGPIODriver& m_gpio_driver  = de::gpio::CGPIODriver::getInstance();                    
                
    };

}
}

#endif
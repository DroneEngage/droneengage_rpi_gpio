#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

#include <iostream>
#include <vector>

#include "../helpers/json_nlohmann.hpp"
using Json_de = nlohmann::json;


namespace de
{
namespace gpio
{

typedef struct GPIO{
        bool active;
        uint pin_number;
        uint pin_mode;
        int pin_value;
        std::string pin_name;
    } GPIO;


class CGPIODriver
    {
        public:

            static CGPIODriver& getInstance()
            {
                static CGPIODriver instance;

                return instance;
            }

            CGPIODriver(CGPIODriver const&)              = delete;
            void operator=(CGPIODriver const&)           = delete;

        
        private:

            CGPIODriver() 
            {

            }

            
        public:
            
            ~CGPIODriver ()
            {

            }
        

        public:

            bool init();
            bool uninit();

        public:

            void configurePort (const GPIO & gpio);
            void setPinMode (uint pin_number, uint pin_mode);
            int readPin (uint pin_number);
            void writePin (uint pin_number, uint pin_value);

            const std::vector<GPIO> getGPIOStatus () const; 

            const GPIO* getGPIOByNumber (uint pin_number) const;
            const GPIO* getGPIOByName (const std::string& pin_name) const;


        private:

            void removeGPIOByNumber (uint pin_number);
            bool initGPIOFromConfigFile();

        private:

            std::vector<GPIO> m_gpio_array;


    };
}
}


#endif

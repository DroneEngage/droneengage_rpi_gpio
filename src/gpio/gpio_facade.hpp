#ifndef GPIO_FACADE_H_
#define GPIO_FACADE_H_

#include "../de_common/de_facade_base.hpp"
#include "gpio_driver.hpp"

namespace de
{
namespace gpio
{
    class CGPIO_Facade : public de::comm::CFacade_Base
    {
        public:
            //https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
            static CGPIO_Facade& getInstance()
            {
                static CGPIO_Facade instance;

                return instance;
            }

            CGPIO_Facade(CGPIO_Facade const&)         = delete;
            void operator=(CGPIO_Facade const&)      = delete;

        
            // Note: Scott Meyers mentions in his Effective Modern
            //       C++ book, that deleted functions should generally
            //       be public as it results in better error messages
            //       due to the compilers behavior to check accessibility
            //       before deleted status

        private:

            CGPIO_Facade()
            {
            };

        public:
            
            ~CGPIO_Facade ()
            {
                
            };
                

        public:
            void API_sendGPIOStatus(const std::string&target_party_id, const bool internal) const;
            void API_sendSingleGPIOStatus(const std::string&target_party_id, const GPIO& gpio, const bool internal) const;
            
            
        protected:

            
    };
}
}

#endif

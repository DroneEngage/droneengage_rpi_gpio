
#include "../de_common/de_databus/messages.hpp"
#include "../de_common/de_databus/de_module.hpp"
#include "../defines.hpp"

#include "gpio_facade.hpp"
#include "gpio_parser.hpp"
#include "gpio_driver.hpp"

#include "../de_common/helpers/json_nlohmann.hpp"
using Json_de = nlohmann::json;



namespace de
{
namespace gpio
{

    /**
     * @brief 
     * 
     */
    class CGPIOMain
    {
        public:

            static CGPIOMain& getInstance()
            {
                static CGPIOMain instance;

                return instance;
            }

            CGPIOMain(CGPIOMain const&)               = delete;
            void operator=(CGPIOMain const&)         = delete;

        
        private:

            CGPIOMain() 
            {

            }

            
        public:
            
            ~CGPIOMain ()
            {

            }
        
        public:
            
            bool init (const std::string& module_key);
            bool uninit ();
            void loopScheduler();

        public:
            

            
            /**
             * @brief Get the ModuleKey 
             * 
             * @return const std::string& 
             */
            inline const std::string& getModuleKey () const
            {
                return m_module_key;
            };  


        private:

            u_int64_t m_counter = 0;
            std::thread m_scheduler_thread;

            std::string m_module_key;
            
            bool m_exit_thread = true;
            
            CGPIODriver &m_gpio_driver = CGPIODriver::getInstance();

        
    };
}
}

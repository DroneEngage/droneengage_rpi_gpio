
#include "../de_common/messages.hpp"
#include "../de_common/de_module.hpp"
#include "../defines.hpp"

#include "gpio_facade.hpp"
#include "gpio_parser.hpp"


#include "../helpers/json_nlohmann.hpp"
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
            
            bool init ();
            bool uninit ();
            void loopScheduler();

        public:
            /**
             * @brief Set the PartyID & GroupID
             * 
             * @param party_id 
             * @param group_id 
             */
            void setPartyID (const std::string& party_id, const std::string& group_id) 
            {
                m_party_id = party_id;
                m_group_id = group_id;
            };


        private:

            u_int64_t m_counter = 0;
            std::thread m_scheduler_thread;

            std::string m_party_id;
            std::string m_group_id;

            bool m_exit_thread = true;
            

        
    };
}
}

#ifndef GPIO_FACADE_H_
#define GPIO_FACADE_H_

#include "../de_common/de_facade_base.hpp"

namespace de
{
namespace sdr
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
        
            
        protected:

            
    };
}
}

#endif
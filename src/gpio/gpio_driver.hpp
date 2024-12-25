#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_


namespace de
{
namespace gpio
{
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
    };
}
}


#endif

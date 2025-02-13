#include "../global.hpp"
#include "../helpers/helpers.hpp"
#include "gpio_parser.hpp"
#include "gpio_facade.hpp"
#include "gpio_driver.hpp"
#include "gpio_main.hpp"

using namespace de::gpio;

#ifdef TEST_MODE_NO_WIRINGPI_LINK

#else
#include <wiringPi.h>
#endif



/// @brief Parses & executes messages received from uavos_comm"
/// @param parsed JSON message received from uavos_comm 
/// @param full_message 
/// @param full_message_length 
void CGPIOParser::parseMessage (Json_de &andruav_message, const char * full_message, const int & full_message_length)
{
    const int messageType = andruav_message[ANDRUAV_PROTOCOL_MESSAGE_TYPE].get<int>();
    bool is_binary = !(full_message[full_message_length-1]==125 || (full_message[full_message_length-2]==125));   // "}".charCodeAt(0)  IS TEXT / BINARY Msg  
    

    uint32_t permission = 0;
    if (validateField(andruav_message, ANDRUAV_PROTOCOL_MESSAGE_PERMISSION, Json_de::value_t::number_unsigned))
    {
        permission =  andruav_message[ANDRUAV_PROTOCOL_MESSAGE_PERMISSION].get<int>();
    }

    bool is_system = false;
    if ((validateField(andruav_message, ANDRUAV_PROTOCOL_SENDER, Json_de::value_t::string)) && (andruav_message[ANDRUAV_PROTOCOL_SENDER].get<std::string>().compare(SPECIAL_NAME_SYS_NAME)==0))
    {   // permission is not needed if this command sender is the communication server not a remote GCS or Unit.
        is_system = true;
    }

    UNUSED(is_system);
    UNUSED(permission);
    std::cout << "messageType:" << messageType << std::endl;

    if (messageType == TYPE_AndruavMessage_RemoteExecute)
    {
        parseRemoteExecute(andruav_message);

        return ;
    }

    else
    {
        Json_de cmd = andruav_message[ANDRUAV_PROTOCOL_MESSAGE_CMD];
        
        switch (messageType)
        {
            case TYPE_AndruavMessage_GPIO_ACTION:
            {
                /**
                * @brief This is a general purpose message 
                * 
                * a: P2P_ACTION_ ... commands
                * 
                */

                const Json_de cmd = andruav_message[ANDRUAV_PROTOCOL_MESSAGE_CMD];
        
                if (!cmd.contains("a") || !cmd["a"].is_number_integer()) return ;
                    
                CGPIODriver& cGPIODriver  = CGPIODriver::getInstance();                    
                

                switch (cmd["a"].get<int>())
                {
                    case GPIO_ACTION_PORT_CONFIG:
                    {
                        /**
                         * 'm': set mode 
                         * 
                         * *    INPUT			        0
                         * *    OUTPUT			        1
                         * *    PWM_OUTPUT		        2
                         * *    PWM_MS_OUTPUT	        8
                         * *    PWM_BAL_OUTPUT          9
                         * *    GPIO_CLOCK		        3
                         * *    SOFT_PWM_OUTPUT		    4
                         * *    SOFT_TONE_OUTPUT	    5
                         * *    PWM_TONE_OUTPUT		    6
                         * *    PM_OFF		            7   // to input / release line
                         *  
                         * 'n': gpio name
                         * 'p': gpio number
                         * 'v': value [used in write mode.]
                         * 'r': read
                         */

                        if (!cmd.contains("p")) return ; // missing parameters

                        GPIO gpio;
                        gpio.pin_number = cmd["p"].get<int>();
                        gpio.pin_mode = cmd["m"].get<int>();
                        if (cmd.contains("v"))
                        {
                            gpio.pin_value = cmd["v"].get<int>();
                        }
                        if (cmd.contains("n"))
                        {
                            gpio.pin_name = cmd["n"].get<int>();
                        }
                        cGPIODriver.configurePort (gpio);

                        // Send updated GPIO Status
                        CGPIO_Facade::getInstance().API_sendGPIOStatus("", true);

                    }
                    break;

                    case GPIO_ACTION_PORT_WRITE:
                    {
                        /**
                         * 'n': gpio name       // 1st priority
                         * 'p': gpio number     // 2nd priority
                         * 'v': value           // mandatory
                         */

                        CGPIODriver& cGPIODriver  = CGPIODriver::getInstance();
                        
                        bool trigger_event = false;
                            
                        if (!cmd.contains("v")) return ;
                        const GPIO* gpio;
                        const int value = cmd["v"].get<int>();
                        if (cmd.contains("n")) // priority for named gpio  over gpio value.
                        {
                            // gpio name
                            gpio = cGPIODriver.getGPIOByName(cmd["n"].get<std::string>());
                            if (gpio == nullptr) return ; // gpio is not defined.
                            if (gpio->pin_mode == INPUT)
                            {
                                if (gpio->pin_value != value) trigger_event = true;
                                cGPIODriver.writePin(gpio->pin_number, value);
                            }
                            else if (gpio->pin_mode == PWM_OUTPUT)
                            {
                                if (!cmd.contains("d")) return ;// pwm width
                                const int pwm_width = cmd["d"].get<int>();
                                if (gpio->pin_pwm_width != pwm_width) trigger_event = true;
                                
                                cGPIODriver.writePWM(gpio->pin_number, value, pwm_width);
                            }
                        }
                        else
                        if (cmd.contains("p"))
                        {
                            // gpio name
                            gpio = cGPIODriver.getGPIOByNumber(cmd["p"].get<int>());
                            if (gpio == nullptr) return ; // gpio is not defined.
                            if (gpio->pin_mode == INPUT)
                            {
                                if (gpio->pin_value != value) trigger_event = true;
                                cGPIODriver.writePin(gpio->pin_number, value);
                            }
                            else if (gpio->pin_mode == PWM_OUTPUT)
                            {
                                if (!cmd.contains("d")) return ;// pwm width
                                const int pwm_width = cmd["d"].get<int>();
                                if (gpio->pin_pwm_width != pwm_width) trigger_event = true;
                                
                                cGPIODriver.writePWM(gpio->pin_number, value, pwm_width);
                            }
                        }

                        // Send updated GPIO Status
                        if (trigger_event)
                        {
                            CGPIO_Facade::getInstance().API_sendSingleGPIOStatus("", gpio[0], false);
                        }
                    }
                    break;

                    case GPIO_ACTION_PORT_READ:
                    {
                        // TODO should reply to sender with a message contains value.

                        
                    }
                    break;

                    default:
                    {

                    }
                    break;                 


                };
            }
            break;


            case TYPE_AndruavMessage_GPIO_REMOTE_EXECUTE:
            {
                const int subCommand = cmd["a"].get<int>();
                switch (subCommand)
                {

                    case TYPE_AndruavMessage_GPIO_STATUS:
                        CGPIO_Facade::getInstance().API_sendGPIOStatus("", true);
                    break;

                    default:
                    break;
                }
            }
            break;       

            default:
            {

            }
            break;
        }

    }

    UNUSED(is_binary);
}

/**
 * @brief part of parseMessage that is responsible only for
 * parsing remote execute command.
 * 
 * @param andruav_message 
 */
void CGPIOParser::parseRemoteExecute (Json_de &andruav_message)
{
    const Json_de cmd = andruav_message[ANDRUAV_PROTOCOL_MESSAGE_CMD];
    
    if (!validateField(cmd, "C", Json_de::value_t::number_unsigned)) return ;
                
    uint32_t permission = 0;
    if (validateField(andruav_message, ANDRUAV_PROTOCOL_MESSAGE_PERMISSION, Json_de::value_t::number_unsigned))
    {
        permission =  andruav_message[ANDRUAV_PROTOCOL_MESSAGE_PERMISSION].get<int>();
    }

    bool is_system = false;
     
    if ((validateField(andruav_message, ANDRUAV_PROTOCOL_SENDER, Json_de::value_t::string)) && (andruav_message[ANDRUAV_PROTOCOL_SENDER].get<std::string>().compare(SPECIAL_NAME_SYS_NAME)==0))
    {   // permission is not needed if this command sender is the communication server not a remote GCS or Unit.
        is_system = true;
    }

    UNUSED (is_system);
    UNUSED (permission);
    
    const int remoteCommand = cmd["C"].get<int>();
    
    std::cout << "cmd: " << remoteCommand << std::endl;
    
    // switch (remoteCommand)
    // {
    //     case TYPE_AndruavMessage_GPIO_ACTION:
    //     {
    //         const int subCommand = cmd["a"].get<int>();

    //         switch (subCommand)
    //         {
    //             case GPIO_ACTION_GPIO_INFO:
    //                 CGPIO_Facade::getInstance().API_GPIOInfo(andruav_message[ANDRUAV_PROTOCOL_SENDER].get<std::string>());
    //                 break;
    //             case GPIO_ACTION_LIST_GPIO_DEVICES:
    //                 CGPIO_Facade::getInstance().API_SendGPIODrivers(andruav_message[ANDRUAV_PROTOCOL_SENDER].get<std::string>());
    //                 break;
                
    //             default:
    //                 break;
    //         }
    //     }
    //     break;

        
    // }
}
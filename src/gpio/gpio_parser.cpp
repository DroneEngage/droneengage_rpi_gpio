#include "../global.hpp"
#include "../helpers/helpers.hpp"
#include "gpio_parser.hpp"
#include "gpio_facade.hpp"
#include "gpio_driver.hpp"
#include "gpio_main.hpp"

using namespace de::gpio;




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
            case TYPE_AndruavMessage_ID:
            {
            }
            break;

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
                UNUSED(cGPIODriver);

                // switch (cmd["a"].get<int>())
                // {
                //     case GPIO_ACTION_CONNECT:
                //     {
                //         cGPIODriver.openGPIO();
                //     }
                //     break;

                //     case GPIO_ACTION_DISCONNECT:
                //     {
                //         cGPIODriver.closeGPIO();
                //     }
                //     break;

                //     case GPIO_ACTION_SET_CONFIG:
                //     {
                //         /**
                         
                //             int     i:  driver index
                //             double fc:  frequency center
                //             double  g:  gain
                //             double  s:  sample Rate
                //             double  d:  decode mode
                //             double  r:  number of bars for barchart.
                //             [int     t]:  interval of sending data back per seconds [0 means ignore]. 

                //          */
                //         #ifdef DEBUG
                //             std::cout << cmd.dump() << std::endl;
                //         #endif
                        
                //         if (validateField(cmd,"i", Json_de::value_t::number_unsigned))
                //         {
                //             cGPIODriver.setGPIODriverIndex(cmd["i"].get<int>());
                //         }

                //         if (cmd.contains("fc"))
                //         {
                //             cGPIODriver.setFrequencyCenter(cmd["fc"].get<double>());
                //         }

                //         if (cmd.contains("g"))
                //         {
                //             cGPIODriver.setGain(cmd["g"].get<double>());
                //         }

                //         if (cmd.contains("s"))
                //         {
                //             cGPIODriver.setSampleRate(cmd["s"].get<double>());
                //         }

                //         if (cmd.contains("m"))
                //         {
                //             //NOT IMPLEMENTED (Modulation)
                //         }

                //         if (cmd.contains("r"))
                //         {
                //             cGPIODriver.setBars(cmd["r"].get<double>());
                //         }

                //         if (validateField(cmd, "t", Json_de::value_t::number_unsigned))
                //         {   // milli-seconds
                //             cGPIODriver.setIntervals(cmd["t"].get<int>()); 
                //         }
                //         else
                //         {
                //             cGPIODriver.setIntervals(0);
                //         }


                //         if (validateField(cmd, "l", Json_de::value_t::number_unsigned))
                //         {   // milli-seconds
                //             cGPIODriver.setTriggerLevel(cmd["l"].get<int>()); 
                //         }
                //         else
                //         {
                //             cGPIODriver.setTriggerLevel(0);
                //         }


                //         // broadcast updated info.
                //         CGPIODriver::getInstance().openGPIO();

                //         CGPIO_Facade::getInstance().API_GPIOInfo(std::string(""));
                //         CGPIO_Facade::getInstance().sendLocationInfo(std::string(""));
                //     }
                //     break;

                //     case GPIO_ACTION_READ_DATA:
                //     {
                //         // Create a new thread and detach it immediately
                //         std::thread([&]() { 
                //             cGPIODriver.startStreaming(); 
                //         }).detach(); // Detach the thread right after creation
                //     }
                //     break;
                    

                //     case GPIO_ACTION_PAUSE_DATA:
                //     {
                //         cGPIODriver.pauseStreaming();
                //     }
                //     break;

                //     default:
                //     {

                //     }
                //     break;
                // }
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
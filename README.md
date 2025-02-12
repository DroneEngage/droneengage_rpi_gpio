
[![Ardupilot Cloud EcoSystem](https://cloud.ardupilot.org/_static/ardupilot_logo.png "Ardupilot Cloud EcoSystem")](https://cloud.ardupilot.org "Ardupilot Cloud EcoSystem") **Drone Engage** is part of Ardupilot Cloud Eco System

------------

![Drone Engage GPIO Module](resources/de_logo_title.png) 

# Drone Engage RPI-GPIO Module

DroneEngage-RPI-GPIO is a Drone Engage plugin module that enables DroneEngage to control GPIO ports of RPI.
The module uses RPI GPIO as on-off ports to controll leds, buzzer, motors ...etc.

# Configuration File

*de_rpi_gpio.config.module.json* is used to define ports.

    {
           "gpio": 7,            			// GPIO Port Number.
           "mode": 1,            			// 1 OUTPUT mode.
           "value": 1,           			// 1 ON-at startup.
           "gpio_type": 1,       			// 1 System Port
           "name": "power_led"    			// Port Name 
    }

**System Port** means that it is part of DroneEngage system, such as Power/Connection LED. CameraLeds ...etc.
**Port Name:** Some of them are reserver such as power_led, others can be of any name. The main purpose of them is to enable other module uses port names to turn on/off if these port exist on the system.

    {
	    "gpio": 11,
	    "mode": 1,
	    "value": 1,
	    "gpio_type": 1, // 1 System Port
	    "name": "camera_flash"  //Camera module uses it as flash
    },

    {
	    "gpio": 12,
	    "mode": 1,
	    "value": 0,
	    "gpio_type": 0, // 0 Generic Port
	    "name": "my_gpio"  // any custom gpio for any purpose.
    },


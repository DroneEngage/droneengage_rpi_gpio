[![Ardupilot Cloud EcoSystem](https://cloud.ardupilot.org/_static/ardupilot_logo.png "Ardupilot Cloud EcoSystem")](https://cloud.ardupilot.org "Ardupilot Cloud EcoSystem") **Drone Engage** is part of Ardupilot Cloud Eco System

------------

![Drone Engage GPIO Module](resources/de_logo_title.png) 

# Drone Engage RPI-GPIO Module

DroneEngage-RPI-GPIO is a Drone Engage plugin module that enables DroneEngage to control GPIO ports of RPI.


[![Ardupilot Cloud EcoSystem](https://cloud.ardupilot.org/_static/ardupilot_logo.png "Ardupilot Cloud EcoSystem")](https://cloud.ardupilot.org "Ardupilot Cloud EcoSystem") **Drone Engage** is part of Ardupilot Cloud Eco System

## Getting Started

### Prerequisites

- CMake 3.28.3 or higher
- GCC or compatible C++ compiler
- Raspberry Pi with GPIO support


### Building the Project

1. Clone the repository:
    ```sh
    git clone https://github.com/DroneEngage/droneengage_rpi_gpio.git
    cd droneengage_rpi_gpio
    ```

2. Run the build script:
    ```sh
    ./build.sh
    ```
### Configuration
Configuration files are located in the root directory:

de_rpi_gpio.config.module.json: Module configuration.

You can define static predefined GPIO ports in the config file.
Other modules of DroneEngage can define GPIO dynamically.

You can give names to the ports, some names are predefined across the system such as "power_led". "power_led" will be used by Communicator Module to display connection status.
 
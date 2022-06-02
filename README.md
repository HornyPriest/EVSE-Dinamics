# EVSE-Dinamics



The Dinamics uses an **ESP32** which communicates with the OpenEVSE controller via serial RAPI API. The online UI is served at napolni.me/app, data gets to napolni.me via MQTT. Local webUI is served at ip of device.


## Contents

<!-- toc -->
- [Versions](#versions)
- [Changes](#changes)
- [Requests](#requests)
- [Settings](#settings)
- [Issues](#issues)
- [Features](#features)
- [Requirements](#requirements)
- [About](#about)
- [Licence](#licence)

<!-- tocstop -->


## Versions

Last version is in folder ESP32_dinamika_V11_getSettings


## Changes

V11:
- added requests for settings and wifi

V10
- Semi ported to Event based software

## Requests

Requests are done via MQTT. Use MQTT explorer, topic prefix is "Dinamics/[DeviceSerialNo.]/"

Requests available over MQTT:
|       Topic       | Value |          Description              |
| ----------------- | ----- | --------------------------------- |
| get_status        | 1     | request new currents status       |
| get_settings      | 1     | request active settings           |
| get_wifi          | 1     | request active wifi settings      |

## Settings

At the moment settings can be changed only over MQTT. Use MQTT explorer, topic prefix is "Dinamics/[DeviceSerialNo.]/"

Settings setable over MQTT:
|       Topic       |                    Value                   |                                          Description                                            |
| ----------------- | ------------------------------------------ | ----------------------------------------------------------------------------------------------- |
| set_enable        | 1,2,3                                      | set enable status of charging station (1 = turn off, 2 = enable charging, 3 = sleep)            |
| set_breaker       | 6-100[A]                                   | set value of breakers in house in Amps                                                          |
| set_update        | 1=start update of sketch / fota            | start fota from server, only update if new version available                                    |
| set_update_spiffs | 1=start update of SPIFFS / fota            | start fota of SPIFFS from server, only update if new version available                          |
| set_t_limit       | 1-100 (1=15min)                            | set charging time limit, each count is 15min (4 means 1h) !only possible when charging          |
| set_e_limit       | 1-100 (1=1-3kWh, depends on active phases) | set charging energy limit, (1P - 1 means 1kWh ; 1P - 1 means 3kWh) !only possible when charging |
| set_c_limit       | 6-32[A]                                    | set charging current limit in Amps  !only possible when charging                                |
| set_calibration   | 277(50A,1V), 544(100A,1V)                  | set calibration value for CT sensors (depends on type of sensors)                               |
| set_debug         | 1-5 (not defined yet)                      | set debug level (higher level means more debug messages on MQTT                                 |
| set_timer         | 1-500000 (default = 2000)                  | set time interval for sending Currents to MQTT                                                  |
| set_timer1        | 1-500000 (default = 50000)                 | set time interval for auto stop charging if charge current is < 0.5A                            |
| set_timer2        | 1-500000 (default = 10000)                 | set time interval for auto stop charging if cable state is not connected                        |
| set_timer3        | 1-500000 (default = 10000)                 | set time interval for auto stop charging if charging current > max. allowed current             |
| set_timer4        | 1-500000 (default = 10000)                 | set time interval for auto stop charging if max. allowed current < 3A                           |
| set_timer5        | 1-500000 (default = 50000)                 | set time interval for checking set max. allowed current from openEVSE                           |
| set_timer6        | 1-500000 (default = 30000)                 | set time interval for checking cable state from openEVSE                                        |
| set_timer7        | 1-500000 (default = 10000)                 | set time interval for checking charging current from openEVSE                                   |
| set_timer8        | 1-500000 (default = 50000)                 | set time interval for checking charging status from openEVSE                                    |
| set_timer9        | 1-500000 (default = 100000)                | set time interval for checking energy usage from openEVSE                                       |
| set_timer10       | 1-500000 (default = 50000)                 | set time interval for checking Dinamics<->openEVSE connection status                            |
| set_timer11       | 1-500000 (default = 50000)                 | set time interval for pause between WiFi recconnect attempts                                    |
| set_timer12       | 1-500000 (default = 100000)                | set time interval for sending timeSync to MQTT                                                  |
| set_plugandcharge | 0 or 1 (default = 0)                       | set Plug and Charge mode, (1 = P&C is active, 0 = station automatically stops charge)            |

## Issues

- Web UI on server over AP does not work on last version, somewhat works on previous version ESP32_dinamika_V10_TaskWiFi


## Features

- MQTT view & control all OpenEVSE functions
  - Start / pause
  - Delay timer
  - Time limit
  - Energy Limit
  - Adjust charging current
  - more
- Smart dynamic power adjustment in real-time
- MQTT view & control dynamic management settings
  - Sensor calibration
  - Breakers value
  - Timers
  - Plug&Charge or Controlled Start&Stop


## Requirements

### EVSE by Implera d.o.o. charging station

- Purchase via www.implera.com
- OpenEVSE FW [V7.1.3+ recommended](https://github.com/OpenEVSE/open_evse/releases)
- All new EVSE units are shipped with V7.1.3 pre-loaded (April 2021 onwards)

### Dinamics by Implera d.o.o. smart dynamic management device based on ESP32
- Purchase via www.implera.com

## About

Project made by Implera d.o.o. based on [OpenEnegyMonitor](http://openenergymonitor.org) and [OpenEVSE](https://openevse.com) projects.

Contributions by:

- @HornyPriest
- @MatejZ-zz
- @bernardo2798
- @buntalol
- @Implera d.o.o.


## Licence

GNU General Public License (GPL) V3
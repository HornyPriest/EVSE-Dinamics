# EVSE-Dinamics



The Dinamics uses an **ESP32** which communicates with the OpenEVSE controller via serial RAPI API. The online UI is served at napolni.me/app, data gets to napolni.me via MQTT. Local webUI is served at ip of device.


## Contents

<!-- toc -->
- [Versions](#versions)
- [Flashing](#flashing)
- [SetUp](#setup)
- [EVSECalibration](#evsecalibration)
- [Changes](#changes)
- [Requests](#requests)
- [Settings](#settings)
- [MessagesAndResponses](#messagesandresponses)
- [Issues](#issues)
- [Features](#features)
- [Requirements](#requirements)
- [About](#about)
- [Licence](#licence)

<!-- tocstop -->


## Versions

Last version is in folder ESP32_dinamika_V14_NewWebPage
Bin file DinamicsV4.bin

## Flashing

To flash use "flash_download_tool_3.9.2.rar". Open app after unrar. You must upload two files .bin (DinamicsV1.bin and spiffs.bin). See example picture.
Make sure to add correct address before upload. Address for DinamicsV1.bin = 0x10000 and address for spiffs.bin = 0x290000

## SetUp

At first start some settings have to be set over MQTT in order for Dinamics to work as supposed.

Mandatory settings:
- set_breaker
- set_calibration
- set_c_limit
- set_plugandcharge

(look in [Settings](#setting) for details)

## EVSECalibration

EVSEs current measurments, can be calibrated using RAPI commands. Usable commands are:
- S2 (set 0|1 - disable/enable ammeter calibration mode - ammeter is read even when not charging)
- SA currentscalefactor currentoffset - set ammeter settings
- T0 amps - set fake charging current

more info on https://github.com/openenergymonitor/open_evse/blob/master/firmware/open_evse/rapi_proc.h
 


## Changes

V14: added debug set/get
     added JSON form for some answers (not complete)

V11:
- added requests for settings and wifi

V10
- Semi ported to Event based software

## Requests

Requests are done via MQTT. Use MQTT explorer, topic prefix is "Dinamics/[DeviceSerialNo.]/"

Requests available over MQTT:
|       Topic       |              Value            |          Description              |                                                      Response Topic                                          |
| ----------------- | ----------------------------- | --------------------------------- | ------------------------------------------------------------------------------------------------------------ |
| get_status        | 1                             | request new currents status       | current1, current2, current3, max_charging_current, charging current, active phases                          |
| get_settings      | 1                             | request active settings           | settings                                                                                                     |
| get_wifi          | 1                             | request active wifi settings      | wifi                                                                                                         |
| get_debug         | 1                             | request active debug settings     | debug                                                                                                        |
| rapi_request      | RAPI commands without "$"     | send direct RAPI command to EVSE  | rapi_response (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h) |                                                                                  |

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
| set_c_limit       | 6-32[A] (default = breaker switch on board)| set charging current limit in Amps                                                              |
| set_c_min_limit   | 6-32[A] (default = 6)                      | set Plug and Charge mode, (1 = P&C is active, 0 = station automatically stops charge)           |
| set_calibration   | 277(50A,1V), 544(100A,1V) (default = 277)  | set calibration value for CT sensors (depends on type of sensors)                               |
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
| set_timer13       | 1-500000 (default = 6000)                  | set timeout for requests to EVSE                                                                |
| set_plugandcharge | 0 or 1 (default = 0)                       | set Plug and Charge mode, (1 = P&C is active, 0 = station automatically stops charge)           |
| set_debug         | 40 digits with 0 or 1 (default = 40x0)     | set debug messages, (1 = debug is active, 0 = debug inactive), only valid if all 40 digits sent |

## MessagesAndResponses

Messages and responses from Dinamics to MQTT server. Topic prefix is "Dinamics/[DeviceSerialNo.]/"

|         Topic        |                       Value                     |                                                                             Description                                                                      |
| -------------------- | ----------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| breaker              | 6-200[A]                                        | currently set breakers value in Amps                                                                                                                         |
| fwversion            | 1-9999                                          | FW version running on Dinamics                                                                                                                               |
| current1             | 0-200[A]                                        | current measured on CT 1 in Amps                                                                                                                             |
| current2             | 0-200[A]                                        | current measured on CT 2 in Amps                                                                                                                             |
| current3             | 0-200[A]                                        | current measured on CT 3 in Amps                                                                                                                             |
| max_charging_current | 0-32[A]                                         | calculated max available charging current in Amps                                                                                                            |
| charging_current     | 0-32[A]                                         | actual charging current in Amps                                                                                                                              |
| active_phases        | 1 to 3 digits (0,1,2,3,12,13,23,123,99)         | active phases on this charging session                                                                                                                       |
| enable               | 1-3                                             | enable charging status (1 - station turned off, 2 - charging enabled, 3 - station is asleep)                                                                 |
| timeout              | timer13/2-timer13                               | time needed for response from EVSE (only posted if timer13/2 < value < timer13)                                                                              |
| state                | 0,1,2                                           | status of charging plug (0 - vehicle not connected, 1 - vehicle connected, 2 - unknown)                                                                      |
| connection           | OK or CON_ERROR                                 | status of connection between Dinamics <---> EVSE                                                                                                             |
| debug_log            | \\$message1$$message2$...etc                    | one or more debug messages each encapsuled in separators "$"                                                                                                 |
| synctime             | current epoch timestamp                         | actual epochtime in the moment of sending this message (meant for time syncing)                                                                              |
| state_change         | RAPI message from EVSE ($AT...)                 | automatic message from EVSE when charging state is changed (cable unpluged, battery full, etc.)                                                              |
| settings             | String (\\$variable=value$...)                  | string contain multiple individual settings each encapsuled in separators "$", each timer separated with ":"                                                 |
| wifi                 | String (\\$variable=value$...)                  | string contain multiple individual wifi settings and credentials each encapsuled in separators "$"                                                           |
| debug                | String (\\$Debugs=debug1:...:debug40$)          | string contain multiple individual debug messages switch settings, each debug switch(0 or 1) separated with ":"                                              |
| responseGC           | RAPI response from EVSE (String)                | EVSEs RAPI response for GC - get current capacity info (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)        |
| responseG0           | RAPI response from EVSE (String)                | EVSEs RAPI response for G0 - get EV connect state (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)             |
| responseGG           | RAPI response from EVSE (String)                | EVSEs RAPI response for GG - get charging current and voltage (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h) |
| responseGS           | RAPI response from EVSE (String)                | EVSEs RAPI response for GS - get state (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)                        |
| responseGU           | RAPI response from EVSE (String)                | EVSEs RAPI response for GU - get energy usage (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)                 |
| responseFD           | RAPI response from EVSE (String)                | EVSEs RAPI response for FD - disable EVSE (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)                     |
| responseFE           | RAPI response from EVSE (String)                | EVSEs RAPI response for FE - enable EVSE (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)                      |
| responseFS           | RAPI response from EVSE (String)                | EVSEs RAPI response for FS - sleep EVSE (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)                       |
| responseSC           | RAPI response from EVSE (String)                | EVSEs RAPI response for SC - set current capacity (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)             |
| responseST           | RAPI response from EVSE (String)                | EVSEs RAPI response for ST - set timer (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)                        |
| responseSH           | RAPI response from EVSE (String)                | EVSEs RAPI response for SH - set cHarge limit to kWh (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)          |
| rapi_response        | RAPI response from EVSE (String)                | EVSEs RAPI response for rapi_request (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h)                          |

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

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
- [ResponseDataFormat](#responsedataformat)
- [ATmessagesDecypher](#atmessagesdecypher)
- [Issues](#issues)
- [Features](#features)
- [Requirements](#requirements)
- [About](#about)
- [Licence](#licence)

<!-- tocstop -->


## Versions

Last version is in folder "ESP32_dinamika_V28_wifiscan"

Older versions are placed in archive
PROPERLY TESTED IS VERSION V28!!!!

## Flashing

To flash use "flash_download_tool_3.9.2.rar". Open app after unrar. You must upload 4 files .bin (DinamicsHW2.bin, Dinamics.spiffs.bin, Dinamics.partitions.bin, Dinamics.bootloader.bin). See example picture.
Make sure to add correct address before upload. Addresses for bin files are DinamicsHW2.bin = 0x10000, Dinamics.spiffs.bin = 0x290000, Dinamics.partitions.bin = 0x8000, Dinamics.bootloader.bin = 0x1000

## SetUp

Charging station looks for 2 wifi networks on boot.
1. User save wifi network
2. System default ssid: "Implera" password: "AdminSettings"
If either is found it connects to it, if none found charging station makes unprotected access point/hotspot (AP) with ssid: "Dinamics-SN".

At first start some settings have to be set over MQTT in order for Dinamics to work as supposed.

!!!Mandatory check and set settings!!!:
- set_breaker (default 16A)
- set_calibration (default 277 - CT=50A)
- set_c_limit (max current; default 16A)
- set_plugandcharge (default 1 = ON)
- set_cte (default 0 - no CT sensors)
- set_dinamics (default 0 - disabled dynamic power management)
- rapi_request = SL 2 

Optional settings:
- set_update (if running FW version is not the last available)
- delete_settings (if running FW version is not last available. Restores settings to default)
- set_lora (default 0 = cable connection active)



(look in [Settings](#setting) for details)

## EVSECalibration

EVSEs current measurments, can be calibrated using RAPI commands. Usable commands are:
- S2 (set 0|1 - disable/enable ammeter calibration mode - ammeter is read even when not charging)
- SA currentscalefactor currentoffset - set ammeter settings
- T0 amps - set fake charging current

more info on https://github.com/openenergymonitor/open_evse/blob/master/firmware/open_evse/rapi_proc.h
 


## Changes

v28: add wifi scan before connecting for more robust set-up environment


v27: Repaired AT async message parsing


v26: Added "debug" string spliting because MQTT message maximum length is 200 chars.


v25: Outgoing JSON messages are now in correct JSON format


v24: MQTT reconnection update


v23: This Number is retired (version v23 does not exist)


v22: Fixed bug when P&C==0, wifi reconnecting time is lowered


V21: Posta united with other versions, added lora toggle


V20: Catch async messages from EVSE fix, added CT On/Off switch, added dinamic mode toggle


V18: added support for LCD


V17: added 3 retries before connection error is activated, added auto P&C if no internet connection


V16: timers changed, LoRa communication optimized, Web page working


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
| rapi_request      | RAPI commands without "$"     | send direct RAPI command to EVSE  | rapi_response (details on: https://github.com/OpenEVSE/open_evse/blob/stable/firmware/open_evse/rapi_proc.h) |
| get_timers        | 1                             | request active timers value       | timers                                                                                    |

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
| set_timer1        | 1-500000 (default = 150)                   | set time interval for auto stop charging if charge current is < 0.5A                            |
| set_timer2        | 1-500000 (default = 5)                     | set time interval for auto stop charging if cable state is not connected                        |
| set_timer3        | 1-500000 (default = 10000)                 | set time interval for auto stop charging if charging current > max. allowed current             |
| set_timer4        | 1-500000 (default = 100)                   | set time interval for auto stop charging if max. allowed current < 3A                           |
| set_timer5        | 1-500000 (default = 200000)                | set time interval for checking set max. allowed current from openEVSE                           |
| set_timer6        | 1-500000 (default = 30000)                 | set time interval for checking cable state from openEVSE                                        |
| set_timer7        | 1-500000 (default = 30000)                 | set time interval for checking charging current from openEVSE                                   |
| set_timer8        | 1-500000 (default = 200000)                | set time interval for checking charging status from openEVSE                                    |
| set_timer9        | 1-500000 (default = 120000)                | set time interval for checking energy usage from openEVSE                                       |
| set_timer10       | 1-500000 (default = 5000)                  | set time interval for checking Dinamics<->openEVSE connection status                            |
| set_timer11       | 1-500000 (default = 10000)                 | set time interval for pause between WiFi recconnect attempts                                    |
| set_timer12       | 1-500000 (default = 100000)                | set time interval for sending timeSync to MQTT                                                  |
| set_timer13       | 1-500000 (default = 8000)                  | set timeout for requests to EVSE                                                                |
| set_timer14       | 1-500000 (default = 10000)                 | set CON ERROR flag reset for requests to EVSE                                                   |
| set_plugandcharge | 0 or 1 (default = 1)                       | set Plug and Charge mode, (1 = P&C is active, 0 = station automatically stops charge)           |
| set_nwpc          | 0 or 1 (default = 0)                       | set auto Plug and Charge mode if no internet, (1 = nwpc is active, 0 = nwpc is inactive)        |
| set_neg_amp       | 0 or 1 (default = 0)                       | set possible negative amperage, (1 = negative amps expected, 0 = no negative amps expected)     |
| set_debug         | 40 digits with 0 or 1 (default = 40x0)     | set debug messages, (1 = debug is active, 0 = debug inactive), only valid if all 40 digits sent |
| set_adjust        | 0 or 1 (default = 1)                       | set adjust, (1 = implera remote adjust(peakshaving) active, 0 = implera remote adjust inactive) |
| set_autoupdate    | 0 or 1 (default = 1)                       | set automatic updates, (1 = auto update active, 0 = auto update inactive)                       |
| set_TFO           | 0 - 1000 (default = 30)                    | set TFO(timers factor off, (factor timers5,7,8,9 when NOT charging AND timer6 when charging)    |
| delete_settings   | 1                                          | restore settings to default predefined values                                                   |
| delete_wifi       | 1                                          | delete wifi credentials                                                                         |
| set_lcd           | String                                     | Write this string on LCD                                                                        |
| erase_lcd         | 1                                          | clear whole LCD                                                                                 |
| set_cte           | 1,12,123,23,13,0 (default=0)               | set CT active and measuring (set which CTs are active)                                          |
| set_dinamics      | 0 or 1 (default = 0)                       | set enable dinamics mode (1 = on, 0 = off)                                                      |
| set_lora          | 0 or 1 (default = 0)                       | activate communication between EVSE and Dinamics over LoRa (1 = LoRa active, 0 = Cable active)  |

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
| no_of_phases         | 1-3                                             | number of active phases                                                                                                                                      |
| power                | 0-22000[W]                                      | charging power                                                                                                                                               |
| energy               | 0-99999999[Wh]                                  | session delivered energy                                                                                                                                     |
| enable               | 1-3                                             | enable charging status (1 - station turned off, 2 - charging enabled, 3 - station is asleep)                                                                 |
| timeout              | if timeout > timer13/2-timer13                  | time needed for response from EVSE (only posted if timer13/2 < value < timer13)                                                                              |
| state                | 0,1,2                                           | status of charging plug (0 - vehicle not connected, 1 - vehicle connected, 2 - unknown)                                                                      |
| connection           | OK or CON_ERROR_FAILED_COM :                    | status of connection between Dinamics <---> EVSE (failed communication returs rapi message that was not answered                                             |
| debug_log            | \\$message1$$message2$...etc                    | one or more debug messages each encapsuled in separators "$"                                                                                                 |
| synctime             | current epoch timestamp                         | actual epochtime in the moment of sending this message (meant for time syncing)                                                                              |
| state_change         | RAPI message from EVSE ($AT...)                 | automatic message from EVSE when charging state is changed (cable unpluged, battery full, etc.)                                                              |
| settings             | String (\\$variable=value$...)                  | string contain multiple individual settings each encapsuled in separators "$", each timer separated with ":"                                                 |
| timers               | String/JSON (\\$variable=value$...)             | string contain multiple individual timers in JSON format                                                                                                     |
| wifi                 | String (\\$variable=value$...)                  | string contain multiple individual wifi settings and credentials each encapsuled in separators "$"                                                           |
| debug                | String (\\$Debugs=debug1:...:debug40$)          | string contain multiple individual debug messages switch settings, each debug switch(0 or 1) separated with ":"                                              |
| plugandcharge        | 0 or 1                                          | set_plugandcharge response to confirm received setting                                                                                                       |
| ip                   | ip form [xxx.xxx.xxx.xxx]                       | Dinamics local ip address                                                                                                                                    |
| freeRAM              | Free SRAM in kbytes                             | Free HEAP size on ESP32                                                                                                                                      |
| responseEraseLCD     | RAPI response from EVSE (String)                | EVSEs RAPI response for FP 0 0 - erase chars on LCD                                                                                                          |
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

## ResponseDataFormat

Responses that contain more that one variable are in JSON format. Variable names are shortened to save space for more useful informations.
Glossary of JSON variables:
| Abbrevation | Description |
| ----------- | ---------------------------------------------------------------------------------------------------------- |
| FW | Firmware version |
| B | breakers |
| MAXC | max allowed charging current |
| MINC | min allowed charging current |
| C | CT sensors calibration value |
| P&C | plug and charge setting |
| NWPC | no wan plug and charge (if no internet connection --> turn on plug and charge) |
| NA | negative amperage (device expect negative amperage in case user is producing more electricity then he/she consumes) |
| A | allows Implera to change MAXC and MINC settings |
| AU | auto update |
| LCD | LCD enabled or disabled |
| TFO | not charging timers factor (to lower communications when charging is not active) |
| CTE | which CTs are enabled |
| DA | dynamic or static power mode |
| L | Lora on or off |


## ATmessagesDecypher

EVSE state transition: sent whenever EVSE state changes

MESSAGE FORM:  "$AT EVSE_STATE EVSE_STATE c_limit vflags"

 evsestate(hex): EVSE_STATE_xxx

 pilotstate(hex): EVSE_STATE_xxx

 currentcapacity(decimal): amps

 vflags(hex): m_wVFlags bits

EVSE_STATE dictionary:

| Value | Description |
| ----------- | ---------------------------------------------------------------------------------------------------------- |
| 01 | vehicle state A 12V - not connected |
| 02 | vehicle state B 9V - connected, ready |
| 03 | vehicle state C 6V - charging |
| 04 | vehicle state D 3V - vent required |
| FAULTS |  FAULTS |
| 05 | diode check failed |
| 06 | GFCI fault |
| 07 | bad ground |
| 08 | stuck relay |
| 09 | GFI self-test failure |
| 10 | over temperature error shutdown |
| 11 | over current error shutdown |
| 12 | pilot self test error |
| 13 | temp sensor dead |
| 14 | EVSE relay closure fault |
| 254 | waiting for timer |
| 255 | disabled |


## Issues

- Timers might need to be additionally calibrated in some cases
- Restart when paused because too little current is available is not happening
- Negative amperage expectation is not tested enough, might not work
- WiFi reconnection sometimes does not always reconnect fast enough (timers?!)
- Ports must be opened for MQTT[31883] and auto update[80] features


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
  - CT sensors calibration
  - Adjustable breakers value
  - Timers
  - Plug&Charge or Controlled Start&Stop
  - LCD control
  - Toggle between LoRa and cable communication
  - Toggle between Dynamic and Static power settings
  - Connect and activate reading of up to 3 CT sensors


## Requirements

### EVSE by Implera d.o.o. charging station

- Purchase via www.implera.com

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

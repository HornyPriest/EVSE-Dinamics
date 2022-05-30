# EVSE-Dinamics



The Dinamics uses an **ESP32** which communicates with the OpenEVSE controller via serial RAPI API. The web UI is served at napolni.me/app, data gets to napolni.me via MQTT.


## Contents

<!-- toc -->
- [Versions](#versions)
- [Changes](#changes)
- [Issues](#issues)
- [Features](#features)
- [Requirements](#requirements)
- [About](#about)
- [Licence](#licence)

<!-- tocstop -->


## Versions

Last version is in folder ESP32_dinamika_V10_TaskWiFiLoop_Events


## Changes

- Semi ported to Event based software

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
  - Plug&Charge or Controlled Start


## Requirements

### OpenEVSE by Implera d.o.o. charging station

- Purchase via www.implera.com
- OpenEVSE FW [V7.1.3+ recommended](https://github.com/OpenEVSE/open_evse/releases)
- All new OpenEVSE units are shipped with V7.1.3 pre-loaded (April 2021 onwards)

### Dinamics by Implera d.o.o. smart dynamic management device based on ESP32
- Purchase via www.implera.com

## About

Collaboration of [OpenEnegyMonitor](http://openenergymonitor.org) and [OpenEVSE](https://openevse.com).

Contributions by:

- @HornyPriest
- @MatejZ-zz


## Licence

GNU General Public License (GPL) V3
# ESP32 Buck-Boost MPPT Battery Charger

> This project is NOT MAINTAINED nor a finished device. With a few [fixes](#neccesary-fixes) a prototype was tested to output a voltage, but the firmware isn't even finished to do anything else yet.

A battery charger in the spirit of [Fugo - 1kW Arduino MPPT Solar Charger](https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/).
The main difference is that it's a four-switch buck-boost converter, meaning you can charge batteries below AND above your PV-panels voltage.

![image_project](/images/image_project-1.jpg)

## Features:
- 4-way buck-boost stage
- 5-80V, 30A 
	- Mosfets rating is max 80V, but it's advised to not actually operate near the limit
- 39kHz switching frequency
- OLED Display
- 4 Buttons
- cheap car fuses
- Input-/Output Voltage- & Current sensing
- redundant logic power source: connect either PV panel or battery to start system
- adjustabe gate voltage to finetune mosfet efficiency
- extra unused GPIO-pins

---
# Hardware
## Components
- MCU: WROOM32E
- Onboard Voltage Regulators: 
	- LM5161 (4V Buck converter)
	- AP2112 (3.3V LDO)
	- SDB628 (12V Boost converter for MOSFET Gate Drivers)
- ADC: ADS1015
	- 12bit 3300sps vs ADS1115 16bit 860sps (unneccesary resolution, we need higher sample rate)
	- Internal vref 2,058V
	- added RC-filter to reduce noise
- OLED: 0.96" 128x64 SSD1306
- Mosfet Gate Driver: IR2184
	- IR2104 output much less current but might suffice since we're not switching at high frequencies and use FETs with low gate-capacitance
- Shunt Amplifier: INA240 (1mOhm shunt resistor)
- Temp Sensor: NTC 10k B3950 (measured with ESP32's internal ADC)

## 4-switch buck-boost topology
Basically we PWM-switch an inductor in a MOSFET H-Bridge. Depending on which Mosfet is controlled, we can achieve either a syncronous buck-converter, boost-converter or buck-boost-converter.
[This](https://www.ti.com/lit/an/slyt765/slyt765.pdf) article explains it.
Some High-side MOSFETs would need to be constantly on, depending on the operating mode. But since they are driven with a bootstrap driver, we'll just drive them with something like a 99% dutycycle at a lower frequency. If Texas Instruments says that's good enough then I won't doubt it. 
Backflow current shouldn't be an issue wither with an H-Bridge. Neither when battery voltage is higher than PV voltage, nor do MOSFET body diodes leak current.

## Inductor
The inductor is self-wound. Depending on what voltage/current your setup runs, you would want to calculate the ideal inductivity.
Instead of the original "Magnetics Inc. 0077071A7" I used a cheaper alternative "CS330060" from aliexpress.
My (shizophrenic!) notes on them are [here](/documentation/Inductor Dimensions.md). I have no clue what I'm doing! There are likely mistakes!

**My inductor used:**
- 3 strands of 16AWG copper wire, each ca. 1.29mm diameter, total crosssection ca. 4.1mm² 
- ca. 64uH, 36.6A saturation current

While i still had access to fancy industrial measurement equipment I measured the actual inductivity with different amount of turns.

| Turns | Inductance |
| ----- | ---------- |
| 31 T  | 75 µH      |
| 30 T  | 70 µH      |
| 29 T  | 65 µH      |
| 28 T  | 61 µH      |
| 27 T  | 57 µH      |
| 26 T  | 53 µH      |
| 25 T  | 48 µH      |
| 24 T  | 44 µH      |
| 23 T  | 41 µH      |
| 22 T  | 37 µH      |
| 21 T  | 34 µH      |
| 20 T  | 31 µH      |
| 19 T  | 28 µH      |
| 18 T  | 25 µH      |
| 17 T  | 22 µH      |
| 16 T  | 20 µH      |
| 15 T  | 18 µH      |
| 14 T  | 15 µH      |

## Problems
- I haven't tested the 12V Boost converter & wired up external 12V in my tests
- when connecting an external 3.3V powersource for programming, the voltage was backfed and powered the 12V boost converter, thereby enabling the Mosfet Driver
	- my programmer got pretty hot from powering all that!
- no protection circuitry! should be added in the future for a robust/reliable system
	- efuse: reverse polarity, inrush current limiting, overvoltage, overcurrent
	- TVS diodes: transients, ESD on anything external (cables, buttons, display)
	- output relay: to disconnect the battery potential-free
	- choke filter for logic power supply
	- don't use just a potentiometer (R20) for voltage feedback - if the wiper has no contact with the resistive element, it may be pulled up to a voltage high enough to destroy the IC pin it's connected to! try to use poti alongside fixed resistor-divider in case of poti failure
- change `C14` to 100V
- change value of `R9` (25k is hard to find, I had to [stack](/images/image_project-2.jpg) 2x50k in parallel)
- change value of `R12`, `R13` (5kOhm is hard to find)
- change `C17`, `C18`, `C19`, `C20` (25V instead of 50V is plenty)
- clearly label PV and Battery terminal on silkscreen
- reposition the OLED mounting holes

## Neccesary fixes
The current schematic/layout has some major oopsies!
- [3.3V power rail](/images/3v3-vin_shorted.PNG) is routed through a 80V power plane
	- i cut the [trace](/images/fixes_cut_traces.png) with an x-acto knife & [bridged it with wire](/images/fixes_wirebridges.JPG)
- the boost-stage gate-driver signals were connected to ESP32 GPIO 36 & 39 (Input only)
	- cut the [traces](/images/fixes_cut_traces.png) & [wire](/images/fixes_wirebridges.JPG) to GPIO 25 & 26 instead

## Testpoints
**Top side:**
- `4V`
- `12V`
- `3V3`
**Bottom side:**
- `VIN`: voltage input for the 4V buck converter (coming from either PV or Battery terminal)
- `UVLO`: undervoltage lockout signal for 4V buck
- `SW`: output switch node from 4V buck converter

## Jumper
The onboard voltage regulators outputs have a solder jumper that you'll need to bridge for the system to work.
- `JP1`: enable 4V power rail
- `JP2`: enable 12V power rail
- `JP3`: enable 3.3V power rail

---
# Software
The firmware only demonstrates its buck/boost functionality.
A constant voltage gets outputted.
No MPPT algorithm or anything else is yet implemented.

## Versions
- v01.00 - functional demo
- v01.01 - adds OLED display features

## Flashing
- You'll need a serial-to-usb adapter 
- wire 2 buttons to the programming header to pull GPIO0 & EN to GND
- software can be opened using Visual Studio Code & PlatformIO
- other software can be used to flash firmware files located in `/.pio/build/denky32`

## To Do's
- MPPT algorithm
- [Shadow Management](https://www.youtube.com/watch?v=AZ8VQ_zmf1M&t=209s): run a global sweep for the power curve to find the MPPT peak (in case of multipe PV)
- buck-boost mode
- battery charging algorithms
- software watchdog
- [Task Scheduling](https://youtu.be/J8zc8mMNKtc?si=thOGyFFcOTulUn3A) (run the pwm-loop on one core, everything else on the other core)
- measure temperature
- change user parameters on OLED display
- connect to MQTT server over WiFi

---
# Test Instructions
- flash the ESP32 with firmware
- connect a lab power supply (12V, 200mA) to the Input (U3)

- Testing 4V power rail & undervoltage lockout
	- Testpoint `TP1` should measure 4V
	- change power supply:
	- <5.0V and `TP1` should measure 0V
	- \>5.6V and `TP1` should measure 4V again
	- close the jumper `JP1`
> Only solder jumpers with the power supply off!
- put power supply back to 12V

- Testing 12V power rail:
	- Testpoint `12V` should measure 11.4V (adjust with potentiometer `R20`)
	- close the jumper `JP2`

- Testing 3.3V power rail:
	- Testpoint `3V3` should measure 3.3V
	- close the jumper `JP3`

- Testing Buck-mode:
	- power on the device
	- battery output (U4) should measure 5V
> You may need to wire a power resistor to the output or the voltage could be unstable
	  
- Testing Boost-mode:
	- hold down the `SEL`-Button while powering on the device
	- battery output (U4) should measure 24V




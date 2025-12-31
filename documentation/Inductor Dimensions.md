## From Fugo 1kW MPPT
- Model: 0077071A7
- Manufacturer: Magnetics Inc.
- Core Material: Kool Mμ (Sendust - FeSiAl / Iron-Silicon-**Aluminium**)
- Relative Magnetic Permeability: 60μ
- Dimensions: OD = 33.5mm , ID = 19.5mm, H = 11mm
- Al = 61 nH/T^2
- Ae = 65.6mm^2
- Bsat = 1.0T

**How to build Toroidal core inductor**
1. use T130 or larger
2. Yellow #26 iron powder core is common, Sendust has 40-50% less core losses though ([How to coose inductor material](https://www.coilws.com/index.php?main_page=page&id=41))
3. Visit Coil32's online calculator ([Click me](https://coil32.net/online-calculators/ferrite-torroid-calculator.html))
4. Scroll down and find ferrite toroid calculator.
5. We will be using a gauge 16 magnet wire, there is a dropbox on the upper right and select "AWG 16"
6. In the text box "L", input the inductance that you would want your inductor to have. My MPPT design required an inductance of 64μH, so I entered 64 in the box. (it was actually 48μH, the previous steps discussed why it became 64μH).
7. OD refers to your toroid's outer diameter, measure yours in mm and input it in the text box.
8. ID refers to your toroid's inner diameter, measure yours in mm and input it in the text box.
9. H refers to your toroid's height or thickness, measure yours in mm and input it in the text box.
10. You can leave Chamfer to 0mm, unless your toroidal core's datasheet specifies it.
11. μr refers to the "Relative Magnetic Permeability". This one is tricky and is often found in in your toroidal core's datasheet. Luckily 0077071A7 has a datasheet that specified it of having 60μ, so I entered 60 in the text box. If you are dealing with an unknown toroidal core with no label, observe if it was colored with paint, some toroidal inductors have colors to specify the core material. Yellow toroids are often 75μ. ([inductor color code article](http://www.catzco.com/toroids.htm))
12. Do not touch the other text boxes, leave them be!
13. Press the "Compute" button.
14. You will get results down below
15. N - is the number of turns of gauge 16 magnet wire that you would have to wind around the toroidal core in order to get the inductance that you have specified in L
16. Lw - is simple an approximation on the length of wire that you will need in order to get N number of turns around the toroidal inductor.

**Inductor Current Rating**
- also called saturation current
- the DC Current at which the inductance value drops below a specific point (~30%)
How to calculate it:
1. Visit Pigeon's Nest Calculator ([Click me](http://pigeonsnest.co.uk/stuff/core-saturation.html))
2. If you want to learn, you can find the formulas from there as well!
3. You will be asked to fill some parameters.
4. These parameters can only be found from your toroidal core's datasheet
5. This is why I said it was difficult. Very few manufactures specify these fields in their datasheet.
6. The parameters ``Al`` is the inductance per turn. This is something you would find in a toroidal core datasheet. If you don't have datasheets for your toroidal core, you can visit ([this link](http://www.catzco.com/toroids.htm)) for finding ``Al`` values for generic iron powder core toroids.
7. ``Ae`` is simply the effective cross-sectional area of a toroid. This can also be found in the datasheet. If you don't have one, use a formula from your geometry subject to solve for a toroid's cross-sectional area.
8. ``L`` is simply the inductance that your designed inductor has.
9. ``Bsat`` is the Magnetic Saturation Flux Density and it is one of the most difficult to find. I couldn't even find it in the datasheet, I didn't want to settle with Pigeon's Nest's 0.4T assumption since I'm dealing with a different core material. Luckily, I found ([this article](https://www.coilws.com/index.php?main_page=page&id=41)) showing the different magnetic flux densities (``Bsat``) of the common core materials used in toroids. (Sendust Kool Mu cores like mine turned out to have a Bsat = 1.0T). But, if you are dealing with a generic iron powder core, ``Bsats`` can be found from ([visit this link](http://www.catzco.com/toroids.htm)).
10. Now that we have everything we need. Press the "Calculate" button on the Isat box.
11. ``Isat`` is now your Inductor's Saturation Current Rating!
---
## From Aliexpress
- Model: AS130060A CS330060 77071-A7 P (CGS330060E14)
- Material: Sendust FeSiAl toroidal
- Dimensions (according to datasheet): ODxIDxHT 33x19x10 mm
- AL (Inductance coefficient): 51nH/N2 
- Ue (Permeability): 60 approximately
- saturation magnetic induction intensity: 1.05T
	- comments: probably closer to 1L; Throttle by calculation 440 μg and currents up to 14A. 10A definitely holds without falling out into saturation

**Specs from Taobao**
- 33x19,9x14mm, 45g
- Inductance coefficient 80nH
- Magnetic Permeability: 60

**Specs from another [Aliexpress](https://de.aliexpress.com/item/1005005183711482.html):
- Model: Juncan Sendust Core CS330060E14 (130-060E14) 
- 33x19,9x14
- Le = 8,15cm
- Ae = 0,855cm²
- V = 6,97cm³
- μo = 60
- AL = 80,0 nH/N²
- Density = 6,6 g/cm³
- weight: 43g
- Coating: epoxy resin
- Material: 6%Al, 9%Si, 85%Fe
- can be appplied with Frequencies above 8kHz
- Saturation Flux density: 1,05T

**Results from [coil32.net](https://coil32.net/online-calculators/ferrite-torroid-calculator.html) calculator**
- Input: 16AWG, 64uH, 33x19x10mm, 60μr
- Turns: 31,082
- AL = 66 nH/N^2
- Length: 1,057m

**Results from [coil32.net](https://coil32.net/online-calculators/ferrite-torroid-calculator.html) calculator for taobao** specs
- Input: 16AWG, 64uH, 33x19x10mm, 60ur
- Turns: 26,269
- AL = 93 nH/N^2
- Length: 1,103m

**Results from [pigeonsnest](http://pigeonsnest.co.uk/stuff/core-saturation.html) calculator for taobao soecs**
- Ae = 2xAxB = 2x8mmx14mm = 224mm^2  
- input: 1.0T,   93nH/N^2,   224mm^2,  64uH
- Isat = 91,8156 A
- N = 26,233

**Results from [coil32.net](https://coil32.net/online-calculators/ferrite-torroid-calculator.html) calculator for new aliexpress specs**
- Input: 64uH, 33x19x14mm, 60ur, 1,32mm
- Turns: 26,269
- AL = 93 nH/N²
- Length: 1,103m

**Results from [pigeonsnest](http://pigeonsnest.co.uk/stuff/core-saturation.html) calculator for new aliexpress specs** 
- input: 1.05T,   80nH/N^2,   85,5mm^2,  64uH
- Isat = 36.5828 A
- N = 26.233

---
## Inductor Configurations

| Inductor          | _L_  | ODxIDxh    | Permeability | _d_    | =Turns | =_AL_   | =Length | _Bsat_ | _AL_    | Ae      | L    | =Isat   | =N      |
| ----------------- | ---- | ---------- | ------------ | ------ | ------ | ------- | ------- | ------ | ------- | ------- | ---- | ------- | ------- |
| AE (AE new specs) | 64uH | 33x19x14mm | 60 _µr_      | 1,32mm | 26,27  | 93nH/N² | 1,103m  | 1.05T  | 80nH/N² | 85,5mm² | 64uH | 36,58A  | 26,233  |
|                   | 64   |            |              | 2.2863 | 26.269 | 93      | 1.138   |        |         | 85.5mm² |      | 39.6753 | 28.2843 |
|                   | 48   |            |              |        | 22.749 | 93      | 0.955   |        |         |         |      | 45.8131 | 24.4949 |
|                   | 35   |            |              |        | 19.426 | 93      | 0.816   |        |         |         |      | 53.6508 | 20.9165 |
|                   | 25   |            |              |        | 16.418 | 93      | 0.69    |        |         |         |      | 63.4805 | 17.6777 |

---
**Inductances:**
- Fugo: should be **48uH**, but chose **64uH** as it's more versatile for most setup

| L       | Vin max | I max | Vout max | f     | Vripple | Iripple | efficiency |
| ------- | ------- | ----- | -------- | ----- | ------- | ------- | ---------- |
| 15,52uH | 80V     | 30A   | 27V      | 39kHz | 0,05V   | 35%     | 95%        |
| 27,16uH |         |       |          |       |         | 20%     |            |
| 45,26   | 50      | 20    | 27       | 39    | 0,05    | 20      | 95         |
| 35,83   | 60      | 15    | 28       | 39    | 0,05    | 35      | 95         |
| 62,7    |         |       |          |       |         | 20      |            |
| 14,46   | 60      | 15    | 15       | 39    | 0,05    | 35      | 95         |
| 25,30   |         |       |          |       |         | 20      |            |


---
## Notes
- **!!! Instead of one thikk ass copper wire, bundle multiple smaller ones together to negate the skin effect !!!** like [here](https://www.instructables.com/DIY-ESP32-Based-Dual-Phase-Interleaved-MPPT/)
- 16AWG = 1,2908mm diam = 1,3087 mm²
- 3x 1,32mm diam = 3x 1,3685mm² = 4,1054mm² = 2.2863 diam = 11AWG
- Copper length estimate: 44cm per 10 turns (tested with 1x1,32mm on AE inductor)
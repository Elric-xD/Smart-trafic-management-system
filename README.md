## Circuit Diagram Description for Your Smart Traffic Control System

### Components and Connections:

#### 1. **Arduino Uno**

* Power: Connect 5V and GND to the breadboard power rails.

#### 2. **Sensors (4 lanes)**

* Each sensor's signal pin → Arduino Analog pins:

  * Lane 1 sensor → A0
  * Lane 2 sensor → A1
  * Lane 3 sensor → A2
  * Lane 4 sensor → A3
* Power and GND from Arduino 5V and GND.

#### 3. **Emergency IR Receiver**

* Signal pin → Arduino Digital pin 2 (with internal pull-up enabled)
* Power (5V) and GND from Arduino.

#### 4. **Traffic Light LEDs for each lane (Green, Yellow, Red)**

* Lane 1:

  * Green → D3 (with 220Ω resistor in series to GND)
  * Yellow → D4 (with 220Ω resistor)
  * Red → D5 (with 220Ω resistor)
* Lane 2:

  * Green → D6
  * Yellow → D7
  * Red → D8
* Lane 3:

  * Green → D9
  * Yellow → D10
  * Red → D11
* Lane 4:

  * Green → D12
  * Yellow → D13
  * Red → D14 (This is pin A0 digital mode, be careful with conflict; if needed, use D0 or D1 instead)

Each LED's long leg (anode) goes to Arduino digital output pin through a 220Ω resistor; the short leg (cathode) goes to GND.

---

## Visual Circuit Diagram You Can Create

![Simplified Smart Traffic Light Circuit](https://cdn-learn.adafruit.com/assets/assets/000/074/243/large1024/ledcircuit_bb.png?1654221307)

*(Note: This image shows a basic LED with resistor connected to Arduino pin and GND. You’d replicate this for each LED.)*

---

## How to Make Your Own Diagram Easily

1. **Use Fritzing** ([https://fritzing.org/](https://fritzing.org/))

   * Place Arduino Uno.
   * Add 4 IR or ultrasonic sensors and wire them to A0-A3.
   * Add 12 LEDs (3 per lane), each with 220Ω resistor to GND.
   * Wire LED inputs to pins D3–D14.
   * Add an IR receiver module wired to D2.
   * Connect all grounds and 5V lines.

2. **Use Tinkercad Circuits** ([https://www.tinkercad.com/circuits](https://www.tinkercad.com/circuits))

   * It has built-in Arduino Uno, LEDs, sensors.
   * You can simulate and run your code.

---

If you want, I can help create a **step-by-step wiring schematic in text** for you to follow or help build the Fritzing file if you upload your partial work.

Would you like me to prepare a detailed wiring text guide or suggest exact parts and wire colors?

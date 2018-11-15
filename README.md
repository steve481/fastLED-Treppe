# fastLED-Treppe
Code für die Steuerung der LEDs, die in der treppe in den 1. Stock eingebaut sind.

Für das Projekt wird die **FastLED** Library mit den grundlagen des "XY MATRIX" beispiels verwendet. 


### Hardware:
- Die LEDs auf der Treppe bilden eine **11x17** Matrix, verbunden in einer Schlangenform.
- Es sind **WS2811** LED-Streifen verbaut mit **30 LEDs/Meter**.
- Die Kontrolle übernimmt ein Arduino UNO, der in einem Case an der Wand oberhalb der Treppe befestigt ist.
- Die Stromversorgung regelt ein **12V 3A Netzteil**, dass sich ebenfalls im Case oberhalb der Treppe befindet... Und ja, 3A sind eigentlich nicht ausreichend, wenn man wirklich sicher gehen möchte... deshalb sollten **NICHT alle LEDs gleichzeitig in Weiß leuchten!**
- Im Steuerungscase sind zusätzlich **2 Knöpfe** und **ein Kippschalter** mit 3 einstellungsmöglichkeiten angebracht. Die bisherige steuerung definiert, dass der Kippschalter die LEDs An und Aus macht und in den Debug-Mode versetzt. Die Knöpfe sind noch undefiniert.

### Arduino-Pinbelegung:

- Pin 6: WS2811 LED streifen

- Pin 8: Roter Knopf oben
- Pin 9: Roter Knopf unten
- Pin 10: Kippschalter in Stellung Mitte/Oben
- Pin 11: Kippschalter in Stellung Mitte/Unten

- VIN: 12V Stromversorgung
- GND: GND..

# Datasheets & Pinouts

Overzicht van alle technische documentatie en pinout-schema's van de gebruikte componenten in dit project.

| Onderdeel | Datasheet / Pinout | Wat belangrijk is voor het project |
| :--- | :--- | :--- |
| **18650 Li-ion Batterijen (2x)** | [Datasheet 18650 Batterij](./Datasheet_18650Batterij.pdf) | Nominale spanning 3,6 V per cel (7,2 V in serie, tot 8,4 V volgeladen), capaciteit 2900 mAh en maximale ontlaadstroom van 8,25 A voor de voeding van de motoren en elektronica. |
| **Drukknop (Start / Stop)** | [Datasheet Drukknop](./Datasheet_Button.pdf) | Normaal open (NO) momentschakelaar op `GPIO 14` voor de externe hardware-interrupt (`RISING`) waarmee de robot fysiek gestart of gestopt wordt. |
| **DRV8833 Dubbele H-brug** | [Datasheet DRV8833](./Datasheet_DRV8833.pdf) | Voedingsspanning 2,7 V – 10,8 V, continue stroom tot 1,5 A per kanaal (2 A piek) en `EEP`/`nSLEEP`-pin (`GPIO 18`) om de driver in slaapstand te zetten bij stilstand. |
| **DRV8833 Pinout** | [Pinout DRV8833](./PINout_DRV8833.pdf) | Aansluitschema voor `AIN1`/`AIN2` (`GPIO 19, 21`), `BIN1`/`BIN2` (`GPIO 22, 23`), `EEP` (`GPIO 18`), `VM` (motorvoeding) en `GND`. |
| **ESP32 DevKit V1** | [Datasheet ESP32](./Datasheet_ESP32.pdf) | Dual-core microcontroller met 3,3 V logisch niveau, ingebouwde Wi-Fi (Access Point + WebSocket) en 12-bit ADC1-kanalen (`0–4095`) voor het uitlezen van de sensoren. |
| **ESP32 Pinout** | [Pinout ESP32](./PINout_ESP32.pdf) | Pinselectie van uitsluitend ADC1-pinnen (`GPIO 36, 39, 34, 35, 32, 33`) voor de sensoren, omdat ADC2-pinnen niet bruikbaar zijn wanneer Wi-Fi actief is. |
| **N20 DC-Transmissiemotor (2x)** | [Datasheet G12-N20 Motor](./Datasheet_G12-N20-GearMotor-6V-500RPM.pdf) | Nominale spanning 6 V, onbelast toerental 500 RPM (overbrenging 1:30) voor een goede balans tussen snelheid en koppel op het parcours. |
| **MINI 560 PRO Step-Down Converter** | [Datasheet MINI 560 PRO](./Datasheet_MINI560PRO.pdf) | Efficiënte DC-DC buck converter (tot 5 A) die de batterijspanning (~7,2 V – 8,4 V) stabiel omzet naar 5 V voor de ESP32 en de sensor-array. |
| **MTS-113 Tuimelschakelaar** | [Datasheet MTS-113](./Datasheet_MTS-113.pdf) | Hoofdschakelaar (SPDT) waarmee de batterijvoeding volledig losgekoppeld kan worden van het volledige circuit. |
| **Philips Condensator (47 µF – 385 V)** | [Specificaties Philips 47 µF](./Datasheet_Philips_47uF_385V.md) | Axiale elektrolytische ontkoppelcondensator over de voedingsrail om spanningsdippen en motorruis op te vangen zodat de ESP32 niet reset. |
| **QTR-8A Reflectiesensor Array** | [Datasheet QTR-8A](./Datasheet_QTR-8A.pdf) | Analoge IR-reflectiesensoren met een steekafstand van 9,525 mm (6 sensoren gebruikt op `3,3 V`), ideaal voor parabolische lijnpositie-interpolatie. |

# Proof of Concept: Motorsturing (DRV8833 Dual H-Bridge)

Voor de aansturing van de twee N20 motoren wordt gebruikgemaakt van de **DRV8833 dual H-bridge motor driver**. Deze module verzorgt de vermogensschakeling tussen de batterijvoeding en de motoren op basis van 3.3V logische signalen van de ESP32.

#### Pinconfiguratie & Aansluitingen
* **OUT1 & OUT2:** Vermogensuitgang naar Motor A (links).
* **OUT3 & OUT4:** Vermogensuitgang naar Motor B (rechts).
* **IN1 t/m IN4:** PWM-ingangen vanaf de ESP32 GPIO's voor snelheids- en draairichtingsregeling.
* **Vcc:** Voedingsspanning voor de motoren (rechtstreeks vanaf het 2S Li-ion batterijpakket via de hoofdschakelaar).
* **GND:** Gemeenschappelijke referentiemassa (verbonden met de GND van de batterij en ESP32).
* **EEP (Sleep):** Ingang voor slaapmodus (active low). Moet logisch HIGH (3.3V) zijn om de driver in te schakelen.
* **ULT (Fault):** Foutuitgang (active low, overstroom/oververhitting). Blijft voor de initiële test ongeadresseerd / zwevend.

#### Implementatie & Testdoel
De vier ingangen worden aangesloten op PWM-compatibele GPIO-pinnen van de ESP32. Via `analogWriteFrequency()` worden 20kHz PWM-signalen gegenereerd om pieptonen te elimineren. 
In de code wordt de EEP-pin via GPIO18 op HIGH gezet om de driver in te schakelen.
De effectieve motorspanning vanuit het 2S-batterijpakket (7,4V - 8,4V) wordt via de PWM-duty-cycle softwarematig begrensd naar maximaal 6V.

### Test
1. Motor links vooruit/achteruit
2. Motor rechts vooruit/achteruit
3. Motor links van traag naar snel in 4 snelheden
4. Motor rechts van traag naar snel in 4 snelheden
5. Beide motoren vooruit/achteruit

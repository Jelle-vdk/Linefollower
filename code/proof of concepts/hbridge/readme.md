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
De vier ingangen worden aangesloten op PWM-compatibele GPIO-pinnen van de ESP32. Via het interne LEDC-peripheral worden 20 kHz PWM-signalen gegenereerd om hoogfrequente pieptonen in de motoren te elimineren. 

Het doel van deze Proof of Concept is valideren dat:
1. Beide motoren onafhankelijk van elkaar bidirectioneel (vooruit en achteruit) kunnen worden aangestuurd.
2. De PWM-duty cycle de rotatiesnelheid lineair en betrouwbaar regelt.
3. De slaapstand via de EEP-pin correct in- en uitgeschakeld kan worden.

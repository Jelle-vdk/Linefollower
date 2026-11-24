# ESP32 PID LineFollower

Een zelfgebouwde autonome lijnvolger-robot op basis van een **ESP32**, aangestuurd door een tijdsgestuurde **PID-regelaar** (P, I en D) met parabolische sensor-interpolatie, niet-vluchtige parameteropslag in EEPROM en een live **WebSocket-dashboard** voor draadloze tuning, kalibratie en manuele besturing.

---

## Overzicht & Werking

De robot volgt een zwarte lijn op een witte ondergrond met behulp van een **6-kanaals analoge reflectiesensor-array** (steekafstand 9,525 mm) en twee DC-motoren aangestuurd via een **DRV8833 H-brug**.

### Belangrijkste kenmerken
* **Deterministische regellus:** De volledige meting, interpolatie en PID-berekening draait op een vaste, instelbare cyclustijd (in microseconden).
* **Parabolische interpolatie:** Berekent niet alleen welke sensor zwart ziet, maar schat de exacte lijnpositie in millimeters (ca. -24 mm tot +24 mm) voor vloeiend stuurgedrag.
* **Volledige PID-regeling met tijdsschaling:**
  * **Proportioneel (`Kp`):** Directe stuurreactie evenredig met de lijnpositie.
  * **Integrerend (`Ki`):** Werkt kleine mechanische snelheidsverschillen tussen beide motoren en blijvende volgfouten weg (inclusief *anti-windup* begrenzing en automatische reset bij start).
  * **Differentiërend (`Kd`):** Dempt snelle koerswijzigingen in scherpe bochten om doorschot (*overshoot*) te voorkomen.
  * **Vermogensverdeling (`diff`):** Regelt de verhouding tussen versnellen van het buitenste wiel en afremmen/tegensturen van het binnenste wiel in bochten.
* **Draadloos WebSocket-dashboard:** De ESP32 zet een eigen Wi-Fi Access Point op (`Linefollower_JVDK`) met een live webinterface voor statusbeheer, live PID-tuning, sensorgrafiek en manuele besturing.
* **Ingebouwde veiligheden:**
  * Automatische noodstop (`STATE_STOP`) wanneer alle 6 sensoren gedurende 10 opeenvolgende cycli wit zien (lijn kwijt of robot opgetild).
  * Slaapstand-beveiliging op de DRV8833 (`EEP`-pin) wanneer beide motoren stilstaan of de browserverbinding wegvalt.
  * Fysieke drukknop met hardware-interrupt en debounce voor directe start/stop op de robot zelf.

---

## Hardware & Pinout (ESP32)

| Onderdeel | ESP32 Pin(nen) | Opmerking |
| :--- | :--- | :--- |
| **Reflectiesensoren (S1 – S6)** | `GPIO 36, 39, 34, 35, 32, 33` | Analoge ingangen (ADC1, steekafstand 9,525 mm) |
| **Motoren (Links & Rechts)** | `GPIO 19, 21, 22, 23` | PWM-sturing naar DRV8833 `IN1..IN4` |
| **Motor Enable / Sleep (`EEP`)** | `GPIO 18` | Actief hoog bij motoractiviteit, laag in rust |
| **Start/Stop Drukknop** | `GPIO 14` | Externe interrupt (`RISING`) met 200 ms software-debounce |
| **Status LED** | `GPIO 27` | Uit = `STOP`, Aan = `RUN`, Knipperend = `MANUAL` |
| **Wi-Fi Verbindings-LED** | `GPIO 26` | Brandt zodra er een client verbonden is met het Access Point |

---

## Bestandsstructuur & Uitleg per Bestand

### 1. `LineFollower_Code.ino` (Hoofdprogramma & Toestandsmachine)
Het startpunt van de firmware. Dit bestand koppelt de hardwarepinnen aan de regelklasse, zet het Wi-Fi Access Point en de asynchrone webserver/WebSocket op, en beheert de hoofdstatus van de robot via een toestandsmachine (*state machine*):

* **`STATE_STOP` (Stop-modus):**
  * Alle motoren zijn spanningsloos en de `EEP`-pin van de H-brug staat laag (slaapstand).
  * De PID-regellus is gepauzeerd en de status-LED is uit.
  * Veilige begintoestand bij het opstarten, na het indrukken van de noodstopknop of wanneer de robot van de lijn raakt.
* **`STATE_RUN` (Autonome lijnvolg-modus):**
  * Bij het betreden van deze status worden de PID-geheugenwaarden (`iTerm`, `lastErr` en de `offLineCount`-teller) gereset zodat de robot altijd met een schone lei vertrekt.
  * De regellus draait op maximale snelheid volgens de ingestelde `cycleTime` zonder vertragende seriële communicatie in de lus.
  * De status-LED brandt continu.
* **`STATE_MANUAL` (Test-, kalibratie- & handmatige modus):**
  * Bedoeld om de robot op het parcours te testen en af te stellen zonder dat hij zelfstandig wegrijdt.
  * Stuurt elke 40 ms de genormaliseerde sensorwaarden (`0–1000`) en de berekende lijnpositie (in mm) via WebSocket naar de live grafiek op de webpagina.
  * Activeert de manuele besturingsknoppen (vooruit, achteruit, links en rechts draaien met instelbaar snelheidspercentage).
  * De status-LED knippert om aan te geven dat de robot in testmodus staat.

---

### 2. `Cyclus.h` (Regeltechniek, Sensorverwerking & Commando-afhandeling)
De kern van de software. In deze klasse zit de volledige regeltechnische logica en commandoverwerking verwerkt:
* **Kalibratie (`calibrate black` / `calibrate white`):** Meet per sensor het gemiddelde van 10 analoge metingen voor zwart en wit en slaat deze op in EEPROM.
* **Normalisatie & Parabolische Interpolatie (`normalisatie_interpolatie()`):** Herschaalt elke ruwe ADC-meting naar een uniforme schaal van `0` (wit) tot `1000` (zwart), zoekt de pieksensor en berekent via een parabool door de drie hoogste naastliggende sensoren de exacte sub-sensorpositie in millimeters.
* **PID-regeling & Motorverdeling (`update()`):**
  * Berekent `output = P + I + D`, begrensd tussen `-510` en `+510`.
  * Verdeelt `output` over `powerLeft` en `powerRight` op basis van `power` (basissnelheid `0–255`) en `diff` (`0.0–1.0`).
  * Herschaalt `Ki` en `Kd` automatisch naar seconden wanneer de cyclustijd (`set cycle`) wordt aangepast.
* **Dual Input Stream (`DualInputStream`):** Zorgt ervoor dat commando's zowel via de USB-seriële monitor als draadloos via de WebSocket-terminal door dezelfde `SerialCommand`-parser verwerkt worden.

---

### 3. `webpage.html.h` (Live WebSocket Dashboard)
Bevat de volledige HTML-, CSS- en JavaScript-code voor de bedieningsinterface, opgeslagen in het flashgeheugen (`PROGMEM`) van de ESP32:
* **Status-selector:** Direct schakelen tussen `STOP`, `RUN` en `MANUAL` met visuele kleurfeedback.
* **Live PID- & Motorpaneel (zichtbaar in `RUN`):** Invoervelden met directe `Send`-knoppen en actuele uitlezing voor `power`, `diff`, `Kp`, `Ki` en `Kd`, zonder telkens `debug` te hoeven uitvoeren.
* **Live Sensorgrafiek & Manuele Besturing (zichtbaar in `MANUAL`):** Een HTML5-canvas dat de 6 sensorbalken, de geïnterpoleerde curve en de berekende lijnpositie in real-time tekent, gecombineerd met touch-vriendelijke rijknoppen en een snelheidsregelaar (`0–100%`).
* **Interactieve Terminal & Commando-lijst:** Live logvenster met klikbare snelcommando's aan de zijkant.

---

### 4. `Memory.h` (Niet-vluchtige EEPROM-opslag)
Een template-bibliotheek (`EEPROM_writeAnything`, `EEPROM_readAnything`, `EEPROM_resetAnything`) waarmee de volledige `param_t`-structuur (cyclustijd, kalibratiewaarden en PID-parameters) als één geheel in het flash/EEPROM-geheugen van de ESP32 wordt bewaard. Hierdoor blijven alle instellingen behouden wanneer de spanning van de robot wordt gehaald.

---

### 5. `SerialCommand.h` & `SerialCommand.cpp` (Commando-parser)
Externe Arduino-bibliotheek (door Steven Cogswell) die binnenkomende tekstregels opdeelt in commando's en argumenten (`set`, `debug`, `calibrate`, `drive`) en koppelt aan de bijbehorende callback-functies in `Cyclus.h`.

---

## Gebruik & Tuning-workflow

1. **Verbinden:** Zet de robot aan, verbind met het Wi-Fi-netwerk **`Linefollower_JVDK`** (wachtwoord: `ESP32_jelle`) en open `http://192.168.4.1` in een browser.
2. **Kalibreren:** Zet de robot op `MANUAL`, plaats de sensoren boven de witte ondergrond en klik op `calibrate white`. Plaats ze daarna boven de zwarte lijn en klik op `calibrate black`.
3. **Cyclustijd instellen:** Laat de robot kort in `RUN` draaien, klik op `debug` om de maximale `calculation time` (in µs) te bekijken en stel `set cycle <us>` in op ongeveer 2× die rekentijd (bijv. `2000` µs).
4. **PID afregelen:**
   * Start met `Ki = 0` en `Kd = 0`, kies een basissnelheid (`power`) en verhoog `Kp` tot de robot de lijn vlot volgt.
   * Voeg `Kd` toe om schommelingen in en na scherpe bochten te dempen en voer daarna `power` verder op.
   * Voeg een kleine `Ki` toe om blijvende afwijkingen op rechte stukken weg te werken.

# Gebruiksaanwijzing

### Opladen / vervangen batterijen
De robot wordt gevoed door twee oplaadbare **18650 Li-ion batterijen** (3,6 V nominaal per cel, 2900 mAh) in serie.
* **Vervangen & opladen:** Zet eerst de hoofdschakelaar (`MT-113`) op **OFF**. Haal de twee 18650-cellen uit de batterijhouder en laad ze op in een geschikte externe Li-ion batterijlader (laadspanning max. 4,2 V per cel).
* **Polariteit:** Let bij het terugplaatsen van de batterijen strikt op de `+` en `-` aanduiding in de batterijhouder om schade aan de step-down converter, H-brug en ESP32 te voorkomen.
* **Wanneer opladen:** Zodra de batterijspanning onder ~6,6 V (3,3 V per cel) zakt of de motoren merkbaar minder koppel leveren bij dezelfde `power`-instelling, moeten de cellen opgeladen worden.

---

### Draadloze communicatie

#### Verbinding maken
De ESP32 zet bij het opstarten automatisch een eigen Wi-Fi Access Point op, waardoor geen router of internetverbinding nodig is:
1. Zet de robot aan met de hoofdschakelaar.
2. Open de Wi-Fi-instellingen op je smartphone of laptop en verbind met het netwerk **`Linefollower_JVDK`**.
3. Voer het wachtwoord **`ESP32_jelle`** in.
4. Zodra de verbinding actief is, gaat de **Wi-Fi-LED** (`GPIO 26`) op de robot continu branden.
5. Open een webbrowser (Chrome, Safari, Edge) en surf naar **`http://192.168.4.1`** om het live WebSocket-dashboard te openen.

#### Commando's
Alle commando's kunnen zowel via het dashboard (klikbare knoppen en invoervelden) als via de ingebouwde terminal of USB-seriële monitor (115200 baud) verstuurd worden:

| Commando | Beschrijving |
| :--- | :--- |
| `debug` | Toont de huidige cyclustijd, alle ingestelde parameters (`power`, `diff`, `Kp`, `Ki`, `Kd`), de gekalibreerde zwart/wit-sensorwaarden en de maximale `calculation time` (in µs). |
| `set state run` | Start de autonome lijnvolg-cyclus (`RUN`). |
| `set state stop` | Stopt de motoren en pauzeert de regellus (`STOP`). |
| `set state manual` | Zet de robot in manuele test- en kalibratiemodus (`MANUAL`) met live sensorgrafiek en rijknoppen. |
| `set cycle <µs>` | Stelt de cyclustijd van de PID-lus in microseconden in (bv. `set cycle 2000`) en herschaalt `Ki` en `Kd` automatisch mee. |
| `set power <0..255>` | Stelt de basissnelheid (PWM-duty cycle) van de motoren in. |
| `set diff <0..1>` | Bepaalt de snelheidsverdeling in bochten: `0` = enkel binnenste wiel vertraagt, `0.5` = gelijke verdeling, `1` = enkel buitenste wiel versnelt. |
| `set Kp <0..>` | Stelt de proportionele versterking (`Kp`) in. |
| `set Ki <0..>` | Stelt de integrerende versterking (`Ki`, in s⁻¹) in. |
| `set Kd <0..>` | Stelt de differentiërende versterking (`Kd`, in s) in. |
| `calibrate white` | Kalibreert de 6 sensoren op de witte ondergrond en slaat de waarden op in EEPROM. |
| `calibrate black` | Kalibreert de 6 sensoren op de zwarte lijn en slaat de waarden op in EEPROM. |
| `drive <dir> <0..255>` | Stuurt de robot manueel aan in `MANUAL`-modus (`forward`, `backward`, `left`, `right`, `stop`). |

---

### Kalibratie
Voer een kalibratie uit telkens wanneer de robot op een nieuw parcours of onder gewijzigd omgevingslicht rijdt:
1. Zet de robot in **`MANUAL`**-modus (via het dashboard of `set state manual`).
2. Plaats de sensor-array volledig boven het **witte gedeelte** van het parcours (geen enkele sensor boven de zwarte lijn).
3. Klik op **`calibrate white`** (of typ dit in de terminal). De ESP32 neemt per sensor het gemiddelde van 10 metingen en slaat deze op.
4. Schuif de robot vervolgens zodat alle 6 sensoren één voor één (of gelijktijdig) boven de **zwarte lijn** gepositioneerd worden en klik op **`calibrate black`**.
5. Controleer in de live sensorgrafiek van de `MANUAL`-modus of wit rond `0` ligt, zwart rond `1000` ligt en de berekende positie `0.00 mm` aangeeft wanneer de lijn exact in het midden onder de robot ligt.
6. Alle kalibratiewaarden worden automatisch bewaard in het niet-vluchtige **EEPROM**-geheugen en blijven behouden na uitschakelen.

---

### Settings
De robot rijdt stabiel met volgende parameters (getest op een cyclustijd van 2000 µs):

* **`cycle`:** `2000` µs (500 Hz, ruim boven de maximale rekentijd van ~800 µs)
* **`power`:** `75` (stabiele basissnelheid met P-regeling, gemiddeld 0,33 m/s; verder op te voeren in combinatie met `Kd`)
* **`diff`:** `0.30`
* **`Kp`:** `7.0`
* **`Ki`:** `0.0` (optioneel licht verhogen naar `0.5` tegen blijvende volgfout)
* **`Kd`:** `0.0` (optioneel opbouwen richting `0.10 – 0.15` om bochten bij hogere `power` te dempen)

---

### Start / Stop button
* **Locatie:** De fysieke drukknop bevindt zich op de bovenzijde van de printplaat en is verbonden met **`GPIO 14`** (met een 10 kΩ pull-down weerstand).
* **Werking:** De knop is gekoppeld aan een hardware-interrupt (`RISING`) met een softwarematige debounce van 200 ms om contactdender te voorkomen:
  * Staat de robot in **`STOP`**, dan schakelt één druk op de knop de robot direct naar **`RUN`** (de status-LED gaat branden en de robot vertrekt).
  * Staat de robot in **`RUN`** of **`MANUAL`**, dan schakelt één druk op de knop de robot onmiddellijk naar **`STOP`** (alle motoren vallen stil en de status-LED gaat uit).

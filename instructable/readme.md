# Instructable

Een instructable is een stappenplan - zonder verdere uitleg - hoe je vertrekkend van de bill of materials en gebruik makend van de technische tekeningen de robot kan nabouwen. Ook de nodige stappen om de microcontroller te compileren en te uploaden staan beschreven.

### Stap 1: Componenten verzamelen
* Verzamel of bestel alle componenten uit de Bill of Materials[cite: 35].

### Stap 2: Mechanische assemblage
1. Bevestig de twee Aslong JGA12-N20-30 motoren op het chassis met behulp van de N20-motorbeugels en M2-bouten en -moeren.
2. Schuif de DFRobot 42x19mm wielen op de motorassen.
3. Monteer het 3/8" metalen zwenkwieltje (*Ball Caster*) vooraan onder het chassis met M2-schroeven en afstandsbusjes.
4. Monteer de QTR-8A reflectiesensor-array vooraan onder het chassis op enkele millimeters boven het rijoppervlak met M2-afstandsbusjes en schroeven.
5. Bevestig de 2x18650 batterijhouder en de 80x120mm printplaat op de bovenzijde van het chassis.

### Stap 3: Elektronica & Bedrading op de printplaat
1. Soldeer de male pin-headers op de printplaat voor de ESP32 DevKit V1, DRV8833 H-brug, MINI 560 PRO buck converter en QTR-8A sensor-array.
2. Plaats de MT-113 tuimelschakelaar in serie tussen de positieve pool van de 2x18650 batterijhouder en de centrale voedingsrail (`V_bat`).
3. Sluit de voedingsrail aan op de ingang van de MINI 560 PRO step-down converter (5 V uitgang) en op `VM` van de DRV8833[cite: 35].
4. Plaats de Philips 47 µF - 385 V elektrolytische ontkoppelcondensator parallel over de voedingsrail (let op de polariteit: min-pool aan `GND`)[cite: 34, 35].
5. Verbind de 5 V uitgang van de MINI 560 PRO met `VIN` van de ESP32 en de `3V3`-pin van de ESP32 met `VCC` van de QTR-8A sensor-array[cite: 35].
6. Verbind alle massa-aansluitingen (`GND`) van batterij, buck converter, ESP32, DRV8833 en sensoren met elkaar[cite: 35].
7. Sluit de motoren en H-brug (DRV8833) aan volgens het elektrisch schema[cite: 26, 35]:
   * `GPIO 19` → `AIN1`[cite: 26, 35]
   * `GPIO 21` → `AIN2`[cite: 26, 35]
   * `GPIO 22` → `BIN1`[cite: 26, 35]
   * `GPIO 23` → `BIN2`[cite: 26, 35]
   * `GPIO 18` → `EEP` (`nSLEEP`)[cite: 26, 35]
   * `AOUT1` / `AOUT2` → Linkermotor[cite: 25, 26]
   * `BOUT1` / `BOUT2` → Rechtermotor[cite: 25, 26]
8. Sluit de 6 middelste analoge uitgangen van de QTR-8A sensor-array aan op de ADC1-pinnen van de ESP32[cite: 26, 35]:
   * Sensor 1 t.e.m. 6 → `GPIO 36, 39, 34, 35, 32, 33`[cite: 26, 35]
9. Sluit de drukknop (met 10 kΩ pull-down weerstand) aan op `GPIO 14`[cite: 26, 35].
10. Sluit de status-LED (met 470 Ω voorschakelweerstand) aan op `GPIO 27` en de Wi-Fi-LED (met 150 Ω voorschakelweerstand) op `GPIO 26`[cite: 26].

### Stap 4: Software voorbereiden & Bibliotheken installeren
1. Download of kloon deze GitHub-repository en open `LineFollower_Code.ino` in de **Arduino IDE**[cite: 26].
2. Zorg ervoor dat de volgende bestanden in dezelfde projectmap staan:
   * `LineFollower_Code.ino`[cite: 26]
   * `Cyclus.h`[cite: 25, 26]
   * `Memory.h`[cite: 25, 27]
   * `SerialCommand.h` & `SerialCommand.cpp`[cite: 25, 28, 29]
   * `webpage.html.h`[cite: 26, 30]
3. Installeer in de Arduino IDE via *Boards Manager* het **ESP32** board-pakket (Espressif Systems) en selecteer **DOIT ESP32 DEVKIT V1** als board.
4. Installeer de volgende vereiste bibliotheken (via *Library Manager* of ZIP-import)[cite: 26]:
   * `ESPAsyncWebServer`[cite: 26]
   * `AsyncTCP` (vereist voor ESP32)

### Stap 5: Compileren & Uploaden naar de ESP32
1. Verbind de ESP32 via een USB-kabel met de computer.
2. Selecteer de juiste COM-poort onder *Tools > Port*.
3. Klik op **Verify** (compileren) om te controleren op fouten en klik vervolgens op **Upload** om de firmware naar de ESP32 te flashen.
4. Open de Seriële Monitor op **115200 baud** en controleer of `ready` verschijnt[cite: 26].

### Stap 6: Eerste opstart, Kalibratie & Testen
1. Plaats twee opgeladen 18650 Li-ion batterijen in de batterijhouder en zet de tuimelschakelaar aan.
2. Verbind met een smartphone of laptop met het Wi-Fi-netwerk **`Linefollower_JVDK`** (wachtwoord: `ESP32_jelle`) en surf naar `http://192.168.4.1`[cite: 26].
3. Zet de robot op **MANUAL** en controleer via de rijknoppen of beide motoren de juiste richting uit draaien[cite: 25, 30].
4. Plaats de sensoren boven de witte ondergrond en voer `calibrate white` uit; plaats ze daarna boven de zwarte lijn en voer `calibrate black` uit[cite: 25, 30].
5. Stel de cyclustijd en startparameters in via de terminal of het parameterpaneel (bv. `set cycle 2000`, `set power 75`, `set diff 0.3`, `set Kp 7`, `set Ki 0`, `set Kd 0`)[cite: 25, 30].
6. Plaats de robot op de zwarte lijn en druk op de fysieke startknop of klik op **RUN** in het webdashboard[cite: 25, 26, 30].

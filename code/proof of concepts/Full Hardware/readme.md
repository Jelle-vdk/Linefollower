# POC: Systeemintegratie & Web Dashboard

Proof of Concept waarin de ESP32 als Wi-Fi Access Point fungeert om alle inputs en outputs van de linefollower draadloos via een webpagina te testen en te monitoren.

---

## Doel
Aantonen dat alle hardwarecomponenten tegelijkertijd en zonder interferentie draadloos kunnen worden aangestuurd en uitgelezen via een HTML/JavaScript webinterface.

---

## Functionaliteiten

* **Aansturing (Web -> ESP32):**
  * Motoren (M1 & M2): aan/uit, richting (vooruit/achteruit) en PWM-snelheid (0–100%) via de DRV8833 op 20 kHz.
  * Status-LEDs: LED 1 (Oranje) en LED 2 (Blauw) afzonderlijk in- en uitschakelen.
* **Uitlezing (ESP32 -> Web):**
  * Real-time analoge waarden van 6 QTR-8A sensoren (live balkgrafieken, schaal 0–4095).
  * Status van de fysieke drukknop (OPEN / INGEDRUKT).
  * Automatische updates elke 150 ms via JSON (`/status`).

---

## Pinbezetting

| Component | Pinnen | Type | Functie |
| :--- | :--- | :--- | :--- |
| **DRV8833** | GPIO 18 | Digitaal Uit | Sleep-pin (`HIGH` = aan) |
| | GPIO 19, 21 | LEDC PWM (20 kHz) | Motor 1 (IN1, IN2) |
| | GPIO 22, 23 | LEDC PWM (20 kHz) | Motor 2 (IN3, IN4) |
| **LEDs** | GPIO 27, 26 | Digitaal Uit | Status-LED Oranje & Blauw |
| **Drukknop** | GPIO 14 | Digitaal In | Start/Stop (pull-down) |
| **QTR-8A** | GPIO 36, 39, 34, 35, 32, 33 | Analoog In (ADC1) | Sensoren S1 t/m S6 |

---

## Gebruik & Verbinding

1. Flash de firmware naar de ESP32 en sluit de hardware aan.
2. Verbind via Wi-Fi met:
   * **SSID:** `Linefollower_JVDK`
   * **Wachtwoord:** `ESP32_jelle`
3. Open de browser en surf naar: `http://192.168.4.1`

---

## Status & Conclusie
* **Resultaat:** Geslaagd. Bidirectionele communicatie via Wi-Fi werkt stabiel; motoren, sensoren en indicatoren reageren direct.
* **Aandachtspunt:** Motoren starten mechanisch pas vanaf ~40% PWM (op te vangen met software deadband-compensatie in de PID-fase).
* **Volgende stap:** Hardware definitief solderen, sensor-kalibratielogica implementeren en het PID-lijnvolgalgoritme bouwen.
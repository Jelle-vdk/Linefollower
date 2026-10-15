# Draadloze communicatie proof of concept

Minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een smartphone/laptop, gebruik makend van een webserver via een lokaal Wi-Fi Access Point.
<br />

### Configuratie
* **Mode:** Wi-Fi Access Point (`WIFI_AP`).
* **SSID:** `Linefollower_JVDK` (geen extern netwerk of router vereist).
* **IP-adres:** `192.168.4.1`
* **Communicatie:** HTTP Webserver (duplex data-uitwisseling via GET/POST requests).

### Opmerkingen
* De ESP32 zet zelfstandig een lokaal Wi-Fi-netwerk op, waardoor er geen afhankelijkheid is van een extern netwerk.
* De communicatie is bidirectioneel (duplex): de smartphone/laptop kan commando's of parameters (zoals snelheid/PID-waarden) naar de ESP32 sturen, en de ESP32 stuurt live statusdata (zoals sensordata of status) terug naar de webpagina.

### Gebruiksaanwijzing
1. Schakel de ESP32 in.
2. Maak met de smartphone of laptop verbinding met het Wi-Fi-netwerk `Robot-AP`.
3. Open een browser en surf naar `http://192.168.4.1`.
4. Gebruik de interface om een commando te versturen en controleer de ontvangen statusdata op het scherm en in de seriële monitor.


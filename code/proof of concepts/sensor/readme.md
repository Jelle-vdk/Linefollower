# Sensoren proof of concept

Minimale opstelling die aantoont dat 6 sensoren van de Pololu QTR-8A afzonderlijk analoog worden uitgelezen via `analogRead()`.

De analoge signalen zijn verbonden met 6 ADC1-pinnen van de ESP32 (GPIO 36, 39, 34, 35, 32 en 33). De ADC staat geconfigureerd op een 12-bit resolutie (0–4095) voor het volledige meetbereik. De ruwe sensorwaarden worden continu naar de seriële monitor gestuurd om het contrast tussen de witte ondergrond en de zwarte lijn te verifiëren.

### Hardwarekeuze: 6 sensoren
Op de `DOIT ESP32 DevKit V1 (30-pins)` zijn fysiek slechts 6 pinnen van **ADC1** beschikbaar. Omdat de pinnen van **ADC2** door de Wi-Fi-driver worden geblokkeerd zodra Wi-Fi actief is, worden de 6 centrale sensoren rechtstreeks op ADC1 aangesloten. Hierdoor blijft Wi-Fi storingsvrij bruikbaar voor telemetrie zonder dat een extra multiplexer nodig is.

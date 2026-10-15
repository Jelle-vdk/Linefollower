# Sensoren proof of concept

Dit is een minimale code die aantoont dat minimaal 6 sensoren van de Pololu QTR-8A onafhankelijk van elkaar analoog kunnen worden uitgelezen.

De analoge uitgangen van de sensoren zijn verbonden met 6 ADC-pinnen van de ESP32 (ADC1). De ADC is ingesteld op een maximale resolutie van 12-bit (waarden van 0 tot 4095) en de juiste verzwakking (attenuation) om het volledige spanningsbereik van de converter te benutten.

In de code worden de 6 sensoren afzonderlijk uitgelezen via `analogRead()` en worden de ruwe meetwaarden rechtstreeks naar de seriële monitor geprint om het verschil tussen een witte ondergrond en de zwarte lijn zichtbaar te maken.

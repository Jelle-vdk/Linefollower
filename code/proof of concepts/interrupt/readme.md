# Start/stop interrupt proof of concept

Dit is een minimale code die de correcte werking van een start/stop drukknop aantoont met behulp van een hardware interrupt.

De drukknop is aangesloten op een GPIO-pin met een hardwarematige pull-down weerstand. Bij het indrukken triggert een hardware interrupt (RISING edge) een ISR-functie. Hierin wordt een boolean (`running`) omgeschakeld (toggle):
* **1ste druk:** `running` wordt `true` (start) en de statusled gaat blinken.
* **2de druk:** `running` wordt `false` (stop) en de statusled gaat uit.

!! We maken ook gebruik van een softwarematige debounce om denderen (contact bounce) te voorkomen, zodat `running` niet meerdere keren ongewild omschakelt bij één enkele druk

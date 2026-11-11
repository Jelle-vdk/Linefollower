# basis programma

volgende zaken werken samen in dit template project  

* instelbare parameters mbv. SerialCommands 
  * cyclus tijd
  * debug on/off
  * de status: kan stop/run/manual zijn (word met een led gevisualiseerd)
* parameters worden opgeslagen in eeprom geheugen
* run/stop kan ook geactiveerd worden mbv. een externe interrupt

de EEPROM en SerialCommand zitten in aparte bibliotheken,
maar ook de cyclus zit in een aparte bibliotheek.
Deze word dan uit het hoofdprogramma steeds aangeroepen als het actief is.

! de seriele commando's kunnen wel altijd gebruikt worden.

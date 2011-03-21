Did I Already Take This Pill?
RFID Meter: pill dispensation manager and auto-switching night light

MOTIVATION

When taking her battery of medicines, my mother occasionally loses - or, lost - track of which ones she had already taken. This aims to keep track of what's been taken (and how recently); and also to provide a night light, as long as it's taking up an electrical outlet.

OVERVIEW

The meter consists of an Arduino connected to an RFID reader, along with various switches and LEDs and a speaker for input and feedback. An RFID tag is attached to each pill bottle. Before taking any pill, one scans the bottle; the meter either announces 'no' (it was taken too recently); or announces 'yes' and records that the pill is being taken. (An announcement is either a red or a green LED accompanied by a short melody played on the speaker.)

DETAILS

For a video and photos, construction notes, and circuit schematic, see:
http://www.markfickett.com/stuff/artPage.php?id=373

CODE OVERVIEW

rfidmeter.pde : Coordinate the other modules, and directly control/read the LEDs, speaker, and buttons.
Meters : Manage the core did-I-already-take-it information; store an ID a timestamp pairs, and read from / write to EEPROM.
ID12 : Interface with the RFID reader, an Innovations ID-12.
MomentaryButton : Provide a simple momentary switch interface.
NightLight : Read the ambient light sensor and control switching and smooth transitions of the night light LEDs.


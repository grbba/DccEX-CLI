DCC++ EX Commandline Interface 
==============================

Main functionality:

- Modelrailroad layout builder to be used in conjunction with the DCC++ EX Commandstation:

    Model railroad layouts are described by the means of a JSON Schema and associated JSON files.
    The tool reads those files and validates the layout against the schema and then calculates all
    possible direct paths for a train taking into account the geometry of the track.

    The application, when run in interactive mode, will provide a cli for the Dcc-EX commandstation to upload
    the definition for automation purposes but also allows for issuing individual commands recognized by the 
    commandstation

- Commandstations management & configuration
- Commandstation operations 

Running the Commandline Interface
=================================

``dcccli``'s usage looks like::

    $ ./dcccli [OPTIONS]

.. option:: -h, --help

   Prints help information on options and exits.

.. option::  -s,--schema text:file

   Path/name of the DCC++ EX Layout Schema file. If this file is omitted we will look for the the schema file in the current 
   directory. If not found the system will continue but no validation of the layout will be done and this may yield
   unpredictable results.

.. option::  -l,--layout text:file 

   Path/name of the modelrailroad layout file.

.. option::   -v,--verbose text 

   Verbose settings. Can be one of ``[silent|info|warning|debug|trace]``. Default is set to ``info``, ``trace`` will provide the most verbose output.

.. option:: -i,--interactive 

   Interactive mode; Opens a shell from which commands can be issued. Once the cli has started type ``help`` at the prompt for more information


Upload
------

In order to use the dcccli to upload the standard commandtation binary to your supported arduino platform the follwoing can be used

.. option:: -m,--mcu text                         

    set the mcu architecture can be one of uno, mega or nano
    
.. option:: -u,--upload Needs: --mcu --port       

    upload the cs code to the mcu set in -m or --mcu connected to the port -p

Connecting
----------

To connect to your commandstation when starting dcccli use the following options. This can not be used at thesame time as the -u option

.. option:: -p,--port text

    set the port to which the arduino is connected for uploading

.. option::  -c,--commandstation Needs: --port     

    Connect to the commandstation either on the serial port (-p) of the DCC++ EX commandstation. If -i has been specified as well, after sending, 
    the interactive commandline interface will be available.

.. option::  -b,--baud INT                        

    Set the baud rate for serial connection. If omitted the default of 115200 is used

.. toctree::
    :maxdepth: 1
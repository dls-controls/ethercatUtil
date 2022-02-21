ethercatUtil
============

This support module aims to provide GUI support via additional records and
through organisation of ethercat modules into groups of widgets.

Module types
------------

There are many different module types in use at Diamond, but broadly speaking
they fall into one of the following general types:

* Analogue input
* Analogue output
* Binary input (digital and relay)
* Binary output (digital and relay)
* Power supply terminal

Builder support uses base classes based on these general types to provide the
records and set of widgets used to control or read values from a particular
module.

### Multi-purpose modules

Modules such as the ELM3704 provide additional functionality by being able to
be configured for different types of measurements - from strain gauges to 
thermocouples, temperature sensors and standard analogue voltages.

Adding a new module
-------------------

The steps are hopefully quite simple, assuming this device is a standard 
fixed-purpose module:

* Identify type of module from the types above
* Add a class instance in the appropriate builder Python file
    - Set the class name to the name of the module
    - Inherit from the parent type, e.g. AnalogInputModule for an analog input
    module
    - Call `__init__` of the parent class
        + Pass `name`, `slave`, `P` and `R` standard arguments
        + Identify the pattern name of the entry for setting or reading the
        primary value of a channel (i.e. the analogue voltage reading or turning
        on/off the digital output signal) - this is the `value_entry` argument
        + Set measurement type to the main purpose of the module/channel
        + Set the measurement subtype to describe range of values or other 
        additional information.

If the module is general purpose (like the ELM3704) - then you may want
to write an asynPortDriver which can present a friendly interface to the
user and translate that to the relevant SDO values to set over the SDO
asyn port.

### To do

* Test module with actual signals
* Add support for configuring ELM3704 multi-purpose modules
    * Add reset option
* Create CSS screens
    * Fix issue with dynamically changing MBBI/O labels
* Add optional logic for copying scale and offset values to an AreaDetector scale plugin
* Add documentation
* Add change log
* Autosave aliases and settings and check other PVs

### Done

* Add basic AsynPortDriver for ELM3704
    * Connects an AsynPortClient to the SDO port created from the SDO requests
    * Supports setting main interface setting for measurement type
    * Added secondary screen for detailed settings
        * Sensor supply
        * Scaler
        * RTD element
        * TC element
        * Status string


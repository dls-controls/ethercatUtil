Changelog
=========

All notable changes to this project will be documented in this file.

The format is based on `Keep a Changelog <https://keepachangelog.com/en/1.0.0/>`_.

`0-5 <../../compare/0-4...0-5>`_ - 2022-04-13
---------------------------------------------

Added:

- EL3104 analogue input module

`0-4 <../../compare/0-3...0-4>`_ - 2022-04-01
---------------------------------------------

Changed:

- Refactored ELM3704 properties into separate class
- Started working on getting ELM3704 state at IOC startup

Fixed:

- Bug with InputChannelScalePluginSync where multiple instances of the record would overwrite
  each other.

`0-3 <../../compare/0-2...0-3>`_ - 2022-03-23
---------------------------------------------

Changed:

- Sync record now processes during IOC initialisation

Fixed:

- ELM3704
    - IEPE mode now sets sensor supply to local control mode as expected by module
    - Correct TC element options based on CJC or 80mV mode are now displayed
    - Now correctly updates TC element option when changing to TC mode (instead of RTD element)


`0-2 <../../compare/0-1...0-2>`_ - 2022-03-18
---------------------------------------------

Added:

- Template to synchronise scale and offset values of input modules to other records


`0-1 <../../releases/tag/0-1>`_ - 2022-03-17
--------------------------------------------

- Initial release

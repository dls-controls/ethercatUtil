from iocbuilder import Device
from iocbuilder.arginfo import makeArgInfo, Simple


# Core builder functionality requires these attributes to be set for slave modules
class _SimChainElem:
    def __init__(self, position, type):
        self.position = position
        self.type = type

class SimELM3704SdoPortDriver(Device):
    """Simulate the SDO asynPortDriver for the ELM3704"""

    def __init__(self, name, position=4, type="ELM3704-0000"):
        self.__super.__init__()
        # Store attributes
        self.name = name
        self.port_name = name + "_SDO"
        self.chainelem = _SimChainElem(position, type)

    def InitialiseOnce(self):
        print("# Creating Simulated ELM3704 SDO asynPortDriver")

    def Initialise(self):
        print("SimELM3704SdoPortDriverConfigure(\"{port_name}\")".format(
            port_name=self.port_name
        ))

    ArgInfo = makeArgInfo(
        __init__,
        name = Simple("Name (use as slave module name)", str),
        position = Simple("Module position (e.g. 3)", int),
        type = Simple("Module type (e.g. ELM3704-0000)", str),
    )

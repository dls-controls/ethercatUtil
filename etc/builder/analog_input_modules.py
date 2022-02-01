from iocbuilder import AutoSubstitution
from iocbuilder.arginfo import makeArgInfo
from iocbuilder.modules.asyn import Asyn
from iocbuilder.modules.ethercat.devices import SdoControl, SdoEntryControl

from core import AnalogInputModule, base_arginfo_args

#==============================================================================
# Custom templates
#==============================================================================

class _ELM3704ModuleTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_ELM3704_module.template"


class _ELM3704ChannelTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_ELM3704_channel.template"


class ELM3704(AnalogInputModule):
    ''' GUI for ELM3704 multi-purpose analog input module '''

    Dependencies = (Asyn,)

    DbdFileList = ['ethercatUtil']
    LibFileList = ['ethercatUtil']

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        # Create name for the asynPortDriver port for handling configuration
        self.logic_port = slave.name + ":LOGIC"

        # Call base class init
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="PAISamples1Channel{ch}.Samples__ARRAY[0]",
            SCAN=SCAN
        )

        # Create SDO interface for configuring module
        self.create_sdo_interface(slave)

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)

    # Override default templates for the ELM3704 GUI
    def make_module_template(self):
        _ELM3704ModuleTemplate(
            name=self.name,
            P=self.p,
            R=self.r,
            POSITION=self.position,
            TYPE=self.type,
            PORT=self.port,
            SCAN=self.scan
        )

    def make_channel_template(self, channel, entry):
        _ELM3704ChannelTemplate(
            P=self.p,
            R=self.r,
            CHANNEL=channel,
            PORT=self.port,
            LOGICPORT=self.logic_port,
            ENTRY=entry,
            SCAN=self.scan
        )

    def InitialiseOnce(self):
        print("# Creating ELM3704 driver for handling configuration logic")

    def Initialise(self):
        print(
            "ELM3704DriverConfigure(\"{logic_port}\", \"{slave_port}_SDO\")".format(
                logic_port=self.logic_port, slave_port=self.port
            )
        )

    def create_sdo_interface(self, slave):
        """
        Create the SDO Control and SDO Entry Control instances

        Instantiating these objects creates the ecSdoAsyn object in ecAsyn.cpp of the
        Ethercat support module. This creates our {slave_port}_SDO asyn port to be able
        to change the settings on the ELM3704 module.

        NOTE: indices and subindices have been converted from hex in the manual to
        decimal.
        """

        # Settings index for each channel
        settings_indices = {
            "CH1": 32768,
            "CH2": 32784,
            "CH3": 32800,
            "CH4": 32816,
        }

        for channel, index in settings_indices.items():
            # Create SdoControl instance for interface to index for main settings
            settings_name = ":{channel}:Settings".format(channel=channel)
            sdo_control_settings = SdoControl(self.name + settings_name, slave, index)

            # Create the entries for each parameter under main settings via subindex
            # Interface 0x80n0:01
            entry_name = settings_name + ":Interface"
            asyn_parameter_name = "{channel}:Interface".format(channel=channel)
            description = "{channel} interface".format(channel=channel)
            subindex = 1
            bit_length = 16
            SdoEntryControl(entry_name, sdo_control_settings, asyn_parameter_name, description, subindex, bit_length)

            # Sensor supply 0x80n0:02
            entry_name = settings_name + ":SensorSupply"
            asyn_parameter_name = "{channel}:SensorSupply".format(channel=channel)
            description = "{channel} sensor supply voltage".format(channel=channel)
            subindex = 2
            bit_length = 16
            SdoEntryControl(entry_name, sdo_control_settings, asyn_parameter_name, description, subindex, bit_length)


            # RTD element 0x80n0:14

            # TC element 0x80n0:15

            # Scaler 0x80n0:2E
            entry_name = settings_name + ":Scaler"
            asyn_parameter_name = "{channel}:Scaler".format(channel=channel)
            description = "{channel} scaling".format(channel=channel)
            subindex = 46
            bit_length = 16
            SdoEntryControl(entry_name, sdo_control_settings, asyn_parameter_name, description, subindex, bit_length)

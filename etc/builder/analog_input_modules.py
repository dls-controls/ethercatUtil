from iocbuilder import AutoSubstitution
from iocbuilder.arginfo import ArgInfo, makeArgInfo
from iocbuilder.modules.asyn import Asyn

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
        # Store name for asynPortDriver port that will handle configuration
        # logic
        self.logic_port = slave.name + ":LOGIC"
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="PAISamples1Channel{ch}.Samples__ARRAY[0]",
            measurement_type=0,
            measurement_subtype=0,
            SCAN=SCAN
        )

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
            TYPE=self.measurement_type,
            SUBTYPE=self.measurement_subtype,
            PORT=self.port,
            LOGICPORT=self.logic_port,
            ENTRY=entry,
            SCAN=self.scan
        )

    def InitialiseOnce(self):
        print("# Creating ELM3704 driver for handling configuration logic")

    def Initialise(self):
        print("ELM3704DriverConfigure(\"{port}\")".format(port=self.logic_port))

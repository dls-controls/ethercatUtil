from iocbuilder import AutoSubstitution, Device
from iocbuilder.arginfo import Choice, Ident, makeArgInfo, Simple
from iocbuilder.modules.calc import Calc
from iocbuilder.modules.ethercat import EthercatSlave

#==============================================================================
# ArgInfo variables
#==============================================================================

scan_periods = [
    ".1 second",
    ".2 second",
    ".5 second",
    "1 second",
    "2 second",
    "5 second",
    "10 second"
]

base_arginfo_args = {
    "name": Simple("Object name", str),
    "slave": Ident("Slave module", EthercatSlave),
    "P": Simple("PV prefix", str),
    "R": Simple("PV suffix", str),
    "SCAN": Choice("Scan period of raw value", scan_periods)
}


#==============================================================================
# Module templates
#==============================================================================

class _EthercatGuiDigitalInputModuleTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_digital_input_module.template"

class _EthercatGuiDigitalOutputModuleTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_digital_output_module.template"

class _EthercatGuiAnalogInputModuleTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_analog_input_module.template"

class _EthercatGuiAnalogOutputModuleTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_analog_output_module.template"

class _EthercatPowerSupplyModuleTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_power_supply_module.template"


#==============================================================================
# Channel templates
#==============================================================================

class _EthercatGuiInputChannelTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_input_channel.template"

class _EthercatGuiDigitalOutputChannelTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_digital_output_channel.template"

class _EthercatGuiAnalogOutputChannelTemplate(AutoSubstitution):
    TemplateFile = "ethercat_gui_analog_output_channel.template"


#==============================================================================
# Base module class
#==============================================================================

class EthercatSlaveGui(Device):
    '''
    Generic top-level class for creating GUI for reading channel values for a slave
    module.
    '''

    # Module dependencies
    Dependencies = (Calc,)

    def __init__(
            self,
            name,
            slave,
            P,
            R,
            channels=4,
            value_entry=None,
            measurement_type="Default",
            measurement_subtype="Default",
            SCAN="1 second"):
        # Initialise base class
        self.__super.__init__()

        # Main properties
        self.name = name
        self.p = P
        self.r = R
        self.channels = channels
        self.value_entry = value_entry
        self.measurement_type = measurement_type
        self.measurement_subtype = measurement_subtype
        self.scan = SCAN

        # Properties from slave module object
        self.position = slave.chainelem.position
        self.type = slave.chainelem.type
        self.port = slave.name

        # Module-level template
        self.make_module_template()

        # Channel-level templates
        for channel in range(1, self.channels+1):
            entry = self._get_entry_name(channel)
            self.make_channel_template(channel, entry)

    def _get_entry_name(self, channel):
        if self.value_entry:
            # Substitute channel number if pattern is in string
            return self.value_entry.format(ch=channel)
        else:
            return None

    def make_module_template(self):
        raise NotImplementedError(
            "{name}: no module template found. Please use one of the module type classes.".format(
                name=self.name
        ))

    def make_channel_template(self, channel, entry):
        raise NotImplementedError(
            "{name}: no channel template found. Does this module have channels?".format(
                name=self.name
        ))

    ArgInfo = makeArgInfo(
        __init__,
        channels             = Simple("Number of channels", int),
        value_entry          = Simple("PDO entry name for raw value. Can use {ch} to sub in channel number", str),
        measurement_type     = Simple("Measurement type", str),
        measurement_subtype  = Simple("Measurement subtype", str),
        **base_arginfo_args
    )


#==============================================================================
# Base module type classes
#==============================================================================

class DigitalInputModule(EthercatSlaveGui):

    def make_module_template(self):
        _EthercatGuiDigitalInputModuleTemplate(
            name=self.name,
            P=self.p,
            R=self.r,
            POSITION=self.position,
            TYPE=self.type,
            PORT=self.port,
            SCAN=self.scan
        )

    def make_channel_template(self, channel, entry):
        _EthercatGuiInputChannelTemplate(
            P=self.p,
            R=self.r,
            CHANNEL=channel,
            TYPE=self.measurement_type,
            SUBTYPE=self.measurement_subtype,
            PORT=self.port,
            ENTRY=entry,
            SCAN=self.scan
        )


class DigitalOutputModule(EthercatSlaveGui):

    def make_module_template(self):
        _EthercatGuiDigitalOutputModuleTemplate(
            name=self.name,
            P=self.p,
            R=self.r,
            POSITION=self.position,
            TYPE=self.type,
            PORT=self.port,
            SCAN=self.scan
        )

    def make_channel_template(self, channel, entry):
        _EthercatGuiDigitalOutputChannelTemplate(
            P=self.p,
            R=self.r,
            CHANNEL=channel,
            TYPE=self.measurement_type,
            SUBTYPE=self.measurement_subtype,
            PORT=self.port,
            ENTRY=entry
        )


class AnalogInputModule(EthercatSlaveGui):

    def make_module_template(self):
        _EthercatGuiAnalogInputModuleTemplate(
            name=self.name,
            P=self.p,
            R=self.r,
            POSITION=self.position,
            TYPE=self.type,
            PORT=self.port,
            SCAN=self.scan
        )

    def make_channel_template(self, channel, entry):
        _EthercatGuiInputChannelTemplate(
            P=self.p,
            R=self.r,
            CHANNEL=channel,
            TYPE=self.measurement_type,
            SUBTYPE=self.measurement_subtype,
            PORT=self.port,
            ENTRY=entry,
            SCAN=self.scan
        )


class AnalogOutputModule(EthercatSlaveGui):

    def make_module_template(self):
        _EthercatGuiAnalogOutputModuleTemplate(
            name=self.name,
            P=self.p,
            R=self.r,
            POSITION=self.position,
            TYPE=self.type,
            PORT=self.port,
            SCAN=self.scan
        )

    def make_channel_template(self, channel, entry):
        _EthercatGuiAnalogOutputChannelTemplate(
            P=self.p,
            R=self.r,
            CHANNEL=channel,
            TYPE=self.measurement_type,
            SUBTYPE=self.measurement_subtype,
            PORT=self.port,
            ENTRY=entry,
            EGU="V",
            EGUF=10.0,
            EGUL=-10.0,
            DRVH=10.0,
            DRVL=-10.0
        )


class PowerSupplyModule(EthercatSlaveGui):

    def make_module_template(self):
        _EthercatPowerSupplyModuleTemplate(
            name=self.name,
            P=self.p,
            R=self.r,
            POSITION=self.position,
            TYPE=self.type,
            SUBTYPE=self.measurement_subtype,
            PORT=self.port,
            SCAN=self.scan,
            POWER_OK="StatusUo.PowerOK",
            OVERLOAD="StatusUo.Overload"
        )


#==============================================================================
# Modules (to be moved)
#==============================================================================

class EL1014(DigitalInputModule):
    ''' GUI for EL1014 24V digital input module '''

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="Channel{ch}.Input",
            measurement_type="Digital input",
            measurement_subtype="24V",
            SCAN=SCAN
        )

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)


class EL1124(DigitalInputModule):
    ''' GUI for EL1124 5V digital input module '''

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="Channel{ch}.Input",
            measurement_type="Digital input",
            measurement_subtype="5V",
            SCAN=SCAN
        )

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)


class EL2124(DigitalOutputModule):
    ''' GUI for EL2124 5V digital output module '''

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="Channel{ch}.Output",
            measurement_type="Digital output",
            measurement_subtype="5V",
            SCAN=SCAN
        )

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)


class EL2624(DigitalOutputModule):
    ''' GUI for EL2624 125V AC / 30V DC relay output module '''

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="Channel{ch}.Output",
            measurement_type="Relay output",
            measurement_subtype="125V AC / 30V DC",
            SCAN=SCAN
        )

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)


class EL4134(AnalogOutputModule):
    ''' GUI for EL4134 analog output module '''

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="AOOutputChannel{ch}.Analogoutput",
            measurement_type="Analog output",
            measurement_subtype="+/-10V",
            SCAN=SCAN
        )

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)


class EL9505(PowerSupplyModule):
    ''' GUI for EL9505 power supply module '''

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            channels=0,
            value_entry=None,
            measurement_type="Power supply",
            measurement_subtype="5V DC",
            SCAN=SCAN
        )

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)


class ELM3704(AnalogInputModule):
    ''' GUI for ELM3704 multi-purpose analog input module '''

    def __init__(self, name, slave, P, R, SCAN="1 second"):
        self.__super.__init__(
            name,
            slave,
            P,
            R,
            value_entry="PAISamples1Channel{ch}.Samples__ARRAY[0]",
            measurement_type="Multi-purpose input",
            measurement_subtype="Unknown",
            SCAN=SCAN
        )

    ArgInfo = makeArgInfo(__init__, **base_arginfo_args)

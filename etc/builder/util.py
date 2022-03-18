from iocbuilder import AutoSubstitution, Device
from iocbuilder.arginfo import ArgInfo, Choice, Ident, makeArgInfo, Simple

from core import EthercatSlaveModule


class _InputChannelScaleOffsetSyncTemplate(AutoSubstitution):
    TemplateFile = "input_channel_scale_offset_sync.template"


class InputChannelScalePluginSync(Device):
    """Synchronise an input channel scale and offset with a scale plugin"""
    def __init__(self, INPUT_MODULE, CHANNEL, PLUGIN_P, PLUGIN_R, PLUGIN_CHANNEL):
        # Initialise base class
        self.__super.__init__()

        # Scale plugin PVs
        output_scale_pv = "{P}{R}CH{CHANNEL}:Scale".format(
            P=PLUGIN_P, R=PLUGIN_R, CHANNEL=PLUGIN_CHANNEL
        )
        output_offset_pv = "{P}{R}CH{CHANNEL}:Offset".format(
            P=PLUGIN_P, R=PLUGIN_R, CHANNEL=PLUGIN_CHANNEL
        )

        # Instantiate template
        _InputChannelScaleOffsetSyncTemplate(
            P=INPUT_MODULE.p,
            R=INPUT_MODULE.r,
            CHANNEL=CHANNEL,
            OUTPUT_SCALE_PV=output_scale_pv,
            OUTPUT_OFFSET_PV=output_offset_pv
        )

    ArgInfo = makeArgInfo(
        __init__,
        INPUT_MODULE=Ident('Input module', EthercatSlaveModule),
        CHANNEL=Choice('Module channel', [1, 2, 3, 4]),
        PLUGIN_P=Simple('Scale plugin PV prefix', str),
        PLUGIN_R=Simple('Scale plugin PV suffix', str),
        PLUGIN_CHANNEL=Simple('Scale plugin channel', int),
    )

from iocbuilder import AutoSubstitution, Device
from iocbuilder.arginfo import ArgInfo, Choice, Ident, makeArgInfo, Simple

from core import EthercatSlaveModule


class _InputChannelScaleOffsetSyncTemplate(AutoSubstitution):
    TemplateFile = "input_channel_scale_offset_sync.template"


class _InputChannelScaleOffsetSyncChannelTemplate(AutoSubstitution):
    TemplateFile = "input_channel_scale_offset_sync_channel.template"


class InputChannelScalePluginSync(Device):
    """Synchronise an input channel scale and offset with up to 2 scale plugin instances"""
    def __init__(
            self,
            INPUT_MODULE,
            CHANNEL,
            PLUGIN_1_P, 
            PLUGIN_1_R,
            PLUGIN_1_CHANNEL,
            PLUGIN_2_P="",
            PLUGIN_2_R="",
            PLUGIN_2_CHANNEL=""):
        # Initialise base class
        self.__super.__init__()

        # Instantiate main template
        self.make_main_template(
            INPUT_MODULE.p,
            INPUT_MODULE.r,
            CHANNEL,
            PLUGIN_1_P,
            PLUGIN_1_R,
            PLUGIN_1_CHANNEL
        )

        # If a second plugin is added instantiate additional template
        if PLUGIN_2_P:
            self.make_additional_channel_template(
                INPUT_MODULE.p,
                INPUT_MODULE.r,
                CHANNEL,
                PLUGIN_2_P,
                PLUGIN_2_R,
                PLUGIN_2_CHANNEL
            )

    @staticmethod
    def get_scale_plugin_scale_pv(plugin_p, plugin_r, plugin_channel):
        return "{P}{R}CH{CHANNEL}:Scale".format(
            P=plugin_p, R=plugin_r, CHANNEL=plugin_channel
        )

    @staticmethod
    def get_scale_plugin_offset_pv(plugin_p, plugin_r, plugin_channel):
        return "{P}{R}CH{CHANNEL}:Offset".format(
            P=plugin_p, R=plugin_r, CHANNEL=plugin_channel
        )

    @staticmethod
    def make_main_template(
        input_module_p,
        input_module_r,
        input_module_channel,
        plugin_p,
        plugin_r,
        plugin_channel
    ):
        # Primary scale plugin PVs
        output_scale_pv = InputChannelScalePluginSync.get_scale_plugin_scale_pv(plugin_p, plugin_r, plugin_channel)
        output_offset_pv = InputChannelScalePluginSync.get_scale_plugin_offset_pv(plugin_p, plugin_r, plugin_channel)

        # Instantiate main template
        _InputChannelScaleOffsetSyncTemplate(
            P=input_module_p,
            R=input_module_r,
            CHANNEL=input_module_channel,
            OUTPUT_SCALE_PV=output_scale_pv,
            OUTPUT_OFFSET_PV=output_offset_pv
        )

    @staticmethod
    def make_additional_channel_template(
        input_module_p,
        input_module_r,
        input_module_channel,
        plugin_p,
        plugin_r,
        plugin_channel
    ):
        # Additional scale plugin PVs
        output_scale_pv = InputChannelScalePluginSync.get_scale_plugin_scale_pv(plugin_p, plugin_r, plugin_channel)
        output_offset_pv = InputChannelScalePluginSync.get_scale_plugin_offset_pv(plugin_p, plugin_r, plugin_channel)

        # Instantiate main template
        _InputChannelScaleOffsetSyncChannelTemplate(
            P=input_module_p,
            R=input_module_r,
            CHANNEL=input_module_channel,
            OUTPUT_SCALE_PV=output_scale_pv,
            OUTPUT_OFFSET_PV=output_offset_pv
        )

    ArgInfo = makeArgInfo(
        __init__,
        INPUT_MODULE=Ident('Input module', EthercatSlaveModule),
        CHANNEL=Choice('Module channel', [1, 2, 3, 4]),
        PLUGIN_1_P=Simple('Scale plugin PV prefix', str),
        PLUGIN_1_R=Simple('Scale plugin PV suffix', str),
        PLUGIN_1_CHANNEL=Simple('Scale plugin channel', int),
        PLUGIN_2_P=Simple('Scale plugin 2 PV prefix', str),
        PLUGIN_2_R=Simple('Scale plugin 2 PV suffix', str),
        PLUGIN_2_CHANNEL=Simple('Scale plugin 2 channel', int),
    )

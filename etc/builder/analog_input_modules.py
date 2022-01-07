from iocbuilder.arginfo import ArgInfo, makeArgInfo

from core import AnalogInputModule, base_arginfo_args


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

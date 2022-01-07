from iocbuilder.arginfo import ArgInfo, makeArgInfo

from core import AnalogOutputModule, base_arginfo_args


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

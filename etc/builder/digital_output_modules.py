from iocbuilder.arginfo import ArgInfo, makeArgInfo

from core import DigitalOutputModule, base_arginfo_args


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

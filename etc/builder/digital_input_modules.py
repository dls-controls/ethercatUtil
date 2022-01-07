from iocbuilder.arginfo import ArgInfo, makeArgInfo

from core import DigitalInputModule, base_arginfo_args


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

from iocbuilder.arginfo import ArgInfo, makeArgInfo

from core import PowerSupplyModule, base_arginfo_args


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

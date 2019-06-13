class MissingCalibrationError(Exception):
    def __init__(self, card, parameter):
        Exception.__init__(
            self,
            'calibration for parameter %s of card %s is missing' % (parameter, card)
        )


def calibrate(card, parameter, value):
    if parameter in card.calibration:
        return value / float(card.calibration[parameter])
    else:
        raise MissingCalibrationError(parameter, card.name)


def calibrated_getter(parameter):
    def wrapper(getter):
        def inner(self):
            if parameter in self.calibration:
                ret = getter(self)
                return ret * self.calibration[parameter]
            else:
                raise MissingCalibrationError(parameter, self.name)
        
        return inner
    return wrapper


def calibrated_setter(parameter):
    def wrapper(setter):
        def inner(self, value):
            return setter(self, calibrate(self, parameter, value))
        return inner
    return wrapper


class TBusCard:
    stack = None
    name = None
    calibration = None
    pending_registers = set()

    def __init__(self, stack, name, calibration):
        self.stack = stack
        self.name = name
        self.calibration = calibration
    
    def queue_register(self, register):
        self.pending_registers.add(register)
    
    def set_bool(self, parameter, value):
        self.stack.set_bool(self.name, parameter, value)
    
    def set_int(self, parameter, value):
        self.stack.set_int(self.name, parameter, value)
    
    def set_float(self, parameter, value):
        self.stack.set_float(self.name, parameter, value)
    
    def get_bool(self, parameter):
        return self.stack.get_bool(self.name, parameter)

    def get_int(self, parameter):
        return self.stack.get_int(self.name, parameter)

    def get_float(self, parameter):
        return self.stack.get_float(self.name, parameter)

    def register_to_bus(self, register, send=True, receive=True):
        return self.stack.register_to_bus(self.name, register, send=send, receive=receive)

    def apply_registers(self):
        for register in self.pending_registers:
            self.register_to_bus(register)
        
        self.pending_registers.clear()
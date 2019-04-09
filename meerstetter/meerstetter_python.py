"""
A python wrapper for the Meerstetter TEC api.
"""
import ctypes as ct
from os import path
from glob import glob
from retrying import retry

ALL_DEVICES = 0
INT_PARAMETERS = {
    'DEVICE_TYPE': 100,
    'SAVE_TO_FLASH': 108,
    'TEMPERATURE_STABLE': 1200,
    'OUTPUT_STATUS': 2010,
    'OUTPUT_SELECTION': 2000,
    'DRIVER_STATUS': 1080,
    'ERROR_NUMBER': 1070,
    'ERROR_INSTANCE': 1071,
    'ERROR_PARAMETER': 1072
}
FLOAT_PARAMETERS = {
    'OBJECT_TEMPERATURE': 1000,
    'TARGET_OBJECT_TEMPERATURE': 1010,
    'CURRENT_LIMITATION': 2030,
    'TEMPERATURE_DEVIATION': 4040,
    'MIN_TIME_IN_WINDOW': 4041,
    'MAX_STABILIZATION_TIME': 4042,
    'STATIC_CURRENT': 2020,
    'STATIC_VOLTAGE': 2021,
    'ACTUAL_CURRENT': 1020,
    'ACTUAL_VOLTAGE': 1021,
    'PROXIMITY_WIDTH': 3002,
    'COARSE_TEMP_RAMP': 3003
}
PARAMETERS = INT_PARAMETERS.copy()
PARAMETERS.update(FLOAT_PARAMETERS)
SPEEDS = [4800, 1000000, 57600]


class DLLException(Exception):
    """
    Raised if the Meerstetter TEC api encountered an error but we don't
    know what happened.
    """


def _to_str(data):
    """Converts a ctypes int8_t array to a python string"""
    new = []

    for char in data:
        if char == '\0':
            break
        new.append(char)

    return ''.join(new)


def _check_exc(exception):
    return isinstance(exception, DLLException)


"""
Connection to Meerstetter sometimes is a little bit unreliable, especially
if multiple devices are connected.
Therefore, all api functions are retried several times by default using this
decorator.
"""
retry_on_dll_error = retry(retry_on_exception=_check_exc, stop_max_attempt_number=5)


class Parameters:
    def __init__(self, parent):
        self.parent = parent

    def __getitem__(self, k):
        return self.parent._get_parameter(k)

    def __setitem__(self, k, v):
        return self.parent._set_parameter(k, v)

    def __iter__(self):
        for parameter in PARAMETERS.keys():
            yield parameter, self[parameter]

    def __contains__(self, parameter):
        return parameter in PARAMETERS


class Meerstetter:
    """
    This class talks to a Meerstetter TEC.

    Usage:

        # this instance talks to all devices that are connected to the usb port
        usb_port = 1
        m = Meerstetter(usb_port)

        # if you just want to talk to a single TEC (address=0 is equivalent to
        # connecting to all devices):
        tec_address = 2
        m.connect(tec_address)

        # if you just want to talk to a single TEC but don't know which USB
        # port is used, you can do this:
        m = Meerstetter.by_address(tec_address)
        # this scans all available USB ports until a TEC with the right address
        # is found
    """
    # for each method of the wrapper, the types of the arguments are defined here
    _cmd_arguments = {
        'open': (ct.c_int, ct.c_int),
        'close': (),
        'get_firmware_identity': (ct.c_int, ct.POINTER(ct.c_char)),
        'get_device_type': (ct.c_int, ct.POINTER(ct.c_int)),
        'set_target_temp': (ct.c_int, ct.c_float),
        'get_temp': (ct.c_int, ct.POINTER(ct.c_float)),
        'get_int_param': (ct.c_int, ct.c_int, ct.POINTER(ct.c_int)),
        'set_int_param': (ct.c_int, ct.c_int, ct.c_int),
        'get_float_param': (ct.c_int, ct.c_int, ct.POINTER(ct.c_float)),
        'set_float_param': (ct.c_int, ct.c_int, ct.c_float),
        'reset_device': (ct.c_int,)
    }

    def __init__(self, port=0, speed=57600):
        """
        Initializes the DLL and Opens a communication port.
        """
        # current TEC address
        self._address = ALL_DEVICES

        assert speed in SPEEDS, 'invalid speed value'

        # connection to the dll
        self._d = ct.CDLL(path.join(path.dirname(__file__) or './', 'wrapper.so'))
        self._init_commands()
        self._d.cmd_open(port, speed)
        self.parameters = Parameters(self)

    @classmethod
    @retry_on_dll_error
    def by_address(cls, address):
        """
        Scans all available USB devices for a Meerstetter with the right address.
        """
        for dev in glob('/dev/ttyUSB*'):
            number = int(''.join([l for l in dev if l.isnumeric()]))
            try:
                m = Meerstetter(number)
                m.connect(address)
                return m
            except:
                continue

        raise Exception('address %d not found' % address)

    def _errcheck(self, result, func, arguments):
        """
        Checks for error code == 0, throws an error otherwise.
        """
        if result != 0:
            raise DLLException(
                _to_str(self._d.get_last_error())
                if (result == 2)
                else 'unknown error'
            )
        return

    def _init_commands(self):
        """
        Set the argument and return value types as specified in `_cmd_arguments`,
        and registers the error handler.
        """
        self._d.get_last_error.restype = ct.POINTER(ct.c_char)

        for cmd, arguments in self._cmd_arguments.items():
            fct = getattr(self._d, 'cmd_%s' % cmd)
            fct.argtypes = arguments
            fct.restype = ct.c_int
            fct.errcheck = self._errcheck

    @retry_on_dll_error
    def connect(self, address=ALL_DEVICES):
        """
        By default, the program talks to all devices that are connected to the
        current bus. Using this method, you can connect to a single device with
        address `address`.
        """
        self._address = address

        # test communication
        try:
            self.get_device_type()
        except:
            self._address = ALL_DEVICES
            raise Exception('failed to connect to address=%d' % address)

    def close(self):
        self._d.cmd_close()

    @retry_on_dll_error
    def get_firmware_identity(self):
        buf = ct.create_string_buffer(b"", 100)
        self._d.cmd_get_firmware_identity(self._address, buf)
        return buf.value

    @retry_on_dll_error
    def get_device_type(self):
        buf = ct.c_int()
        self._d.cmd_get_device_type(self._address, buf)
        return buf.value

    @retry_on_dll_error
    def get_temperature(self):
        buf = ct.c_float()
        self._d.cmd_get_temp(self._address, buf)
        return buf.value

    @retry_on_dll_error
    def get_target_temperature(self):
        return self.parameters['TARGET_OBJECT_TEMPERATURE']

    @retry_on_dll_error
    def temperature_is_stable(self, max_deviation, time=1):
        target = self.get_target_temperature()
        if abs(target - self.get_temperature()) > max_deviation:
            return False
        return True

    @retry_on_dll_error
    def set_target_temperature(self, temp):
        self._d.cmd_set_target_temp(self._address, temp)

    @retry_on_dll_error
    def reset(self):
        self._d.cmd_reset_device(self._address)

    @retry_on_dll_error
    def set_output_enabled(self, value=True):
        self.parameters['OUTPUT_STATUS'] = 1 if value else 0

    @retry_on_dll_error
    def enable_temperature_control(self):
        self.parameters['OUTPUT_SELECTION'] = 2

    @retry_on_dll_error
    def enable_static_current(self):
        self.parameters['OUTPUT_SELECTION'] = 0

    @retry_on_dll_error
    def set_static_current(self, current):
        self.parameters['STATIC_CURRENT'] = current

    @retry_on_dll_error
    def set_static_voltage(self, voltage):
        self.parameters['STATIC_VOLTAGE'] = voltage

    @retry_on_dll_error
    def get_actual_current(self):
        return self.parameters['ACTUAL_CURRENT']

    @retry_on_dll_error
    def get_actual_voltage(self):
        return self.parameters['ACTUAL_VOLTAGE']

    @retry_on_dll_error
    def get_parameter(self, p):
        return self.parameters[p]

    @retry_on_dll_error
    def set_parameter(self, p, v):
        self.parameters[p] = v

    def _check_parameter(self, parameter):
        assert parameter in PARAMETERS, 'unknown parameter %s' % parameter

        code = PARAMETERS[parameter]
        type_ = int if parameter in INT_PARAMETERS else float

        return code, type_

    def _get_parameter(self, parameter):
        code, type_ = self._check_parameter(parameter)

        buf = ct.c_int() if type_ == int else ct.c_float()
        getter = self._d.cmd_get_int_param if type_ == int else self._d.cmd_get_float_param
        getter(self._address, code, buf)
        return buf.value

    def _set_parameter(self, parameter, value):
        code, type_ = self._check_parameter(parameter)
        value = type_(value)

        setter = self._d.cmd_set_int_param if type_ == int else self._d.cmd_set_float_param
        setter(self._address, code, value)


if __name__ == '__main__':
    m = Meerstetter.by_address(ALL_DEVICES)
    print('Identity string: ', m.get_firmware_identity())
    print('Current temperature: ', m.get_temperature())

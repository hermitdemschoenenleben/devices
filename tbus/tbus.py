import struct
import ctypes as ct

from os import path
from functools import reduce

from .cards.dds import DDSCard
from .cards.ta import TACard
from .cards.freq_ctl import FrequencyControlCard
from .buses.ethernet import EthernetBus

CARD_TYPES = {
    'DDS2': DDSCard,
    'tacurrentdriver3': TACard,
    'freqcontrol1': FrequencyControlCard
}

EXCEPTIONS = {
    -1: 'unknown parameter',
    -2: 'unknown card',
    -3: 'unknown stack',
    -4: 'unknown register',
    -5: 'invalid type',
    -6: 'invalid stack file',
    -7: 'invalid card file',
    -8: 'duplicate stack name',
    -9: 'invalid array length',
    -10: 'read pattern mismatch',
    -11: 'missing interface',
    -100: 'unknown'
}


def concat_data(*arrs): 
    return list(
        reduce(
            lambda x, y: x + y,
            zip(*arrs)
        )
    )


def check_success(fct):
    def inner(*args, **kwargs):
        rv = fct(*args, **kwargs)

        if rv == 0:
            return
        else:
            raise Exception(EXCEPTIONS[rv])

    return inner


def ps(s):
    """
    Converts a string to a pascal string.
    """
    return struct.pack('%dp' % (len(s) + 1), s.encode())


def from_ps(chars):
    """
    Converts a pascal string into a python string.
    """
    chars = list(chars)
    length = ord(chars[0])
    return ''.join(
        c.decode() for c in chars[1:length + 1]
    )


def string_buffer(length=1000):
    return (ct.c_char * length)(*(b'0',) * length)


class TBusDLL:
    lib = None

    def __init__(self):

        self.lib = ct.CDLL(
            path.join(
                *path.split(path.abspath(__file__))[:-1],
                'libtbus.so'
            )
        )

    @check_success
    def open_stack(self, stack, filename):
        return self.lib.OpenStack(ps(stack), ps(filename))

    @check_success
    def get_int(self, stack, card, param, value):
        return self.lib.GetIntParam(ps(stack), ps(card), ps(param), value)

    @check_success
    def get_float(self, stack, card, param, value):
        return self.lib.GetFloatParam(ps(stack), ps(card), ps(param), value)

    @check_success
    def get_bool(self, stack, card, param, value):
        return self.lib.GetBoolParam(ps(stack), ps(card), ps(param), value)

    @check_success
    def set_int(self, stack, card, param, value):
        return self.lib.SetIntParam(ps(stack), ps(card), ps(param), int(value))

    @check_success
    def set_float(self, stack, card, param, value):
        return self.lib.SetFloatParam(ps(stack), ps(card), ps(param), ct.c_double(float(value)))

    @check_success
    def set_bool(self, stack, card, param, value):
        return self.lib.SetBoolParam(ps(stack), ps(card), ps(param), int(value))

    @check_success
    def register_to_bus(self, stack, card, register, reg_length, data):
        return self.lib.RegisterToBus(
            ps(stack), ps(card), ps(register), reg_length, data
        )

    @check_success
    def bus_to_param(self, stack, card, register, size, data):
        return self.lib.BusToParam(
            ps(stack), ps(card), ps(register), size, data
        )

    @check_success
    def close_stack(self, stack):
        return self.lib.CloseStack(ps(stack))

    @check_success
    def get_stack_interface(self, stack, interface_type, interface_id):
        return self.lib.GetStackInterface(ps(stack), interface_type, interface_id)

    @check_success
    def get_number_of_cards(self, stack, number):
        return self.lib.GetNumberOfCards(ps(stack), number)

    @check_success
    def get_card_name(self, stack, index, card_name):
        return self.lib.GetCardName(ps(stack), index, card_name)

    @check_success
    def get_card_type(self, stack, index, card_type):
        return self.lib.GetCardType(ps(stack), index, card_type)


class TStack:
    dll = None
    name = None
    bus = None
    calibration = None
    cards = {}
    _reg_length_cache = {}

    def __init__(self, name, filename, calibration={}):
        self.dll = TBusDLL()
        self.open(name, filename)
        self.calibration = calibration

        interface_type, interface_id = self.get_interface()

        if interface_type == 'ethernet':
            self.bus = EthernetBus(self, interface_id)
        else:
            raise Exception('unknown bus')

        self._init_cards()

    def _init_cards(self):
        card_number = ct.pointer(ct.c_uint(0))
        self.dll.get_number_of_cards(self.name, card_number)
        card_number = card_number.contents.value

        for i in range(card_number):
            card_name = string_buffer(1000)
            self.dll.get_card_name(self.name, i, card_name)
            card_name = from_ps(card_name)

            card_type = string_buffer(1000)
            self.dll.get_card_type(self.name, i, card_type)
            card_type = from_ps(card_type).lower()

            if card_type in CARD_TYPES:
                card = CARD_TYPES[card_type](
                    self, card_name, self.calibration.get(card_name, {})
                )
                self.cards[card_name] = card

    def open(self, name, filename):
        self.name = name
        self.dll.open_stack(name, filename)

    def get_int(self, card, param):
        value = ct.pointer(ct.c_int64())
        self.dll.get_int(self.name, card, param, value)
        return value.contents.value

    def get_float(self, card, param):
        value = ct.pointer(ct.c_double())
        self.dll.get_float(self.name, card, param, value)
        return value.contents.value

    def get_bool(self, card, param):
        value = ct.pointer(ct.c_int64())
        self.dll.get_bool(self.name, card, param, value)
        return bool(value.contents.value)

    def set_int(self, card, param, value):
        self.dll.set_int(self.name, card, param, value)

    def set_float(self, card, param, value):
        self.dll.set_float(self.name, card, param, value)

    def set_bool(self, card, param, value):
        self.dll.set_bool(self.name, card, param, value)

    def get_interface(self):
        i_type = string_buffer()
        i_id = string_buffer()
        self.dll.get_stack_interface(self.name, i_type, i_id)
        return from_ps(i_type).lower(), from_ps(i_id)
    
    def _prepare_reg_to_bus(self, card, register):
        # at this point, we don't know how many addresses the register will try to access.
        # The `register_to_bus` method doesn't return this explicitely, but instead
        # expects a buffer it can write to that is sufficiently large.
        # Therefore, by default we initialize a very large buffer. Then we find out how
        # much of this was actually used and cache this value for upcoming calls to the
        # same register. This improves performance significantly if the same call is done
        # several thousand times.
        reg_length_cache_key = (card, register)
        if reg_length_cache_key in self._reg_length_cache:
            reg_length = self._reg_length_cache[reg_length_cache_key]
            reg_length_cached = True
        else:
            reg_length = 1000
            reg_length_cached = False

        a = ct.c_int(reg_length)
        raw = (ct.c_uint * reg_length)(*((0,)*reg_length))

        self.dll.register_to_bus(self.name, card, register, a, raw)
        data = raw[0::4]
        addresses = raw[1::4]
        something = raw[2::4]
        write_or_read = raw[3::4]

        if not reg_length_cached:
            try:
                first_null = addresses.index(0)
            except ValueError:
                first_null = len(addresses)

            addresses = addresses[:first_null]
            data = data[:first_null]
            write_or_read = write_or_read[:first_null]

            self._reg_length_cache[reg_length_cache_key] = first_null * 4

        return addresses, data, write_or_read, something

    def register_to_bus_send(self, card, register):
        addresses, data, write_or_read, something = self._prepare_reg_to_bus(card, register)
        self.bus.send(bytes(
            concat_data(
                addresses, data, write_or_read
            )
        ))
    
    def register_to_bus_rcv(self, card, register):
        addresses, data, write_or_read, something = self._prepare_reg_to_bus(card, register)

        buf = self.bus.receive(3 * len(addresses))

        for i, [address, d, rw] in enumerate(zip(addresses, data, write_or_read)):
            if not rw:
                data[i] = buf[(3 * i) + 1]

        result = concat_data(data, addresses, something, write_or_read)
        result_c = (ct.c_uint * (len(data) * 4))(*result)

        self.dll.bus_to_param(self.name, card, register, len(data), result_c)
    
    def register_to_bus(self, card, register, send=True, receive=True):
        if send:
            self.register_to_bus_send(card, register)
        if receive:
            self.register_to_bus_rcv(card, register)
    
    def register_to_bus_multi(self, cards, registers,
                              before_each_send=lambda x: x,
                              after_each_read=lambda x: x,
                              ignore_recv=False):
        """
        Executes multiple register to bus calls as fast as possible.
        Therefore, at the beginning all data is sent, then all data is received.

        Notice that you can use the `before_each_send` and `after_each_read` hooks.
        """
        for i, [card, register] in enumerate(zip(cards, registers)):
            before_each_send(i)
            self.register_to_bus_send(card, register)
        
        if not ignore_recv:
            for i, [card, register] in enumerate(zip(cards, registers)):
                self.register_to_bus_rcv(card, register)
                after_each_read(i)
        else:
            buf_length = 0

            # this cache is used to speed up things quite a bit by re-using the
            # same buffers again. Normally, each call to `_prepare_reg_to_bus`
            # creates a new buffer but we skip that as we want to ignore the
            # data anyway.
            register_cache = {}

            for i, [card, register] in enumerate(zip(cards, registers)):
                cache_key = (card, register)
                if cache_key in register_cache:
                    cached_data = register_cache[cache_key]
                else:
                    cached_data = self._prepare_reg_to_bus(card, register)
                    register_cache[cache_key] = cached_data

                addresses, data, write_or_read, something = cached_data
                buf_length += 3 * len(addresses)
            
            self.bus.receive(buf_length)
            
    def close(self):
        self.dll.close_stack(self.name)
        self.bus.close()


def DefaultTBus():
    stack_file = '/'.join(__file__.split('/')[:-1] + ['test'])
    return TStack('test', stack_file, calibration={
        'TA1': {
            'is_ta': True,
            'current_A': 1950,
            'current_A_read': 11900
        },
        'TA2': {
            'is_ta': True,
            'current_A': 1827,
            'current_A_read': 11800
        },
        'MO1': {
            'is_ta': False,
            'current_A': 220,
            'current_A_read': 430,
            'current_B': 219,
            'current_B_read': 440
        },
        'MO2': {
            'current_A': 211,
            'current_B': 210
        }
    })


if __name__ == '__main__':
    t = DefaultTBus()

    """SETPOINT = 1800
    print('SETPOINT:', SETPOINT)

    mo = t.cards['TA2']

    for i in [0]:
        mo.set_global_laser_enabled(False)
        mo.set_laser(i, True, SETPOINT, mo.SHUTDOWN_SIGNALS.ALWAYS_ON)

    mo.apply_registers()

    mo.read_laser_status()
    mo.read_status()

    print(mo.get_laser_current_A())

    #print(mo.get_laser_current_B())
    print(mo.get_temperature())
    """
    cnt = t.cards['freq ctl']
    from matplotlib import pyplot as plt

    """_ = []
    for i in range(300):
        cnt.read_slow(2)
        _.append(cnt.get_slow(2))

    plt.plot(_)
    plt.show()"""


    for i in range(10):
        cnt.set_memory_enabled(True if i == 0 else False)
        cnt.set_fifo_enabled(False)
        cnt.set_memory_source(cnt.MEMORY_SOURCES.fast_in)
        cnt.apply_registers()
        print(cnt.read_and_get_fifo_status())

        #cnt.set_memory_enabled(False)
        #cnt.apply_registers()


        plt.plot(cnt.read_and_get_memory_block())
        plt.show()

    """cnt.set_memory_enabled(False)
    cnt.set_fifo_enabled(False)
    cnt.set_memory_source(cnt.MEMORY_SOURCES.fast_in)
    cnt.apply_registers()"""

    t.close()

import numpy as np
from collections import namedtuple
from .card import TBusCard, calibrated_getter, calibrated_setter, calibrate


class TACard(TBusCard):
    def __init__(self, *args, **kwargs):
        TBusCard.__init__(self, *args, **kwargs)
        assert 'is_ta' in self.calibration, 'you have to specify whether current driver card is mo or ta. Set is_ta in calibration parameter'
        self.is_ta = self.calibration['is_ta']

    SHUTDOWN_SIGNALS = namedtuple(
        'TA_SHUTDOWN_SIGNALS',
        ['ALWAYS_ON', 'ALWAYS_OFF', 'TRIGGER_CHANNEL']
    )(14, 13, lambda x: x)

    def set_global_laser_enabled(self, enabled):
        self.set_bool('Global Laser Enable', enabled)
        print('set global laser enabled', enabled)
        self.queue_register('Set Laser Status')

    def set_laser(self, channel, enabled, current, shutdown_signal):
        channel_string = 'A' if channel == 0 else 'B'
        assert channel in [0, 1], 'invalid channel'
        self.set_bool('Laser %s Disable' % channel_string, not enabled)
        self.set_float(
            'Current %s' % channel_string,
            calibrate(self, 'current_%s' % channel_string, current)
        )
        self.set_int('Signal source %s' % channel_string, shutdown_signal)
        self.queue_register('Set Current %s' % channel_string)
        print('set laser', channel)

    @calibrated_getter('current_A_read')
    def get_laser_current_A(self):
        return self.get_float('Read_A')

    @calibrated_getter('current_B_read')
    def get_laser_current_B(self):
        return self.get_float('Read_B')

    def get_temperature(self):
        volt = self.get_float('Read_Temp')
        R_ref = 1800.0 # Ohm
        B = 3650.0 # K
        R_NTC = 10000.0 # Ohm

        sensor_supply = 1 # V
        resistance = volt * R_ref / (sensor_supply - volt)

        temp = B / (
            np.log(
                resistance /
                (R_NTC * np.exp(-B / (25 + 273.15)))
            )
        ) - 273.15

        return temp

    def read_laser_status(self):
        self.register_to_bus('Get Laser Current')

    def read_status(self):
        self.register_to_bus('Get Status')
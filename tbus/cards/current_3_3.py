import numpy as np
from collections import namedtuple
from .card import TBusCard, calibrated_getter, calibrated_setter, calibrate


def str_channel(channel):
    assert channel in [0, 1], 'invalid channel'
    return ['A', 'B'][channel]


class CurrentDriver3p3Card(TBusCard):
    def set_relay(self, channel, value):
        ch = str_channel(channel)
        self.set_int('Relay%s' % str_channel, 2 if value else 0)
        self.queue_register('Set Laser Status')

    def set_enabled(self, channel, value):
        ch = str_channel(channel)
        self.set_bool('Enable%s' % str_channel, value)
        self.queue_register('Set Laser Status')

    def set_lasers(self, current1, current2, power_down1, power_down2):
        raise Exception('not implemented')
        raise Exception('calibration missing')
        self.queue_register('Set Current Both')

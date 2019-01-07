import numpy as np
from scipy import constants
from ben.devices import RedPitaya


def voltage_to_temperature(voltage):
    A = 1.2874E-03
    B = 2.3573E-04
    C = 9.5052E-08

    resistance = voltage * 10000 / 3.3

    temperature = constants.K2C(1.0 / (
        A + B * np.log(resistance) + C * (np.log(resistance)**3)
    ))
    return temperature


class MilasStatus:
    def __init__(self, redpitaya_ip='rp-f04f8b.local'):
        if redpitaya_ip:
            self.rp = RedPitaya(redpitaya_ip)
        else:
            self.rp = RedPitaya()

    @property
    def power_internal(self):
        return float(self.rp.input[1].voltage)

    @property
    def temp1(self):
        return voltage_to_temperature(
            float(self.rp.input[3].voltage)
        )

    @property
    def temp2(self):
        return voltage_to_temperature(
            float(self.rp.input[2].voltage)
        )

    @property
    def power_main(self):
        return self.rp.fast_in[0].get_value()

    @property
    def power_aux(self):
        return self.rp.fast_in[1].get_value()
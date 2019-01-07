# -*- coding: utf-8 -*-
import visa
from time import sleep

class ILX:
    device = None
    batch_size = None

    def __init__(self, address=None):
        self.connect_to_device(address)

    def connect_to_device(self, address):
        rm = visa.ResourceManager()

        if not address:
            address = [a for a in rm.list_resources() if a.startswith('GPIB')][0]

        self.device = rm.open_resource(address)
		
		# increase timeout to prevent errors with large datasets
        self.device.timeout = 120000

        identity = self.read('*IDN?')
        assert 'ILX Lightwave' in identity, 'invalid device identity: %s' % identity

    def read(self, cmd):
        """Read the output of a command"""
        return self.device.query(cmd).rstrip('\n')

    def write(self, cmd):
        """Send one or multiple commands to the device"""
        if isinstance(cmd, (tuple, list)):
            self.device.write(';'.join(cmd))
        else:
            self.device.write(cmd)

    def set_laser_current(self, value):
        self.write('LAS:LDI %f' % value)

    def get_laser_current(self):
        return float(self.read('LAS:LDI?'))

    def set_current_enabled(self, value):
        self.write('LAS:OUT %d' % (1 if value else 0))

    def get_current_enabled(self):
        return self.read('LAS:OUT?')

    def get_current_limit_3724C(self):
        return float(self.read('LAS:LIM:I2?'))

    def get_laser_voltage(self):
        return float(self.read('LAS:LDV?'))

    def set_current_limit_3724C(self, value):
        self.write('LAS:LIM:I2 %f' % value)


if __name__ == "__main__":
    from ben.devices import run_device_service
    device = ILX('GPIB0::1::INSTR')
    run_device_service(device, 'ilx')

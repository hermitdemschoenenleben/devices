import visa
import numpy as np

AXIS_LENGTH = 401

class HP8951A:
    device = None

    def __init__(self, address=None):
        rm = visa.ResourceManager()
        
        if not address:
            address = [a for a in rm.list_resources() if a.startswith('GPIB')][0]
        
        self.device = rm.open_resource(address)
    
    @property
    def center_frequency(self):
        return float(self.device.query('CF?'))
    
    @center_frequency.setter
    def center_frequency(self, value):
        return self.device.write('CF %f' % value)

    @property
    def span(self):
        return float(self.device.query('SP?'))

    @span.setter
    def span(self, value):
        return self.device.write('SP %f' % value)

    @property
    def units(self):
        return self.device.query('AUNITS?')
    
    @property
    def x_axis(self):
        cf = self.center_frequency
        span = self.span
        return np.linspace(cf - span/2, cf + span/2, AXIS_LENGTH)

    def acquire(self):
        return [
            float(value) for value in
            self.device.query('TRA?').replace('\r\n', '').split(',')
        ]


if __name__ == "__main__":
    from ben.devices import run_device_service
    device = HP8951A()
    run_device_service(device, 'hp8951a')
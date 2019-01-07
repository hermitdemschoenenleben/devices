import numpy as np
from visa import ResourceManager

N_POINTS = 501

class RSSpectrumAnalyzer:
    device = None

    def __init__(self, ip):
        rm = ResourceManager()
        self.device = rm.open_resource('TCPIP::%s' % ip)
    
    def query(self, command):
        self.device.write(command)
        return self.device.read().rstrip('\n')
    
    @property
    def span(self):
        return float(self.query('FREQ:SPAN?'))
    
    @span.setter
    def span(self, value):
        self.device.write('FREQ:SPAN %f' % value)
    
    @property
    def center_frequency(self):
        return float(self.query('FREQ:CENTER?'))
    
    @center_frequency.setter
    def center_frequency(self, value):
        self.device.write('FREQ:CENTER %f' % value)
    
    def read_spectrum(self):
        self.device.write('FORMAT ASCII')
        return [float(v) for v in self.query('TRAC1? TRACE1').split(',')]
    
    @property
    def x_axis(self):
        half_span = self.span / 2
        return list(np.linspace(
            self.center_frequency - half_span,
            self.center_frequency + half_span,
            N_POINTS
        ))


if __name__ == "__main__":
    from ben.devices import run_device_service
    device = RSSpectrumAnalyzer('192.168.1.29')
    run_device_service(device, 'rs-fsl')

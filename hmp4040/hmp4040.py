from glob import glob
from serial import Serial

IDENTIFIER = 'HMP4040'


class HMP4040:
    def __init__(self, port=None):
        if port is None:
            for port in glob('/dev/ttyUSB*'):
                self.device = Serial(port)
                self.device.write(b'*IDN?\n')
                self.device.flush()

                if IDENTIFIER in self.read():
                    print('BREAK')
                    break

                self.device.close()
            else:
                raise Exception('device not found')
        else:
            self.device = Serial(port)

    def do(self, cmd):
        self.device.write((cmd + '\n').encode())

    def read(self):
        while True:
            data = self.device.read_all()
            if data:
                return data.decode().rstrip('\n')

    def beep(self):
        self.do('SYST:BEEP')

    def set_channel(self, channel):
        assert channel in [1, 2, 3, 4], 'invalid channel'
        self.do('INST OUT%d' % channel)

    @property
    def voltage(self):
        self.do('VOLT?')
        return float(self.read())

    @voltage.setter
    def voltage(self, value):
        self.do('VOLT %f' % value)

    @property
    def current(self):
        self.do('CURR?')
        return float(self.read())

    @current.setter
    def current(self, value):
        self.do('CURR %f' % value)

    @property
    def enabled(self):
        self.do('OUTP?')
        return self.read() == '1'

    @enabled.setter
    def enabled(self, value):
        self.do('OUTP %s' % ('ON' if value else 'OFF'))

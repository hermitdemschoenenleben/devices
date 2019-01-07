import visa

class Tektronix:
    def __init__(self):
        rm = visa.ResourceManager()
        self.dev = rm.open_resource('USB0::0x0699::0x0343::C023072::INSTR')
        self.write('*IDN?')
        assert self.read() == 'TEKTRONIX,AFG3102,C023072,SCPI:99.0 FV:3.1.1\n'
    
    def write(self, data):
        self.dev.write(data)
    
    def read(self):
        return self.dev.read()

    def get_duty_cycle(self, channel):
        self.write('SOUR%d:PULS:DCYC?' % (channel + 1))
        return float(self.read().replace('\n', ''))
    
    def set_duty_cycle(self, channel, percentage):
        return self.write(('SOUR%d:PULS:DCYC ' % (channel + 1)) + str(percentage))
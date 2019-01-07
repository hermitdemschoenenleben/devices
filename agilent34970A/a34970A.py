from visa import ResourceManager


class A34970A:
    def __init__(self, address):
        rm = ResourceManager()
        self.device = rm.open_resource(address)
        idn = self.query('*IDN?')
        assert '34970A' in idn, 'invalid device identity: %s' % idn

    def query(self, cmd):
        self.device.write(cmd)
        return self.device.read()

    def get_temperature(self, slot, channel):
        address = slot * 100 + channel
        return float(self.query(
            'MEAS:TEMP? THER,10000,(@%d)' % address
        ))


if __name__ == "__main__":
    from ben.devices import run_device_service
    device = A34970A('GPIB0::9::INSTR')
    run_device_service(device, 'agilent34970a')

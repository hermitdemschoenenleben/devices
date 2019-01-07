import socket
import rpyc
from rpyc.utils.server import ThreadedServer

PORTS = {
    'cnt90':            19000,
    'hp8591a':          19001,
    'ilx':              19002,
    'agilent34970a':    19003,
	'rs-fsl':           19004,
    'test':             19005,
    'control':          19006
}


def connect_to_device_service(ip, device_name):
    """
    Connects to a rpyc server that was started with `run_device_service` and
    thus provides access to a remotely running device.

    Usage:

        d = connect_to_device_service(host_ip_address, device_name)
        print(d.root.do_something())
    """
    assert device_name in PORTS, 'invalid device name, has to be one of %s' % ', '.join(PORTS.keys())
    port = PORTS[device_name]
    conn = rpyc.connect(ip, port)
    return conn


def run_device_service(device, device_name):
    """
    Runs a rpyc server that provides remote access to a device.

    Usage:

        class MyDevice:
            def do_something(self):
                return 42

        device = MyDevice()
        run_device_service(device, 'my_device_name')

    Notice that 'my_device_name' has to exist in `PORTS`.
    """
    assert device_name in PORTS, 'invalid device name, has to be one of %s' % ', '.join(PORTS.keys())
    port = PORTS[device_name]

    class ServiceDevice(rpyc.Service):
        def on_connect(self, name):
            pass

        def _rpyc_getattr(self, name):
            """Allow all properties to be retrieved"""
            return getattr(device, name)

        def _rpyc_setattr(self, name, value):
            """Allow all properties to be set"""
            return setattr(device, name, value)

    def get_ip_address():
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]

    print('Running rpyc server for device %s. Connect with connect_to_device_service(%s, "%s")' % (
        device_name, get_ip_address(), device_name
    ))

    t = ThreadedServer(ServiceDevice, port=port)
    t.start()

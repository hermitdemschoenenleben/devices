# current is measured by agilent 3458A via labview
# one data point is measured when redpitaya triggers

N_measurements = 1000

from ben.devices import RedPitaya
from ben.control.client import DeviceClient
from time import time, sleep
import json


device = DeviceClient('ecdl')
rp = RedPitaya('rp-f04f8b.local')

trigger_status = False
currents = []

for i in range(N_measurements + 100):
    print(i)
    currents.append(device.get_ta_current())
    print(currents[-1])
    rp.fast_out[0].set_constant_voltage(1 if trigger_status else -1)
    trigger_status = not trigger_status
    sleep(0.9)

f = open('stability_data.json', 'w')
json.dump({
    'tbus_measured_currents': currents,
    'seconds_between_measurements': 1
}, f)
f.close()
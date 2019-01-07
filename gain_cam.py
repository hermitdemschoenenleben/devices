import os
from time import time
import numpy as np
from matplotlib import pyplot as plt

class Camera:
    def __init__(self, idx):
        filename = 'camera%d' % idx
        try:
            cam = ic.get_device_by_file(filename)
            cam.exposure.value
        except:
            print('opening camera %d' % idx)
            cam = ic.get_device_by_dialog()
            cam.save_device_state(filename)
    
        cam.exposure.value = -13
        cam.prepare_live()
        self.cam = cam
    
    def snap_image(self, background=None):
        self.cam.start_live()
        self.cam.snap_image()
        [d, img_height, img_width, img_depth] = self.cam.get_image_data()
        img_depth = int(img_depth)
        img = np.ndarray(buffer = d, dtype = np.uint8, shape = (img_width, img_height, img_depth))
        self.cam.suspend_live()
    
        data = img[:,:,0].astype(np.int16)
        
        if background is not None:
            data -= background
        
        return data

    def save_image(self, filename):
        self.cam.save_image(filename, 0)

os.chdir('C:\\Users\\gain\\Documents\\The Imaging Source Europe GmbH\\TIS Grabber DLL\\bin\\win32')

import icpy3

ic = icpy3.IC_ImagingControl()
ic.init_library()

cams = [Camera(idx) for idx in range(3)]

for cam in cams:
    try:
        cam.suspend_live()
    except:
        pass
#cam.set_format(0)
#cam.snap_image()
    


"""
input('background')
backgrounds = [cam.snap_image() for cam in cams]
input('continue')
"""
# %%
"""
if not cam.callback_registered:
    cam.register_frame_ready_callback() # needed to wait for frame ready callback

cam.enable_continuous_mode(True)        # image in continuous mode
cam.enable_trigger(True)
#cam.wait_til_frame_ready(1000)
"""
# %% foo
times = []

time_before = time()

for i in range(1):
    for j, cam in enumerate(cams):
        img = cam.snap_image(background=backgrounds[j])
        print(np.sum(np.sum(img)) / 1e9 / (10 * (2 ** (cam.cam.exposure.value))))
                        
        
        """plt.pcolormesh(img)
        plt.colorbar()
        plt.show()"""

    now = time()
    times.append(now - time_before)
    time_before = now

    """plt.pcolormesh(img[:,:,1])
    plt.show()
    plt.pcolormesh(img[:,:,2])
    plt.show()"""
print(np.mean(times))
# %%
"""
import icpy3

# open lib
ic = icpy3.IC_ImagingControl()
ic.init_library()

# open first available camera device
cam_name = 'cam1'
cam = ic.get_device_by_file(cam_name)
#cam1 = ic.get_device_by_file('cam1')
"""
# change camera properties
"""print(cam.list_property_names()         )# ['gain', 'exposure', 'hue', etc...]
cam.gain.auto = True                    # enable auto gain
#print cam.exposure.range                # (0, 10)
emin = cam.exposure.min                 # 0
emax = cam.exposure.max                 # 10
cam.exposure.value = (emin + emax) / 2  # disables auto exposure and sets value to half of range
#print cam.exposure.value                # 5
""""""
cam.exposure.value = -10
# change camera settings
formats = cam.list_video_formats()
#cam.set_video_format(formats[0].decode())        # use first available video format

#%% 

cam.enable_continuous_mode(True)        # image in continuous mode
cam.start_live(show_display=True)       # start imaging

if not cam.callback_registered:
    cam.register_frame_ready_callback() # needed to wait for frame ready callback

for i in range(25):
    print(i)
    cam.save_image(cam_name + '.jpg', 1)
    from time import sleep
    sleep(1)"""
"""
#cam.enable_trigger(True)                # camera will wait for trigger
if not cam.callback_registered:
    cam.register_frame_ready_callback() # needed to wait for frame ready callback

for i in range(100):                        # take 10 shots
    cam.reset_frame_ready()                 # reset frame ready flag
    
    # send hardware trigger OR call cam.send_trigger() here
    #cam.send_trigger()
    
    #cam.wait_til_frame_ready(1000)              # wait for frame ready due to trigger
    cam.save_image(''.join(['image-',       # save image
                            str(i),
                            '.jpg']), 1)

cam.stop_live()
cam.close()

ic.close_library()"""
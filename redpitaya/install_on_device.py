#!/usr/bin/python2.7
"""
Installs pyrp as well as a modified version of redpitaya's monitor tool
(monitoradvanced). To be executed on the red pitaya itself.
"""
import os
import subprocess

def do(cmd):
    subprocess.call(cmd, shell=True)

# monitoradvanced
do('git clone https://github.com/RedPitaya/RedPitaya.git')
os.chdir('RedPitaya/Test/monitoradvanced')
do('make')
do('cp monitoradvanced /usr/bin')

# install pyrp3
do('git clone https://github.com/hermitdemschoenenleben/pyrp3.git')
os.chdir('pyrp3')
do('python3 setup.py install')
os.chdir('../')
do('pip3 install myhdl')

# install pyrp2
do('git clone https://github.com/hermitdemschoenenleben/pyrp.git')
os.chdir('pyrp')
do('python2.7 setup.py install')
do('apt-get install python-pip python-numpy')
do('wget https://bootstrap.pypa.io/ez_setup.py -O - | python')
do('pip2 install myhdl')

# build monitor shared library
os.chdir('monitor')
do('make')
do('cp libmonitor.so /usr/lib/')

# create directory
do('mkdir /jumps')

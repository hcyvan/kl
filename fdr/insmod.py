#!/usr/bin/python
'''
This is a script for this laboratory library.

Usage:
+ *sudo ./insmod.py -d* to do clean work
+ *sudo ./insmod.py 0 1 ...* to create devices, such as, navyfdr0, navyfdr1 ...
'''
import os
import sys
import re
import string
        
RM=False
module='fdr'
device='navyfdr'
mode='644'
minor=[0]

# mknod for a device with the minor device number.
def mknodDev(device, minor):
    # get the major device number
    fd=open(r'/proc/devices')
    devs=fd.read()
    fd.close()
    regexp=re.compile(r'(\d+)\s+%s'%device)
    major=regexp.search(devs).group(1)
    # make inode for the device
    os.system(r'mknod /dev/%s%d c %s %d'%(device, minor, major, minor))
    os.system(r'chown navy /dev/%s%d'%(device, minor))
    os.system(r'chmod %s /dev/%s%d'%(mode, device, minor)) 

OPRATOR=set(['d'])
args=[]
if len(sys.argv)>1:
    args=sys.argv[1:]
    minor=[]
for i in range(len(args)):
    if args[i]== "-d":
        RM=True
    else:
        minor.append(string.atoi(args[i]))

# rm the module
if not os.system('lsmod |grep %s > /dev/null'%module):
    os.system('rmmod %s'%module)
# rm the device
if not os.system('ls /dev | grep %s > /dev/null'%device):
    os.system("rm -f /dev/%s*"%device)

if RM==False:
    # make
    os.system('make')
    # insmod the moudle
    os.system("insmod %s.ko"%module)

    for m in minor:
        mknodDev(device, m)
else:
    os.system('make clean')

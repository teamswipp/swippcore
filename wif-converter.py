#!/usr/bin/python
#
# WIF converter from private ECDSA keys and vice versa, tailored for Swipp
# Some code and ideas taken from https://github.com/crcarlo/btcwif
#
# Copyright (c) 2017 Carlo Cervellin
# Copyright (c) 2017-2018 The Swipp developers
# Distributed under the MIT/X11 software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import errno
import hashlib
import sys

# base58 alphabet
alphabet = '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz'

def sha256(hex_string):
    byte_array = bytearray.fromhex(hex_string)

    m = hashlib.sha256()
    m.update(byte_array)
    return m.hexdigest()

def b58encode(hex_string):
    num = int(hex_string, 16)
    encode = ""
    base_count = len(alphabet)

    while (num > 0) :
        num, res = divmod(num,base_count)
        encode = alphabet[res] + encode

    return encode

def b58decode(v):
    if not isinstance(v, str):
        v = v.decode('ascii')

    decimal = 0

    for char in v:
        decimal = decimal * 58 + alphabet.index(char)

    return hex(decimal)[2:] # (remove "0x" prefix)

def privToWif(priv, testnet=False, verbose=False) :
    _priv = priv.lower()

    if verbose:
        print("Private key: "+_priv)

    priv_add_wprefix = ("ef" if testnet else "9b") + _priv

    if verbose:
        print("Private with prefix at beginning: " + priv_add_wprefix)

    first_sha256 = sha256(priv_add_wprefix)
    if verbose:
        print("SHA256: " + first_sha256.upper())

    seconf_sha256 = sha256(first_sha256)
    if verbose:
        print("SHA256: " + seconf_sha256.upper())

    first_4_bytes = seconf_sha256[0:8]
    if verbose:
       print("First 4 bytes: " + first_4_bytes)

    resulting_hex = priv_add_wprefix + first_4_bytes
    if verbose:
        print("Resulting WIF in HEX: " + resulting_hex)

    result_wif = b58encode(resulting_hex)
    if verbose:
        print("Resulting WIF: " + result_wif)

    return result_wif

def wifToPriv(wif, verbose=False) :
    if not wifChecksum(wif):
        raise Exception("The WIF is not correct (does not pass checksum)")

    if verbose:
        print("WIF: " + wif)

    byte_str = b58decode(wif)
    if verbose:
        print("WIF base58 decoded: " + byte_str)

    byte_str_drop_last_4bytes = byte_str[0:-8]
    if verbose:
        print("Decoded WIF drop last 4 bytes: " + byte_str_drop_last_4bytes)

    byte_str_drop_first_byte = byte_str_drop_last_4bytes[2:]
    if verbose:
        print("ECDSA private key: " + byte_str_drop_first_byte)

    return byte_str_drop_first_byte

def wifChecksum(wif, verbose=False) :
    if verbose:
        print("WIF: " + wif)

    byte_str = b58decode(wif)
    if verbose:
        print("WIF base58 decoded: " + byte_str)

    byte_str_drop_last_4bytes = byte_str[0:-8]
    if verbose:
        print("Decoded WIF drop last 4 bytes: " + byte_str_drop_last_4bytes)

    sha_256_1 = sha256(byte_str_drop_last_4bytes)
    if verbose:
        print("SHA256 1: " + sha_256_1)

    sha_256_2 = sha256(sha_256_1)
    if verbose:
        print("SHA256 2: " + sha_256_2)

    first_4_bytes = sha_256_2[0:8]
    if verbose:
        print("First 4 bytes: " + first_4_bytes)

    last_4_bytes_WIF = byte_str[-8:]
    if verbose:
        print("Last 4 bytes of WIF: " + last_4_bytes_WIF)

    bytes_check = False
    if first_4_bytes == last_4_bytes_WIF:
        bytes_check = True

    if verbose:
        print("4 bytes check: " + str(bytes_check))

    check_sum = False
    if bytes_check and (byte_str[0:2] == "ef" or byte_str[0:2] == "9b"):
        check_sum = True

    if verbose:
        print("Checksum: " + str(check_sum))

    return check_sum

if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.stderr.write("Please specify a keyfile to load\n")
        sys.exit(1)

    with open(sys.argv[1], 'r') as keyfile:
        contents = keyfile.read()

    start = contents.find("priv:")
    end = contents.find("pub:")

    if start == -1:
        sys.stderr.write("Invalid key file\n")
        sys.exit(errno.EINVAL)

    key = contents[len("priv:") + start:end]
    key = key.replace(":", "").replace("\n", "").replace(" ", "")
    privToWif(key, len(sys.argv) >= 3 and sys.argv[2] == "testnet", True)

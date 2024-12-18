#Copyright 2019-2025 Jared Hendrickson
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Tool functions for E2E testing."""
import datetime
import ipaddress
import random


def generate_random_future_date():
    """
    Generates a random future date in MM/DD/YYYY format. This date will always be the next year or greater, and does
    not generate days beyond 28 since not every month has more than 28 days.
    :return: date string in MM/DD/YYYY
    """
    # Local variables
    current_year = datetime.datetime.now().year
    random_day = random.randint(1, 28)
    random_month = random.randint(1, 12)
    random_year = random.randint(current_year + 1, current_year + 10)

    # Ensure single digit date values have leading zero
    random_day = f"0{random_day}" if random_day < 10 else str(random_day)
    random_month = f"0{random_month}" if random_month < 10 else str(random_month)

    # Return the date string
    return f"{random_month}/{random_day}/{random_year}"


def bits_to_hex(bits: int):
    """
    Converts a subnet's bit mask into a hex mask.
    :param bits: a network bitmask
    :return: the hex mask equivalent to the bitmask
    """
    # First convert to dot mask
    dot_mask = '.'.join([str((0xffffffff << (32 - int(bits)) >> i) & 0xff) for i in [24, 16, 8, 0]])

    # Loop through each octet and append it's hex value to our mask
    hex_mask = "0x"
    for octet in dot_mask.split("."):
        # Format octet into 2-digit hex value and add it to our hex mask
        hex_octet = format(int(octet), "02x")
        hex_mask = hex_mask + hex_octet

    return hex_mask


def parse_ifconfig(ifconfig_out: str):
    """Parses the ifconfig output for each individual interface into it's own dict
    :param ifconfig_out: the ifconfig output to check against
    :return: a dictionary containing a section for each individual interface in the ifconfig output
    """
    # Local variables
    ifconfig_dict = {}
    if_name = ""

    # Loop through each line of the ifconfig
    for line in ifconfig_out.split("\n"):
        # If the line does not start with a space, capture the interface name and start it's own dictionary
        if not line.startswith("\t"):
            if_name = line.split(":", maxsplit=1)[0]
            ifconfig_dict[if_name] = []
        # Otherwise, simply add the line to the dict for this if
        else:
            ifconfig_dict[if_name].append(line.replace("\t", ""))

    return ifconfig_dict


def is_if_in_ifconfig(ifconfig_out: str, iface: str, ipaddr: str, bitmask: int, vhid=None):
    """
    Checks if a specific interface configuration is present in ifconfig
    :param iface: the interface expected to host this configuration
    :param ifconfig_out: the ifconfig output to check against
    :param ipaddr: the IPv4 or IPv6 (depending on the inet_type) of the network
    :param bitmask: the bitmask of the network
    :param vhid: optionally check if the interface has a vhid, ignores otherwise
    :return: a string containing the ifconfig line expected from the network details
    """
    # Local variables
    ifconfig = parse_ifconfig(ifconfig_out)
    ipaddr = ipaddress.ip_address(f"{ipaddr}")
    subnet = ipaddress.ip_network(f"{ipaddr}/{bitmask}", strict=False)

    # Set the expected string for a IPv4 network
    if ipaddr.version == 4:
        ifconfig_line = f"inet {ipaddr} netmask {bits_to_hex(bitmask)} broadcast {subnet.broadcast_address}"
    # Set the expected string for a IPv6 network
    elif ipaddr.version == 6:
        ifconfig_line = f"inet6 {ipaddr.compressed} prefixlen {bitmask}"
    # Otherwise, our network details are invalid raise an error
    else:
        raise ValueError("Invalid IP address specified")

    # Check for the VHID if requested
    if vhid:
        ifconfig_line = f"{ifconfig_line} vhid {vhid}"

    # Check if this line is in ifconfig
    if ifconfig_line in ifconfig.get(iface, []):
        return True

    # Return false if not match was found
    return False

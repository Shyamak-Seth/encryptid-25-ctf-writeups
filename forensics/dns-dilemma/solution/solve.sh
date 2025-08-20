#!/bin/bash
tshark -r capture.pcapng -Y 'ip.addr == 8.8.8.8 and dns.flags.response == 0 and dns.qry.name contains "google.com"' -T fields -e dns.id

# the flag is hidden byte-by-byte in the ID fields of the DNS packets that point to valid domains
# this command will give you the hex-encoded IDs, one per line
# essentially, it filters for DNS query packets that point to 8.8.8.8 and contain "google.com" (valid domains)
# once you convert those hex bytes to ASCII, you'll get a base64 encoded string
# decode that to get the flag

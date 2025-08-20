from scapy.all import *
import random
import base64
import string

domains = ['google.com', 'accounts.google.com', 'maps.google.com', 'mail.google.com', 'one.google.com']
data = base64.b64encode(b"encryptid{f1nD1nG_st3g_in_DnS_p@cK3T5???}").decode()
encoded = [ord(c) for c in data]

for val in encoded:
    pkt = IP(dst="8.8.8.8")/UDP(dport=53)/DNS(id=val, rd=1, qd=DNSQR(qname=random.choice(domains)))
    send(pkt)
    for i in range(random.randint(1, 10)):
        random_id = random.randint(0, 65535)
        random_domain = ''.join(random.choices(string.ascii_lowercase, k=10)) + ".com"
        noise_pkt = IP(dst="8.8.8.8")/UDP(dport=53)/DNS(id=random_id, rd=1, qd=DNSQR(qname=random_domain))
        send(noise_pkt)

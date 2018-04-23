import sys
import dpkt
import struct
import socket
import itertools

l = ['0','256','512','1000']
i = 0
out = open('ap_ack.txt','w')

while i < len(l):
	ele1=l[i]
	filename = 'assignment-4-data/'+ele1+'/AccessPoint-1-0.pcap'
	f = open(filename)
	print filename
	pcap = dpkt.pcap.Reader(f)

	frame_rts = 0
	frame_rts_total = 0
	for ts,buf in pcap:
		buf_radiotap = dpkt.radiotap.Radiotap(buf)
		buf_radiotap_len = socket.ntohs(buf_radiotap.length)
		wlan = dpkt.ieee80211.IEEE80211(buf[buf_radiotap_len:])

		if wlan.subtype == dpkt.ieee80211.C_ACK:
			frame_rts_total  = frame_rts_total + len(buf)
			frame_rts = frame_rts + 1
	x = (frame_rts_total*8.00)/(1024*1024*50)
	ele2 = l[i]
	out.write(str(x)+' '+ele2+'\n')
	i += 1

out.close()

out = open('sta1_ack.txt','w')
i = 0

while i < len(l):
	ele1=l[i]
	filename = 'assignment-4-data/'+ele1+'/AccessPoint-1-0.pcap'
	f = open(filename)
	print filename
	pcap = dpkt.pcap.Reader(f)

	frame_rts = 0
	frame_rts_total = 0
	for ts,buf in pcap:
		buf_radiotap = dpkt.radiotap.Radiotap(buf)
		buf_radiotap_len = socket.ntohs(buf_radiotap.length)
		wlan = dpkt.ieee80211.IEEE80211(buf[buf_radiotap_len:])

		if wlan.subtype == dpkt.ieee80211.C_ACK:
			frame_rts_total  = frame_rts_total + len(buf)
			frame_rts = frame_rts + 1
	x = (frame_rts_total*8.00)/(1024*1024*50)
	ele2=l[i]
	out.write(str(x)+' '+ele2+'\n')
	i += 1
out.close()


out = open('sta2_ack.txt','w')
i = 0

while i < len(l):
	ele1=l[i]
	filename = 'assignment-4-data/'+ele1+'/AccessPoint-1-0.pcap'
	f = open(filename)
	print filename
	pcap = dpkt.pcap.Reader(f)

	frame_rts = 0
	frame_rts_total = 0
	for ts,buf in pcap:
		buf_radiotap = dpkt.radiotap.Radiotap(buf)
		buf_radiotap_len = socket.ntohs(buf_radiotap.length)
		wlan = dpkt.ieee80211.IEEE80211(buf[buf_radiotap_len:])

		if wlan.subtype == dpkt.ieee80211.C_ACK:
			frame_rts_total  = frame_rts_total + len(buf)
			frame_rts = frame_rts + 1
	x = (frame_rts_total*8.00)/(1024*1024*50)
	ele2=l[i]
	out.write(str(x)+' '+ele2+'\n')
	i += 1

out.close()
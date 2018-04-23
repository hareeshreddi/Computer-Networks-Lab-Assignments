import sys
import dpkt
import struct
import socket


l = ['0','256','512','1000']
i=0
out=open('tcp_ack_ap.txt','w')
out1=open('tcp_seg_ap.txt','w')

while i < len(l):
	ele1=l[i]
	filename = 'assignment-4-data/'+ele1+'/AccessPoint-1-0.pcap'
	f = open(filename)
	print f
	pcap=dpkt.pcap.Reader(f)
	frame_tcp_ack=0
	frame_tcp_seg=0
	frame_tcp_Ack_total=0
	frame_tcp_seg_total=0

	if 1 :
		for ts,data in pcap:
			buf_radiotap=dpkt.radiotap.Radiotap(data)
			buf_radiotap_len=socket.ntohs(buf_radiotap.length)
			wlan=dpkt.ieee80211.IEEE80211(data[buf_radiotap_len:])

			try:
				tcp=dpkt.tcp.TCP(wlan.data)
				if(tcp.flags & dpkt.tcp.TH_ACK) != 0:
					frame_tcp_ack += 1
					frame_tcp_Ack_total += len(data)
				if len(tcp.data) >0 :
					frame_tcp_seg += 1
					frame_tcp_seg_total += len(data)
			except dpkt.Error as e:
				zz =1


	x1 = (frame_tcp_Ack_total*8.00)/(1024*1024*50)
	x2 = (frame_tcp_seg_total*8.00)/(1024*1024*50)
	out.write(ele1+' '+str(x1)+'\n')
	out1.write(ele1+' '+str(x2)+'\n')
	i += 1

out.close()
out1.close()

out=open('tcp_ack_sta1.txt','w')
out1=open('tcp_seg_sta1.txt','w')
i = 0
while i < len(l):
	ele2=l[i]
	filename = 'assignment-4-data/'+ele2+'/Station-0-0.pcap'
	f = open(filename)
	print f
	pcap=dpkt.pcap.Reader(f)
	frame_tcp_ack=0
	frame_tcp_seg=0
	frame_tcp_Ack_total=0
	frame_tcp_seg_total=0

	if 1 :
		for ts,data in pcap:
			buf_radiotap=dpkt.radiotap.Radiotap(data)
			buf_radiotap_len=socket.ntohs(buf_radiotap.length)
			wlan=dpkt.ieee80211.IEEE80211(data[buf_radiotap_len:])

			try:
				tcp=dpkt.tcp.TCP(wlan.data)
				if(tcp.flags & dpkt.tcp.TH_ACK) != 0:
					frame_tcp_ack += 1
					frame_tcp_Ack_total += len(data)
				if len(tcp.data) >0 :
					frame_tcp_seg += 1
					frame_tcp_seg_total += len(data)
			except dpkt.Error as e:
				zz =1


	x1 = (frame_tcp_Ack_total*8.00)/(1024*1024*50)
	x2 = (frame_tcp_seg_total*8.00)/(1024*1024*50)
	out.write(ele2+' '+str(x1)+'\n')
	out1.write(ele2+' '+str(x2)+'\n')
	i += 1

out.close()
out1.close()

out=open('tcp_ack_sta2.txt','w')
out1=open('tcp_seg_sta2.txt','w')

i = 0
while i < len(l):
	ele3=l[i]
	filename = 'assignment-4-data/'+ele3+'/Station-2-0.pcap'
	f = open(filename)
	print f
	pcap=dpkt.pcap.Reader(f)
	frame_tcp_ack=0
	frame_tcp_seg=0
	frame_tcp_Ack_total=0
	frame_tcp_seg_total=0

	if 1 :
		for ts,data in pcap:
			buf_radiotap=dpkt.radiotap.Radiotap(data)
			buf_radiotap_len=socket.ntohs(buf_radiotap.length)
			wlan=dpkt.ieee80211.IEEE80211(data[buf_radiotap_len:])

			try:
				tcp=dpkt.tcp.TCP(wlan.data)
				if(tcp.flags & dpkt.tcp.TH_ACK) != 0:
					#print("a")
					frame_tcp_ack += 1
					frame_tcp_Ack_total += len(data)
				if len(tcp.data) >0 :
					frame_tcp_seg += 1
					frame_tcp_seg_total += len(data)
			except dpkt.Error as e:
				zz =1


	x1 = (frame_tcp_Ack_total*8.00)/(1024*1024*50)
	x2 = (frame_tcp_seg_total*8.00)/(1024*1024*50)
	out.write(ele+' '+str(x1)+'\n')
	out1.write(ele+' '+str(x2)+'\n')
	i += 1

out.close()
out1.close()
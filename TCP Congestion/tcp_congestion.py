'''
    Name: Jeffin Biju
    Roll: EE19B085
    To run: python3 tcp_congestion.py --i 1.0 --m 1.0 --n 0.5 --f 0.1 --s 0.99 --T 10000
'''


#import packages
import argparse                                                                 
import sys        
import numpy as np
import matplotlib.pyplot as plt
import math


# parse command line arguements
def parse_opts(argv):         
    parser = argparse.ArgumentParser(description='Interactive tool')                                         
    parser.add_argument('-ki', '--i',                                                                         
                        default=1.0)                                            
    parser.add_argument('-km', '--m',                                                                        
                        default=1.0)                                            
    parser.add_argument('-kn', '--n',                                                            
                        default=1.0)  
    parser.add_argument('-kf', '--f',                                               
                        default=0.5)      
    parser.add_argument('-ps', '--s',                                                               
                        default=0.99)       
    parser.add_argument('-total', '--T',                                                
                        default=10000)                                      
    opts = parser.parse_args(argv[1:])                                          
    return opts     

# set value to params
opts = parse_opts(sys.argv) 
ki = float(opts.i)
km = float(opts.m)
kn = float(opts.n)
kf = float(opts.f)
ps = float(opts.s)
total = int(opts.T)

file1 = open("CWvsUpdates.txt", "w") 
file2 = open("CWvsRTT.txt", "w") 
#max segment size
mss = 1000
#receiver window size
rws = 1000*mss
#stores congestion window size after each update
cws1 = []
#stores congestion window size after each round/RTT
cws2 = []
#congestion window size
cw = ki*mss
file1.write('Update: {} CW: {}\n'.format(0,cw))
file2.write('Round: {} CW: {}\n'.format(0,cw))
cws1.append(cw)
cws2.append(cw)

#number of segments sent
cur = 0

threshold = rws/2

np.random.seed(40)

while cur < total:
    num = min(math.ceil(cw/mss), total-cur)
    for i in range(num):
        x = np.random.uniform(0, 1)
        # ack received
        if x < ps:
            if cw < threshold:
                cw = min(cw + km*mss, rws)
            else:
                cw = min(cw + kn*mss*mss/cw, rws)
            cws1.append(cw)
            file1.write('Update: {} CW: {}\n'.format(len(cws1)-1,cw))
        # ack not received   
        else:
            threshold = cw/2
            cw = max(1*mss, kf*cw)
            cws1.append(cw)
            file1.write('Update: {} CW: {}\n'.format(len(cws1)-1,cw))
    cws2.append(cw)  
    file2.write('Round: {} CW: {}\n'.format(len(cws2)-1,cw))
    cur += num

file1.close()
file2.close()

# plot CW vs update number
plt.figure(figsize=(8, 8))
plt.plot(np.arange(len(cws1)), np.array(cws1)/mss)
plt.xlabel('Update number')
plt.ylabel('Congestion Window size (in MSS scale)')
plt.title('ki={}, km={}, kn={}, kf={}, ps={}, T={}'.format(ki, km, kn, kf, ps, total))
plt.savefig('ki={}km={}kn={}kf={}ps={}T={}1.pdf'.format(ki, km, kn, kf, ps, total))

# plot CW vs Number of RTTs
plt.figure(figsize=(8, 8))
plt.plot(np.arange(len(cws2)), np.array(cws2)/mss)
plt.xlabel('Round number or No. of RTTs')
plt.ylabel('Congestion Window size (in MSS scale)')
plt.title('ki={}, km={}, kn={}, kf={}, ps={}, T={}'.format(ki, km, kn, kf, ps, total))
plt.savefig('ki={}km={}kn={}kf={}ps={}T={}2.pdf'.format(ki, km, kn, kf, ps, total))

#write  CW after each update to a file




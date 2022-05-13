'''
    Q1 code 
    JEFFIN BIJU 
    EE19B085
'''
import matplotlib.pyplot as plt
import requests
import subprocess
from math import radians, cos, sin, asin, sqrt

#Part a)
def haversine(lon1, lat1, lon2, lat2):
    """
    Calculate the great circle distance in kilometers between two points on the earth (specified in decimal degrees)
    """
    # convert decimal degrees to radians
    lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])
    # haversine formula
    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2 
    c = 2 * asin(sqrt(a))
    r = 6371 # Radius of earth in kilometers.
    return c * r

lat1 = 12.99
lon1 = 80.23
file1 = open('ping-servers.txt', 'r')
file2= open('distances.txt', 'w')
lines = file1.readlines()
servers = []
lats = []
lons = []
distances = []
with open('distances.txt', 'w') as file2:
    for line in lines:
        servers.append(line.strip())
        print('http://ip-api.com/json/'+line.strip())
        response = requests.get('http://ip-api.com/json/'+line.strip()).json()
        lat2 = response['lat']
        lon2 = response['lon']
        distance = haversine(lon1, lat1, lon2, lat2)
        lats.append(lat2)
        lons.append(lon2)
        distances.append(distance)
        file2.writelines(str(distance)+'\n')

#Part b)
import csv  
columns = ['MY_LOC(lat, lon)', 'SERVER', 'SERVER_LOC', 'RTT1', 'RTT2', 'RTT3', 'RTT4', 'RTT5', 'RTT6', 'RTT7', 'RTT8', 'RTT9', 'RTT10'] 
lat1 = 12.99
lon1 = 80.23
rows = []
with open('rtt_log.csv', 'w') as file2:
    csvwriter = csv.writer(file2)
    csvwriter.writerow(columns)
    for i in range(len(servers)):
        row = [(lat1, lon1), servers[i], (lats[i], lons[i])]
        process = subprocess.Popen(['ping', '-c 25', servers[i]], 
                            stdout=subprocess.PIPE,
                            universal_newlines=True)
        while True:
            output = process.stdout.readline()
            if len(output.split(' ')) > 6 and output.split(' ')[6][0] is 't':
                rtt = output.split(' ')[6][5:]
                row.append(rtt)
                if len(row) is 13:
                    break
        print('done')
        rows.append(row)
        csvwriter.writerow(row)


#Part c)
import matplotlib.pyplot as plt
x = []
y = []
for i in range(len(servers)):
    for j in range(10):
        x.append(distances[i])
        y.append(float(rows[i][3+j]))
plt.scatter(x, y)
plt.xlabel('Distance(km)')
plt.ylabel('RTT(ms)')
plt.title('Scatter plot of Distance (km) vs RTT(ms)')
plt.savefig('Scatter_plot.png')

speeds = []
for i in range(len(x)):
    speeds.append((x[i]/y[i])*1e6)
avg_speed = sum(speeds)/len(speeds)
print('Average speed = {} m/s'.format(avg_speed))
c = 3e8
ratio = c/avg_speed
print('Average speed is {0:.2f} times slower than speed of light'.format(ratio))
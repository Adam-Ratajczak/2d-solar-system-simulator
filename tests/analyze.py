import math
import matplotlib.pyplot as plt
import numpy as np
import re

def load_file(filename, planet):
    file1 = open(filename, "r")

    positionsx = {}
    positionsy = {}
    velocities = {}

    while True:
        line = file1.readline()
        if not line:
            break
        data = re.findall(r"(\d+);(Sun[12]);\(([^,]+), ([^,]+), [^,]+, 1\);\(([^,]+), ([^,]+), [^,]+, 1\)", line)
        # print(line, "TEST", data)
        if len(data) == 0:
            continue
        # print(data[0])
        entry = data[0]
        time = float(entry[0])
        name = entry[1]
        posx = float(entry[2])
        posy = float(entry[3])
        velx = float(entry[4])
        vely = float(entry[5])
        pos = math.hypot(posx, posy)
        vel = math.hypot(velx, vely)

        if name != planet:
            continue
        positionsx[time] = posx / 1000000
        positionsy[time] = posy / 1000000
        velocities[time] = vel
    
    #if (filename == "data-10.txt"):
        #plt.plot(positionsx.keys(), positionsx.values(), label = planet + " " + filename + " posx")
        #plt.plot(positionsy.keys(), positionsy.values(), label = planet + " " + filename + " posy")
        #plt.plot(velocities.keys(), velocities.values(), label = planet + " " + filename + " vel")
    return velocities

def load_data(idx, planet):
    v1 = load_file(f"data-10{idx}.txt", planet)
    #v2 = load_file(f"data-600{idx}.txt", planet)
    v3 = load_file(f"data-15000{idx}.txt", planet)
    #v4 = load_file("data-60000.txt")

    def add_diff(v1, v2):
        vdiff = {}
        for t, v in v2.items():
            if not t in v1:
                continue
            vdiff[t] = (v - v1[t]) * 1000

        plt.figure(figsize = (12, 6))
        plt.plot(v1.keys(), v1.values(), label = f"vel")
        plt.plot(v2.keys(), v2.values(), label = f"vel")
        plt.plot(vdiff.keys(), vdiff.values(), label = f"diff")

    #add_diff(v1, v4)
    add_diff(v1, v3)
    #add_diff(v2, v3)
    #add_diff(v3, v4)

#load_data("", "Sun1")
load_data("-small", "Sun1")

plt.legend()
plt.show()

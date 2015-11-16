#!/usr/bin/python
import pylab as plt
from sys import argv

if (len(argv) > 1):
    names = []
    values = []

    filename = argv[1] + ".data"
    with open(filename) as file:
        for line in file:
            name, value = line.split()
            names.append(name)
            values.append(int(value))
    ind = range(len(values))
    plt.bar(ind, values)
    plt.xticks(map(lambda x: x + 0.5, ind), names)
    #plt.show()
    plotname = argv[1] + ".png"
    plt.savefig(plotname)
else:
    print argv[0], "<event name>"


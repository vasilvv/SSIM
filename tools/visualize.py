#!/usr/bin/python

import matplotlib.pyplot as plt
import sys

if len(sys.argv) < 2:
    print "Usage: vizualize.py file1[:label1] file2[:label2] ..."

colors = ['g', 'b', 'r', '#F800F0', '#00E8CC', '#E8E800']
markers = { 'I' : '*', 'P' : 's', 'B' : 'o' }

if len(sys.argv) - 1 > len(colors):
    print "Too many files specified"

def parse_arg(s):
    if ':' in s:
        return tuple(s.split(s, 1))
    else:
        return (s, s)

sources = map(parse_arg, sys.argv[1:])

for (filename, label), color in zip(sources, colors):
    f = open(filename, 'r')
    x, y  = [], []
    per_type = { 'I' : [], 'P' : [], 'B' : [] }
    for line in f:
        num, ssim, frametype, _ = line.strip().split(' ', 3)
        num = int(num)
        ssim = float(ssim)
        x.append(num)
        y.append(ssim)
        per_type[frametype].append(num)
    plt.plot(x, y, '-', color = color)
    for frametype, marker in markers.items():
        mx = per_type[frametype]
        plt.plot(mx, [y[x] for x in mx], marker, color = color)

plt.show()

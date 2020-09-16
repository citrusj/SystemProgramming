#!/usr/bin/env python

import sys

def read_mapper_output(file):
        for line in file:
                yield line.rstrip().split()

def main():
        data = {}
        inputdata = read_mapper_output(sys.stdin)
        for L in inputdata:
                group, number = L
                number = float(number)
                if group in data.keys():
                        data[group] = max(data[group],number)
                else:
                        data[group]=number
        for group, number in data.items():
                print("{},{}".format(group, number))
if __name__ == "__main__":
        main()

#!/usr/bin/env python

import sys

def read_input(file):
        for line in file:
                yield line.strip().split(',')

def main():
        data = read_input(sys.stdin)
        for i in data:
                print(i[0],i[1])

if __name__ == "__main__":
        main()

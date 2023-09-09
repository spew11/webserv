#!/usr/bin/python3
print("Content-Type: text/html;charset=UTF-8")
print()

import sys
import os


print("<h1>env</h1>")
env = os.environ.items()
line = []
for key, value in env:
    line.append("({}, {})".format(key, value))
print("<br>".join(line))


print("<h1>body</h1>")
print(sys.stdin.read())
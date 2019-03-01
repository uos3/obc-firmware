#!/usr/bin/env python3
import math

crystalosc_frequency = 38400000

wanted_deviation = 10000

wanted_symbolrate = 1200

e = 0
m = 0
d = math.floor((wanted_deviation * (1<<24))/crystalosc_frequency)
d1 = d >> 8
   
if d1 < 2:
  e = 0
  m = (wanted_deviation * (1<<23))/crystalosc_frequency
else:
  e = 0
  while d1 > 0:
    d1 = d1 >> 1
    e = e + 1
  e = e - 1
  m = (math.floor((wanted_deviation * (1<<24))/crystalosc_frequency) >> e) - 256
if (e > 7):
  print("Deviation Exponent error.")
  exit()

print("Deviation Exponent: %d (0x%02x)" % (e, e))
print("Deviation Modulus:  %d (0x%02x)" % (m,m))

if e == 0:
  print("Actual deviation: %d" % ((crystalosc_frequency * m ) >> 23))
else:
  print("Actual deviation: %d" % ((crystalosc_frequency * (256+m) ) >> (24-e)))

s = 0
s1 = 0
ms = 0
es = 0
   
s = math.floor((wanted_symbolrate * (1<<39))/crystalosc_frequency)
s1 = s >> 20
   
if s1 < 2:
  es = 0
  ms = math.floor((wanted_symbolrate * (1<<38))/crystalosc_frequency);
else:
  es = 0
  while s1 > 0:
    s1 = s1 >> 1
    es = es + 1
  es = es - 1
  ms = (math.floor((wanted_symbolrate * (1<<39))/(crystalosc_frequency)) >> es) - (1<<20); 

if (es > 15):
  print("Symbolrate Exponent error.")
  exit()

print("Symbolrate Exponent: %d (0x%02x)" % (es,es))
print("Symbolrate Modulus:  %d (0x%04x)" % (ms,ms))
   
if es == 0:
  print("Actual symbolrate: %d" % ((crystalosc_frequency * ms ) >> 38))
else:
  print("Actual symbolrate: %d" % ((crystalosc_frequency * ((1<<20)+ms) ) >> (39-es)))
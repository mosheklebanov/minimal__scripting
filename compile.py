import os
from time import sleep

gccpath = input("GCC Path: (press enter for default)")
if len(gccpath)<3: gccpath = "gcc"

if os.name=="posix":
  print("-----------------------------------------------")
  os.system("cd source && "+gccpath+" -Wall -c main.c compiler.c")
  print("Please wait...")
  sleep(1)
  os.system(gccpath+" source/main.o source/compiler.o -o bin/main")
  print("--------------Finished-------------------------")
elif os.name=="nt":
  print("-----------------------------------------------")
  os.system("cd source && "+gccpath+" -Wall -c main.c compiler.c")
  print("Please wait...")
  sleep(1)
  os.system(gccpath+" source/main.o source/compiler.o -o bin/main.exe")
  print("--------------Finished-------------------------")
else:
  print("Operating system '"+os.name+"' not supported.")
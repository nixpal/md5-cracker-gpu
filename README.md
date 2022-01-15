# Md5 GPU/CPU Cracker

### About:
This is a simple c tool to crack md5 hash using a wordlist.


### Usage: 
./cracker pwdlist.txt 1200cf8ad328a60559cf5e7c5f46ee6d
### How to compile:
gcc md5cracker.c md5.c -o mmd5cracker -framework opencl

#### Note: choose any password list you want. I tested this on a small file that I parsed from rockyou.txt.


### Result:

<img width="998" alt="dBi" src="https://github.com/nixpal/md5-cracker-gpu/blob/main/2.png">

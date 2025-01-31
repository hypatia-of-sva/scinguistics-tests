import string
with open('temp.txt') as fp:
    for line in fp:
        if(all(c in string.hexdigits or c == '\n' for c in line)):
            num = int("0x"+line,16)
            if(num > 2**15):
                num = num - 2**16
            print(num)

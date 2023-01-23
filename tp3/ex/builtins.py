import sys
import os.path 

for builtin in sys.argv[1:]:
    with open(builtin, "r") as builtin_file:
        array_name = os.path.basename(builtin).replace(".","_")
        print('const char %s[] = {' % array_name)
        i = 0
        for b in builtin_file.read():
            new_line = '\n' if ((i % 16) == 15) else ''
            print('0x%02X,' % ord(b), end=new_line)
            i+=1
        new_line = '\n' if ((i % 16) == 15) else ''
        print('0x%02X' % 0, end=new_line)
        print('};')
        print('')

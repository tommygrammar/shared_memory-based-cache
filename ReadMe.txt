#utilizing shared memory in optimizing read queries by creating an efficient memory cache

the read reads from the database and constantly updates the shared memory database

the shared memory is then read by the python

to set up, make sure you have mongoDB c library

compile read.c:
gcc -o read read.c -Ofast $(pkg-config --cflags --libs libmongoc-1.0) 
to run: ./read

compile write.c
gcc -o write writec -Ofast $(pkg-config --cflags --libs libmongoc-1.0)
to run: ./write

compile the shared_memory shared library
gcc -shared -o sm.so -Ofast -fPIC sm.c $(pkg-config --cflags --libs libmongoc-1.0 libbson-1.0)
to use: python3 read.py

to view current shared memory limits:
ipcs -lm

to adjust:
sudo sysctl -w kernel.shmmax=1073741824
This example sets SHMMAX to 1 GB . set it to the size of the data

to make change permanent add: kernel.shmmax=1073741824 to /etc/sysctl.conf: and then to apply changes: sudo sysctl -p


still undergoing improvements: 
pending updates: granular updates of writes, data compression, optimized parsing

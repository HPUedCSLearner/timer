rm -rf ./a.out

export LD_LIBRARY_PATH=probelib/sharedlib

gcc test.c -g -finstrument-functions -Lprobelib -lprobe -Lprobelib/sharedlib -lprobeImpl -Lprobelib/sharedlib -lhook   -ldl -rdynamic

./a.out
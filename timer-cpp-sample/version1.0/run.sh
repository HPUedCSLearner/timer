rm -rf ./*.o ./*.out ./main
g++ -c hook.cpp && ar -r libhook.a hook.o
#g++ main.cpp hook.o  -g -finstrument-functions -ldl -rdynamic -no-pie
# gcc main.c -o main  -g -finstrument-functions -lhook -lstdc++ -L.  -ldl -rdynamic -no-pie
# g++ main.cpp -isystem /usr/include/c++/ -o main  -g -finstrument-functions    -lhook -lstdc++ -L.  -ldl -rdynamic -no-pie
g++ main.cpp -finstrument-functions-exclude-file-list=/usr/include/c++  -o main  -g -finstrument-functions    -lhook -lstdc++ -L.  -ldl -rdynamic -no-pie
# g++ main.cpp -o main  -g -finstrument-functions    -lhook -lstdc++ -L.  -ldl -rdynamic -no-pie
# g++ main.cpp -o main      -lhook -lstdc++ -L.  -ldl -rdynamic -no-pie


./main

rm -rf ./*.o ./*.out 

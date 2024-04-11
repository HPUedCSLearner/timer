// main.cpp
#include <cstdio>
// #include "instrumentation_header.h"

#include<stack>


void BB() {
}

void AA() {
    BB();
}

int main() {
    std::stack<int> st;
    st.push(1);



    AA();
    BB();
    return 0;
}

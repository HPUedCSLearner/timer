// main.cpp
#include <cstdio>
// #include "instrumentation_header.h"

#include<stack>
#include<vector>

using namespace std;

void BB() {
}

void AA() {
    BB();
}

int main() {
    std::stack<int> st;
    vector<int> vec;
    st.push(1);
    vec.push_back(1);



    AA();
    BB();
    return 0;
}

#include <cassert>
#include "war.hpp"

int main() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    assert(input);
    assert(output);

    output << 0;

    input.close();
    output.close();
    return 0;
}
#include <cassert>
#include "war.hpp"

int main() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    assert(input);
    assert(output);

    vector<int> who_what;
    output << min_time(input, back_inserter(who_what)) << endl;
    copy(begin(who_what), end(who_what), ostream_iterator<int>(output, "\n"));

    input.close();
    output.close();
    return 0;
}
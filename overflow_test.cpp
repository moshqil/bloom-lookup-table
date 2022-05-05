#include <iostream>
using namespace std;

// overflow of (x + y) doesn't influence that 
// (x + y) - y = x
// proof (same thing with other integer types works):
int main() {
    int64_t a = 9000000000000000000;
    int64_t b = 8000000000000000001;
    int64_t c = a + b; 
    cout << c << endl; 
    // -1446744073709551615 obviosly overflow
    c -= b;
    cout << c << endl;
    // 9000000000000000000 correct!
}

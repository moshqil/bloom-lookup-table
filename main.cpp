#include "stress_tests.cpp"


int main() {    

    time_t time_now = time(nullptr) % 1000; // for better random

    cout << "Number of TESTS for list entries method stress test:\n";
    int tests1; cin >> tests1;
    cout << "Number of PAIRS for list entries method stress test:\n ";
    int pairs1; cin >> pairs1;
    cout << "List entries (" << tests1 << " tests with " << pairs1 << " pairs):" << endl;
    vector<double> denoms = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};

    int i = 0;
    for (double denom : denoms) {
        cout << "Percentege of successes for ratio "
            << denom << " is " 
            << stress_test_list_entries(tests1, pairs1, denom * pairs1, 3, 5, i++)
            << endl;
    }
}

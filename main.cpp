#include <BLT.cpp>
#include <stress_tests.cpp>


int main() {    

    time_t time_now = time(nullptr) % 1000; // for better random

    cout << "Number of TESTS for list entries method stress test:\n";
    int tests1; cin >> tests1;
    cout << "Number of PAIRS for list entries method stress test:\n ";
    int pairs1; cin >> pairs1;
    cout << "List entries (" << tests1 << " tests with " << pairs1 << " pairs):" << endl;
    vector<double> denoms = {2.6};

    for (double denom : denoms) {
        int count_ok = 0;
        for (int64_t i = 0; i < tests1; i++) {
            auto B1 = BloomLookupTable(pairs1, denom, 5);
            count_ok += B1.stress_test_list_entries_subtraction(pairs1, time_now * (i + 1));
        }
        double dcount_ok = 100 * (double)count_ok / (double)tests1;
        cout << "Percentege of successes for ratio " << denom << " is " << dcount_ok << endl;
    }
}

#include "stress_tests.cpp"


void test1() {
    time_t time_now = time(nullptr) % 1000; // for better random
    cout << "STRESS TESTING OF LIST-ENTRIES OF BLOOM LOOKUP TABLE" << endl;
    cout << "----------------------------------------------------" << endl;

    cout << "Number of TESTS:\n";
    int tests; cin >> tests;
    cout << "Number of PAIRS per filter:\n";
    int pairs; cin >> pairs;
    cout << "List entries (" << tests << " tests with " << pairs << " pairs):" << endl;
    vector<double> denoms = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};

    int i = 0;
    for (double denom : denoms) {
        cout << "Percentege of successes for ratio "
            << denom << " is " 
            << stress_test_list_entries(tests, pairs, denom * pairs, 3, 5, i++)
            << endl;
    }
}

void test2() {
    time_t time_now = time(nullptr) % 1000; // for better random
    cout << "STRESS TESTING OF LIST-ENTRIES OF SET DIFFERENCE" << endl;
    cout << "------------------------------------------------" << endl;

    cout << "Number of TESTS:\n";
    int tests; cin >> tests;
    cout << "Number of PAIRS per filter:\n";
    int pairs; cin >> pairs;
    cout << "Number of CELLS per filter:\n";
    int cells; cin >> cells;
    cout << "Number of INTERSECTING PAIRS for two sets:\n";
    int intersection_pairs; cin >> intersection_pairs;
    cout << "List entries (" << tests << " tests with " << pairs << " pairs):" << endl;
    vector<double> denoms = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};

    cout << "Percentege of successes is " 
        << stress_test_list_entries_subtracted(tests, pairs, cells, 3, 5,
                intersection_pairs, 1)
        << endl;
}


int main() {
    test1();
    test2();
}

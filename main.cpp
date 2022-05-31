#include "stress_tests.cpp"
#include <fstream>
#include <string>
#include <algorithm>


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
        << stress_test_list_entries_subtracted_empty_intersection(tests, pairs, cells, 3, 5,
                intersection_pairs, 1)
        << endl;
}

void test3() {
    vector<int> ns = {100, 1000, 10000, 100000};
    vector<double> denoms = {1.22, 1.23, 1.3, 1.4};

    int seed = 1;

    for (int k = 3; k <= 3; k++) {
        for (int n : ns) {
            for (auto denom : denoms) {
                int num_tests = 1000;
                if (n > 10000) {
                    num_tests = 100;
                }
                cout << k << " " << n << " " << denom << " " << stress_test_list_entries(num_tests, n,
                            denom * n, k, 10, seed++) << endl;
                // кол-во хеш функций - пар - отношение размера блум фильтра к парам - процент успехов
            }
        }
    }
}

void test4() {
    vector<int> ns_alice = {1000, 10000, 100000};
    vector<double> denoms_sum = {1.22, 1.23, 1.3, 1.5, 2};
    vector<int> ns_bob = {1000, 10000, 100000};

    int num_tests = 100; int i = 0;
    for (int k = 3; k <= 3; k++) {
        for (auto n_alice : ns_alice) {
            for (auto n_bob : ns_bob) {
                for (auto denom_sum : denoms_sum) {
                    if (n_bob != n_alice) {
                        continue;
                    }
                    cout << k << " " << n_alice << " " << n_bob << " " << denom_sum << " " <<
                    stress_test_list_entries_subtracted_empty_intersection(num_tests, n_alice, n_bob,
                                                                           (n_alice + n_bob) * denom_sum,
                                                                           k, 10, i++) << endl;
                }
            }
        }
    }
}

void test5() {
    int same = 1000;
    vector<int> ns_alice = {same};
    vector<double> denoms_sum = {1.22, 1.23, 1.3, 1.4, 1.5};
    vector<int> ns_bob = {same};

    int num_tests = 200; int i = 0;
    for (int k = 3; k <= 3; k++) {
        for (auto n_alice : ns_alice) {
            for (auto n_bob : ns_bob) {
                for (auto denom_sum : denoms_sum) {
                    if (n_bob > n_alice) {
                        continue;
                    }

                    cout << k << " " << n_alice << " " << n_bob << " " << denom_sum << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, 0,
                                                 (n_alice + n_bob) * denom_sum,
                                                 k, 10, i++) << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, same / 4,
                                                                                (6 * same / 4 + same / 4) * denom_sum,
                                                                                k, 10, i++) << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, same / 2,
                                                 (same + same / 2) * denom_sum,
                                                 k, 10, i++) << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, 3 * same / 4,
                                                 (same + same / 4) * denom_sum,
                                                 k, 10, i++) << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, same,
                                                 (same) * denom_sum,
                                                 k, 10, i++) << endl;
                }
            }
        }
    }
}

void test6() {
    int same = 100;
    vector<int> ns_alice = {same};
    vector<double> denoms_sum = {1.23, 1.3, 1.4, 2, 3, 3.5};
    vector<int> ns_bob = {same};

    int num_tests = 1000; int i = 0;
    for (int k = 3; k <= 7; k++) {
        for (auto n_alice : ns_alice) {
            for (auto n_bob : ns_bob) {
                for (auto denom_sum : denoms_sum) {
                    if (n_bob > n_alice) {
                        continue;
                    }
                    cout << k << " " << n_alice << " " << n_bob << " " << denom_sum << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, same,
                                                     (n_alice) * denom_sum,
                                                     k, 10, i++) << endl;

                }
            }
        }
    }
}


int main() {
    // test1();
    // test2();
    // test3();
    // test4();
    // test5();
    // test6();
}


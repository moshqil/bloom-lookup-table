#include "stress_tests.cpp"
#include <fstream>
#include <string>


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
    vector<int> ns = {100000};
    vector<double> denoms = {1.25, 1.3, 1.33, 1.36, 1.4, 1.5};

    int seed = 1;

    for (int k = 3; k <= 3; k++) {
        for (int n : ns) {
            for (auto denom : denoms) {
                int num_tests = 1000;
                cout << k << " " << n << " " << denom << " " << stress_test_list_entries(num_tests, n,
                            denom * n, k, 10, seed++) << endl;
                // кол-во хеш функций - пар - отношение размера блум фильтра к парам - процент успехов
            }
        }
    }
}

void test4() {
    vector<int> ns_alice = {1000};
    vector<double> denoms_sum = {3, 3.5};
    vector<int> ns_bob = {1000};

    int num_tests = 10000; int i = 0;
    cout << "Hash f - Alice pairs - Bob pairs - Alice denom - Bob denom - SUCCESS PERCENTAGE" << endl;
    for (int k = 3; k <= 3; k++) {
        for (auto n_alice : ns_alice) {
            for (auto n_bob : ns_bob) {
                for (auto denom_sum : denoms_sum) {
                    if (n_bob > n_alice) {
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
    vector<double> denoms_sum = {2, 3, 3.5};
    vector<int> ns_bob = {same};

    int num_tests = 100; int i = 0;
    cout << "Hash f - Alice pairs - Bob pairs - Alice denom - Bob denom - SUCCESS PERCENTAGE" << endl;
    for (int k = 3; k <= 7; k++) {
        for (auto n_alice : ns_alice) {
            for (auto n_bob : ns_bob) {
                for (auto denom_sum : denoms_sum) {
                    if (n_bob > n_alice) {
                        continue;
                    }
                    cout << k << " " << n_alice << " " << n_bob << " " << denom_sum << " " <<
                         stress_test_poisoned(num_tests, n_alice, n_bob, same / 4,
                                                                                (n_alice + n_bob) * denom_sum,
                                                                                k, 10, i++) << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, same / 2,
                                                 (n_alice + n_bob) * denom_sum,
                                                 k, 10, i++) << " ";
                    cout << stress_test_poisoned(num_tests, n_alice, n_bob, 3 * same / 4,
                                                 (n_alice + n_bob) * denom_sum,
                                                 k, 10, i++) << endl;
                }
            }
        }
    }
}

void test6() {
    int same = 1000;
    vector<int> ns_alice = {same};
    vector<double> denoms_sum = {3, 3.5};
    vector<int> ns_bob = {same};

    int num_tests = 100; int i = 0;
    cout << "Hash f - Alice pairs - Bob pairs - Alice denom - Bob denom - SUCCESS PERCENTAGE" << endl;
    for (int k = 3; k <= 7; k++) {
        for (auto n_alice : ns_alice) {
            for (auto n_bob : ns_bob) {
                for (auto denom_sum : denoms_sum) {
                    for (int j = 0; j <= same; j+=50) {
                        if (n_bob > n_alice) {
                            continue;
                        }
                        cout << k << " " << n_alice << " " << n_bob << " " << denom_sum << " " << j << " ";
                        cout << stress_test_poisoned(num_tests, n_alice, n_bob, j,
                                                     (n_alice + n_bob) * denom_sum,
                                                     k, 10, i++) << endl;
                    }

                }
            }
        }
    }
}

void test7() {
    ifstream alice_genome("14chr_f.txt");
    ifstream bob_genome("14chr_s.txt");

    vector<string> alice_keys, bob_keys;
    vector<int64_t> alice_values, bob_values;

    string s1, s2; int n;

    while (getline (alice_genome, s1)) {
        alice_keys.push_back(s1);
        getline(alice_genome, s2);
        n = stoi(s2);
        alice_values.push_back(n);
    }

    vector<KeyValuePair> bob_pairs;

    while (getline (bob_genome, s1)) {
        bob_keys.push_back(s1);
        getline(bob_genome, s2);
        n = stoi(s2);
        bob_values.push_back(n);
        bob_pairs.push_back({s1, n});
    }

    vector<double> denoms = {10};

    for (auto denom : denoms) {
        cout << denom << endl;
        int m = denom * (alice_keys.size() + bob_keys.size());
        int k = 3;
        int string_size = 5;
        auto BLT_a = BloomLookupTable(alice_keys.size(), m, k, 5);
        auto BLT_b = BloomLookupTable(bob_keys.size(), m, k, 5);

        for (int i = 0; i < alice_keys.size(); i++) {
            BLT_a.insert(alice_keys[i], alice_values[i]);
        }
        for (int i = 0; i < bob_keys.size(); i++) {
            BLT_b.insert(bob_keys[i], bob_values[i]);
        }

        auto BLT_sub = subtraction(BLT_a, BLT_b);

        Entries difference = BLT_sub.poisoned_list_entries(bob_pairs);

    }
    cout << alice_keys.size() << endl;
    cout << bob_keys.size() << endl;
    //auto BLT_a = BloomLookupTable()

    alice_genome.close();
    bob_genome.close();
}

int main() {
    // test1();
    // test2();
    // test3();
    // test4();
    // test5();
    // test6();
    test5();
}

#include <iostream>
#include <set>
#include <algorithm>

int SetTest() {
    std::set<int> cur_magic_set;
    cur_magic_set.insert(1);
    cur_magic_set.insert(2);
    cur_magic_set.insert(3);
    cur_magic_set.insert(4);
    std::set<int> original_magic_set;
    original_magic_set.insert(3);
    original_magic_set.insert(4);
    original_magic_set.insert(5);
    std::set<int> record;
    std::set_difference(cur_magic_set.begin(), cur_magic_set.end(), original_magic_set.begin(), original_magic_set.end(), std::inserter(record, record.begin()));

    return 0;
}
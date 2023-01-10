#include "priority_queue.hpp"
#include <iostream>
int main()
{
    std::cout << std::boolalpha;
    // Basic operations, including insert().
    PriorityQueue<std::string> p1(20);
    std::cout << p1.insert(10, "AA") << '\n';
    p1.insert(13, "BB");
    p1.insert(8, "CC");
    p1.insert(5, "DD");
    std::cout << p1;
    std::cout << "-----\n";
    p1.insert(3, "EE");
    std::cout << p1;
    std::cout << "Min: " << *(p1.getMinKey()) << ' '
        << *(p1.getMinValue()) << '\n';
    std::cout << "-----\n";
    p1.insert(1, "FF");
    p1.insert(7, "GG");
    std::cout << p1;
    std::cout << "Min: " << *(p1.getMinKey()) << ' '
        << *(p1.getMinValue()) << '\n';
    // deleteMin().
    std::cout << "=======\n";
    std::cout << p1.deleteMin() << '\n';
    std::cout << p1;
    std::cout << "-----\n";
    p1.deleteMin();
    std::cout << p1;
    std::cout << "Min: " << *(p1.getMinKey()) << ' '
        << *(p1.getMinValue()) << '\n';
    // Priority queue can get full.
    std::cout << "$$$$$$$\n";
    PriorityQueue<int> p2(2);
    std::cout << p2.insert(1, -3) << '\n';
    std::cout << p2.insert(2, -4) << '\n';
    std::cout << p2.insert(3, -5) << '\n';
    std::cout << "---End provided test cases---\n";

    std::cout << "---Additional test cases---\n";
    PriorityQueue<int> p3(15);
    std::cout << p3.insert(1, -3) << '\n'; // true
    std::cout << p3.insert(10, 10) << '\n'; // true
    std::cout << p3.insert(12, 15) << '\n'; // true
    std::cout << p3.insert(13, 20) << '\n'; // true
    std::cout << p3.insert(2, -4) << '\n'; // true
    std::cout << p3.insert(3, -5) << '\n'; // true
    std::cout << p3.insert(12, 120) << '\n'; // false
    std::cout << p3;

    std::cout << "---get---\n";
    int* s = p3.get(25);
    if(s!=nullptr) std::cout << *s << '\n';
    int* t = p3.get(10);
    std::cout << *t << '\n';
    *t = 110;

    std::cout << "---Remove---\n";
    std::cout << p3.remove(2) << '\n'; // true
    std::cout << p3.remove(12) << '\n'; // true
    std::cout << p3;
    std::cout << p3.remove(12) << '\n'; // false
    std::cout << p3.remove(20) << '\n'; // false
    std::cout << p3;

    std::cout << "---DecreaseKey---\n";
    std::cout << p3.decreaseKey(13, 4) << '\n'; // true
    std::cout << p3.decreaseKey(55, 4) << '\n'; // false
    std::cout << p3.decreaseKey(20, 4) << '\n'; // false
    std::cout << p3;

    std::cout << "---IncreaseKey---\n";
    std::cout << p3.increaseKey(13, 4) << '\n'; // false
    std::cout << p3.increaseKey(35, 4) << '\n'; // false
    std::cout << p3.increaseKey(3, 4) << '\n'; // true
    std::cout << p3;
}

/* Run code using following commands
 * g++ -Wall -Werror -g -std=c++14 demo_hash_table.cpp -o demo_hash_table
 * ./demo_hash_table
 */

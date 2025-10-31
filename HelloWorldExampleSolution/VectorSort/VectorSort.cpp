#include <iostream>
#include <vector>
#include <algorithm>

// Declare our comparison function (declarations do not need names for the parameters, only types)
bool compare(int64_t, int64_t);

int main()
{
    // Allocate a vector on the stack, no need for the heap here
    std::vector<int> vec;

    // int64_t is equivalent to long long int
    int64_t input = 0;
    // Read the first number from the standard input
    std::cin >> input;
    // Read while the input number is not 0
    while (input != 0)
    {
        vec.push_back(input);

        std::cin >> input;
    }

    // Sort the memory between vec.begin() and vec.end() using the function compare
    std::sort(vec.begin(), vec.end(), compare);

    // Output the sorted vector
    for (int i : vec)
        std::cout << i << " ";
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

bool compare(int64_t a, int64_t b)
{
    // The vector should be sorted in descending order
    return a > b;
}
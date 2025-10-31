// iostream contains input and output streams
#include <iostream>

// The main method does not need to be declared explicitly
int main()
{
    // Stream "Hello World!" to the standard output, followed by a newline character
    std::cout << "Hello World!" << std::endl;
    
    // No need to wait for a keypress in order to not close the console window immediately:
    // Visual Studio does this for us
    // Return 0 to indicate that the programm terminated succesfully: https://en.wikipedia.org/wiki/Exit_status
    return EXIT_SUCCESS;
}
#include <unistd.h>

int main() {

    /* 
    Read keypresses from user 

    When running, the terminal is hooked to the standard input
    Keyboard inputs will be read into the c variable
    */
    char c;
    // read() reads 1 byte from the standard input into the variable, c until there are no more bytes to read
    // read() returns the number of bytes read, and will return 0 when it reaches the end of a file
    while (read(STDIN_FILENO, &c, 1) == 1);

    return 0;
}
using namespace std;

#include <iostream>

int main() {
    char input[100];                             //Create a character array that can hold 100 characters
    cout << "Enter your name: ";                 //Ask the user to input their name
    cin >> input;                                //Accept the user's name and store it into input
    printf(input);                               //Print out the user's name
    return 0;
}
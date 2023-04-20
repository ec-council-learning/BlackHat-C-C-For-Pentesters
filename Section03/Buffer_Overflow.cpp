using namespace std;

#include <iostream>
#include <cstring>

using namespace std;

int main() {
    char buffer[5];                 //Character buffer to store name
    cout << "What is your name? ";  //Program asks user for their name
    cin >> buffer;                  //Input is received from the user and stored into buffer
    cout << buffer << endl;         //The name stored in buffer is printed out
    return 0;
}
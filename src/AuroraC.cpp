#include <iostream>
#include <thread>

using namespace std;

// Port simulation, test performance via console

int main(int argc, char **argv) {
	cout <<  "Hello world";
	std::this_thread::sleep_for(50s);
	return 0;
}

#include <iostream>
#include <thread>
#include "Tracy.hpp"

using namespace std;

// Port simulation, test performance via console

int main(int argc, char **argv) {
	tracy::StartupProfiler();
	tracy::SetThreadName("startup");

	cout <<  "Hello world" << flush;
	while(true){
		std::this_thread::sleep_for(50s);
	}
	cout <<  endl << "end" << flush;

	tracy::ShutdownProfiler();
	return 0;
}

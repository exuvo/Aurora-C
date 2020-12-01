#include <iostream>
#include <thread>
#include "Tracy.hpp"
#include "log4cxx/logger.h"
#include "log4cxx/xml/domconfigurator.h"
#include "log4cxx/helpers/exception.h"

#include "galaxy/Galaxy.hpp"

using namespace std;
using namespace log4cxx;

//TODO Port simulation, test performance via console/tracy
LoggerPtr log;

int main(int argc, char **argv) {
	tracy::StartupProfiler();
	tracy::SetThreadName("startup");

	log4cxx::xml::DOMConfigurator::configure("log4j.xml");
	log = Logger::getLogger("aurora");
	LOG4CXX_FATAL(log, "### Starting ###");

	cout <<  "starting" << endl << flush;

	vector<StarSystem*> starSystems { new StarSystem("test")};
//	vector<StarSystem*> starSystems { new StarSystem("a"), new StarSystem("b"), new StarSystem("c"), new StarSystem("d"), new StarSystem("e") };
	vector<Empire> empires { Empire("player1") };
	Galaxy galaxy(empires, starSystems);

	galaxy.init();
	
	cout <<  "running" << endl;

	while(true){
		std::this_thread::sleep_for(50s);
	}
	cout << endl << "end" << flush;

	tracy::ShutdownProfiler();
	return 0;
}

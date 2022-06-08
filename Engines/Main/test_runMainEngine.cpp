#define TEST_RUNMAINENGINE
#ifdef TEST_RUNMAINENGINE

#include "Main_Engine.h"


int main() {

	Main_Engine engine;
	engine_start_info startInfo;
	
	try {
		engine.start(startInfo);
	}
	catch (std::exception err) {
		std::cout << err.what() << std::endl;
	}

}


#endif
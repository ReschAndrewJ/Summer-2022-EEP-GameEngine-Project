#define TEST_RUNFULLENGINE
#ifdef TEST_RUNFULLENGINE


#include "../Engines/Main/Main_Engine.h"


int main() {

	Main_Engine engine;
	engine_start_info startInfo;

	startInfo.rootObject_filePath = "./test_runFullEngine/test_runFullEngineInst1.inst";
	startInfo.rootObject_objectName = "PerspectiveCamera";

	engine.start(startInfo);
	


}







#endif
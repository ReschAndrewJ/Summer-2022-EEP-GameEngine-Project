#include "Object_Test1.h"


Object_Test1::Object_Test1() {
	addClassIdentifier(OBJECT_TEST1);
	createAttribute("intA", Attribute::types::INTEGER);
	createAttribute("doubleA", Attribute::types::DOUBLE);
	createAttribute("boolA", Attribute::types::BOOLEAN);
	createAttribute("charA", Attribute::types::CHARACTER);
	createAttribute("stringA", Attribute::types::STRING);
}
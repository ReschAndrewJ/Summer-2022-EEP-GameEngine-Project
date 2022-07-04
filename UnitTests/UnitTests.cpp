#include "pch.h"
#include "CppUnitTest.h"

#define UNIT_TESTS

#include "../Attribute/Attribute.h"
#include "../Attribute/Attribute.cpp"

#define private public
#define protected public
#include "../Objects/Base/Object.h"
#include "../Objects/Base/Object.cpp"
#undef private
#undef protected

#include "../Objects/Test/Object_Test1.h"
#include "../Objects/Test/Object_Test1.cpp"

#define private public
#define protected public
#include "../Object_Loader/Object_Loader.h"
#include "../Object_Loader/Object_Loader.cpp"
#undef private
#undef protected



#include <iomanip>
#include <fstream>
#include <chrono>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(AttributeTests) {
	public:
		
		TEST_METHOD(BuildEmpty) {
			Attribute empty = Attribute();
			Assert::AreEqual((int)Attribute::types::EMPTY, (int)empty.getType());
		}
		TEST_METHOD(BuildEmptyInt) {
			Attribute empty_int = Attribute(Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)empty_int.getType());
			int val = empty_int;
			Assert::AreEqual(0, val);
		}
		TEST_METHOD(BuildEmptyDouble) {
			Attribute empty_d = Attribute(Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)empty_d.getType());
			double val = empty_d;
			Assert::AreEqual(0.0, val);
		}
		TEST_METHOD(BuildEmptyBool) {
			Attribute empty_b = Attribute(Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)empty_b.getType());
			bool val = empty_b;
			Assert::IsFalse(val);
		}
		TEST_METHOD(BuildEmptyChar) {
			Attribute empty_char = Attribute(Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)empty_char.getType());
			char val = empty_char;
			Assert::AreEqual('\0', val);
		}
		TEST_METHOD(BuildEmptyString) {
			Attribute empty_s = Attribute(Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)empty_s.getType());
			std::string val = empty_s;
			Assert::AreEqual("", "");
		}

		TEST_METHOD(BuildFilledInt) {
			int set_val = 12345;
			Attribute filled_int = Attribute(set_val, Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)filled_int.getType());
			int get_val = filled_int;
			Assert::AreEqual(set_val, get_val);
		}
		TEST_METHOD(BuildFilledDouble) {
			double set_val = 123.45;
			Attribute filled_double = Attribute(set_val, Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)filled_double.getType());
			double get_val = filled_double;
			Assert::AreEqual(set_val, get_val);
		}
		TEST_METHOD(BuildFilledBool) {
			bool set_val = true;
			Attribute filled_bool = Attribute(set_val, Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)filled_bool.getType());
			bool get_val = filled_bool;
			Assert::AreEqual(set_val, get_val);
		}
		TEST_METHOD(BuildFilledChar) {
			char set_val = 'h';
			Attribute filled_char = Attribute(set_val, Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)filled_char.getType());
			char get_val = filled_char;
			Assert::AreEqual(set_val, get_val);
		}
		TEST_METHOD(BuildFilledString) {
			std::string set_val = "dsrtnbuyog";
			Attribute filled_string = Attribute(set_val, Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)filled_string.getType());
			std::string get_val = filled_string;
			Assert::AreEqual(set_val, get_val);
		}

		TEST_METHOD(BuildImplicitInt) {
			int set_val = 12345;
			Attribute implicit_int = Attribute(set_val);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)implicit_int.getType());
			int get_val = implicit_int;
			Assert::AreEqual(get_val, set_val);
		}
		TEST_METHOD(BuildImplicitDouble) {
			double set_val = 123.45;
			Attribute filled_double = Attribute(set_val);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)filled_double.getType());
			double get_val = filled_double;
			Assert::AreEqual(set_val, get_val);
		}
		TEST_METHOD(BuildImplicitBool) {
			bool set_val = true;
			Attribute filled_bool = Attribute(set_val);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)filled_bool.getType());
			bool get_val = filled_bool;
			Assert::AreEqual(set_val, get_val);
		}
		TEST_METHOD(BuildImplicitChar) {
			char set_val = 'h';
			Attribute filled_char = Attribute(set_val);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)filled_char.getType());
			char get_val = filled_char;
			Assert::AreEqual(set_val, get_val);
		}
		TEST_METHOD(BuildImplicitString) {
			std::string set_val = "dsrtnbuyog";
			Attribute filled_string = Attribute(set_val);
			Assert::AreEqual((int)Attribute::types::STRING, (int)filled_string.getType());
			std::string get_val = filled_string;
			Assert::AreEqual(set_val, get_val);
		}

		TEST_METHOD(BuildCopy) {
			Attribute original = Attribute("ghrysek");
			Attribute copy = Attribute(original);
			Assert::AreEqual((std::string)original, (std::string)copy);
			void* o_ptr = &original;
			void* c_ptr = &copy;
			Assert::AreNotEqual(o_ptr, c_ptr);

		}

		TEST_METHOD(AssignInt) {
			int set_val = 123456;

			Attribute attrE;
			Assert::AreEqual((int)Attribute::types::EMPTY, (int)attrE.getType());
			attrE = set_val;
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrE.getType());
			Assert::AreEqual(set_val, (int)attrE);

			Attribute attrI = Attribute(Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrI.getType());
			Assert::AreEqual(0, (int)attrI);
			attrI = set_val;
			Assert::AreEqual(set_val, (int)attrI);

			Attribute attrD = Attribute(Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrD.getType());
			Assert::AreEqual(0.0, (double)attrD);
			attrD = set_val;
			Assert::AreEqual((double)set_val, (double)attrD);

			Attribute attrB = Attribute(Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::IsFalse((bool)attrB);
			bool err = false;
			try { attrB = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrC = Attribute(Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrC.getType());
			Assert::AreEqual('\0', (char)attrC);
			err = false;
			try { attrC = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrS = Attribute(Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrS.getType());
			Assert::AreEqual((std::string)"", (std::string)attrS);
			err = false;
			try { attrS = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);
		}
		TEST_METHOD(AssignDouble) {
			double set_val = 123456.789;

			Attribute attrE;
			Assert::AreEqual((int)Attribute::types::EMPTY, (int)attrE.getType());
			attrE = set_val;
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrE.getType());
			Assert::AreEqual(set_val, (double)attrE);

			Attribute attrI = Attribute(Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrI.getType());
			Assert::AreEqual(0, (int)attrI);
			attrI = set_val;
			Assert::AreEqual((double)(int)set_val, (double)attrI);

			Attribute attrD = Attribute(Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrD.getType());
			Assert::AreEqual(0.0, (double)attrD);
			attrD = set_val;
			Assert::AreEqual((double)set_val, (double)attrD);

			Attribute attrB = Attribute(Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::IsFalse((bool)attrB);
			bool err = false;
			try { attrB = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrC = Attribute(Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrC.getType());
			Assert::AreEqual('\0', (char)attrC);
			err = false;
			try { attrC = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrS = Attribute(Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrS.getType());
			Assert::AreEqual((std::string)"", (std::string)attrS);
			err = false;
			try { attrS = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);
		}
		TEST_METHOD(AssignBool) {
			bool set_val = true;

			Attribute attrE;
			Assert::AreEqual((int)Attribute::types::EMPTY, (int)attrE.getType());
			attrE = set_val;
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrE.getType());
			Assert::IsTrue((bool)attrE);

			Attribute attrI = Attribute(Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrI.getType());
			Assert::AreEqual(0, (int)attrI);
			bool err = false;
			try { attrI = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrD = Attribute(Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrD.getType());
			Assert::AreEqual(0.0, (double)attrD);
			err = false;
			try { attrD = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrB = Attribute(Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::IsFalse((bool)attrB);
			attrB = set_val;
			Assert::IsTrue(attrB);

			Attribute attrC = Attribute(Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrC.getType());
			Assert::AreEqual('\0', (char)attrC);
			err = false;
			try { attrC = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrS = Attribute(Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrS.getType());
			Assert::AreEqual((std::string)"", (std::string)attrS);
			err = false;
			try { attrS = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);
		}
		TEST_METHOD(AssignChar) {
			char set_val = '~';

			Attribute attrE;
			Assert::AreEqual((int)Attribute::types::EMPTY, (int)attrE.getType());
			attrE = set_val;
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrE.getType());
			Assert::AreEqual(set_val, (char)attrE);

			Attribute attrI = Attribute(Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrI.getType());
			Assert::AreEqual(0, (int)attrI);
			bool err = false;
			try { attrI = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrD = Attribute(Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrD.getType());
			Assert::AreEqual(0.0, (double)attrD);
			err = false;
			try { attrD = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrB = Attribute(Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::AreEqual(false, (bool)attrB);
			err = false;
			try { attrB = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrC = Attribute(Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrC.getType());
			Assert::AreEqual('\0', (char)attrC);
			attrC = set_val;
			Assert::AreEqual(set_val, (char)attrC);

			Attribute attrS = Attribute(Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrS.getType());
			Assert::AreEqual((std::string)"", (std::string)attrS);
			attrS = set_val;
			Assert::AreEqual(std::string(1, set_val), (std::string)attrS);
		}
		TEST_METHOD(AssignString) {
			std::string set_val = "gnihglashg";

			Attribute attrE;
			Assert::AreEqual((int)Attribute::types::EMPTY, (int)attrE.getType());
			attrE = set_val;
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrE.getType());
			Assert::AreEqual(set_val, (std::string)attrE);

			Attribute attrI = Attribute(Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrI.getType());
			Assert::AreEqual(0, (int)attrI);
			bool err = false;
			try { attrI = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrD = Attribute(Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrD.getType());
			Assert::AreEqual(0.0, (double)attrD);
			err = false;
			try { attrD = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrB = Attribute(Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::AreEqual(false, (bool)attrB);
			err = false;
			try { attrB = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrC = Attribute(Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrC.getType());
			Assert::AreEqual('\0', (char)attrC);
			err = false;
			try { attrC = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrS = Attribute(Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrS.getType());
			Assert::AreEqual((std::string)"", (std::string)attrS);
			attrS = set_val;
			Assert::AreEqual(set_val, (std::string)attrS);
		}
		TEST_METHOD(AssignConstCharPtr) {
			auto set_val = "gskughuks";

			Attribute attrE;
			Assert::AreEqual((int)Attribute::types::EMPTY, (int)attrE.getType());
			attrE = set_val;
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrE.getType());
			Assert::AreEqual((std::string)set_val, (std::string)attrE);

			Attribute attrI = Attribute(Attribute::types::INTEGER);
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrI.getType());
			Assert::AreEqual(0, (int)attrI);
			bool err = false;
			try { attrI = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrD = Attribute(Attribute::types::DOUBLE);
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrD.getType());
			Assert::AreEqual(0.0, (double)attrD);
			err = false;
			try { attrD = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrB = Attribute(Attribute::types::BOOLEAN);
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::AreEqual(false, (bool)attrB);
			err = false;
			try { attrB = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrC = Attribute(Attribute::types::CHARACTER);
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrC.getType());
			Assert::AreEqual('\0', (char)attrC);
			err = false;
			try { attrC = set_val; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);

			Attribute attrS = Attribute(Attribute::types::STRING);
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrS.getType());
			Assert::AreEqual((std::string)"", (std::string)attrS);
			attrS = set_val;
			Assert::AreEqual((std::string)set_val, (std::string)attrS);
		}
		TEST_METHOD(AssignAttribute) {
			Attribute attrE;
			Attribute attrS = "gkiashgu";
			Attribute attrD = 1257.794;

			Assert::AreEqual((int)Attribute::types::EMPTY, (int)attrE.getType());
			attrE = attrS;
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrE.getType());
			Assert::AreEqual((std::string)attrS, (std::string)attrE);

			bool err = false;
			try { attrE = attrD; }
			catch (std::runtime_error) { err = true; }
			Assert::IsTrue(err);
		}

		TEST_METHOD(INSERTION) {
			std::stringstream out;
			
			int val_i = 48648;
			Attribute attrI = val_i;
			out << val_i;
			Assert::AreEqual((int)Attribute::types::INTEGER, (int)attrI.getType());
			Assert::AreEqual(val_i, (int)attrI);
			Assert::AreEqual(val_i, std::stoi(out.str()));
			out.str("");

			double val_d = 4654.1651;
			Attribute attrD = val_d;
			out << std::setprecision(8) << attrD;
			Assert::AreEqual((int)Attribute::types::DOUBLE, (int)attrD.getType());
			Assert::AreEqual(val_d, (double)attrD);
			Assert::AreEqual(val_d, std::stod(out.str()));
			out.str("");

			Attribute attrB = true;
			out << attrB;
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::IsTrue((bool)attrB);
			Assert::AreEqual((std::string)"true", out.str());
			out.str("");

			attrB = false;
			out << attrB;
			Assert::AreEqual((int)Attribute::types::BOOLEAN, (int)attrB.getType());
			Assert::IsFalse((bool)attrB);
			Assert::AreEqual((std::string)"false", out.str());
			out.str("");
			
			char val_c = 'y';
			Attribute attrC = val_c;
			out << attrC;
			Assert::AreEqual((int)Attribute::types::CHARACTER, (int)attrC.getType());
			Assert::AreEqual(val_c, (char)attrC);
			Assert::AreEqual(val_c, out.str().c_str()[0]);
			out.str("");

			std::string val_s = "gvasihrei";
			Attribute attrS = val_s;
			out << attrS;
			Assert::AreEqual((int)Attribute::types::STRING, (int)attrS.getType());
			Assert::AreEqual(val_s, (std::string)attrS);
			Assert::AreEqual(val_s, out.str());
			out.str("");
			
		}

	};


	TEST_CLASS(BaseObjectTests) {
	public:

		TEST_METHOD(MockCreateDerived) {
			Object test_object;
			test_object.addClassIdentifier("test_class");
			Assert::IsTrue(test_object.is_class(OBJECT_CLASS_BASE));
			Assert::IsTrue(test_object.is_class("test_class"));
			Assert::AreEqual((std::string)"unnamed_object", test_object.getIdentifier());
			test_object.identifier = "test_object";
			Assert::AreEqual((std::string)"test_object", test_object.getIdentifier());
		}
		TEST_METHOD(AddAttribute) {
			Object test_object;
			test_object.createAttribute("test_attr", Attribute::types::DOUBLE);
			Assert::AreEqual((double)Attribute(Attribute::types::DOUBLE), (double)test_object.getAttribute("test_attr"));
			test_object.setAttribute("test_attr", 9.8);
			Assert::AreEqual(9.8, (double)test_object.getAttribute("test_attr"));
		}
		TEST_METHOD(GetObject) {
			std::unordered_map<std::string, Object*> objects;
			Object test_object;
			test_object.identifier = "test_object";
			test_object.objectContainerPtr = &objects;
			objects.insert({ test_object.identifier, &test_object});
			Assert::IsTrue(std::addressof(test_object) == std::addressof(*test_object.getObject("test_object")));
		}
		TEST_METHOD(MockObjectCreateQueue) {
			std::unordered_map<std::string, Object*> objects;
			std::vector<std::tuple< std::string, std::string, std::string>> mockCreateQueue;
			Object test_object;
			test_object.objectContainerPtr = &objects;
			test_object.identifier = "test_object";
			objects.insert({ test_object.identifier, &test_object });
			test_object.objectCreationQueuePtr = &mockCreateQueue;
			test_object.queueCreateObject("mock_path", "mock_object", "mock_parent");
			Assert::IsTrue(mockCreateQueue.size() == 1);
			Assert::IsTrue(std::get<1>(mockCreateQueue.back()) == "mock_object");
			test_object.queueCreateObject("mock_path", "mock_object2", "mock_parent");
			Assert::IsTrue(mockCreateQueue.size() == 2);
			Assert::IsTrue(std::get<1>(mockCreateQueue.back()) == "mock_object2");
		}


	};


	TEST_CLASS(ObjectLoaderTests) {

		TEST_METHOD_INITIALIZE(WriteFile) {
			std::fstream testFile1;
			testFile1.open("testInst1.inst", std::fstream::out);
			Assert::IsTrue(testFile1.is_open());

			testFile1 <<
				"# this is a comment" << std::endl <<
				std::endl <<
				"Object test_one Object_Test1" << std::endl <<
				std::endl <<
				" Integer intA 2556 " << std::endl <<
				"Double  doubleA 3.14" << std::endl <<
				"Boolean boolA true" << std::endl <<
				" Character  charA f " << std::endl <<
				"  String stringA \"str\"" << std::endl <<
				std::endl;
			testFile1.close();

			std::fstream testFile2;
			testFile2.open("testInst2.inst", std::fstream::out);
			Assert::IsTrue(testFile2.is_open());

			testFile2 <<
				"# this is a comment" << std::endl <<
				std::endl <<
				"Object test_one Object_Test1" << std::endl <<
				std::endl <<
				"Integer intA 2556   " << std::endl <<
				"   Double doubleA 3.14" << std::endl <<
				" Boolean  boolA  true  " << std::endl <<
				"Character charA f" << std::endl <<
				" String stringA \"str\"" << std::endl <<
				std::endl <<
				"Child Here test_two" << std::endl <<
				std::endl <<
				"# this is a comment" << std::endl <<
				std::endl <<
				"Object test_two Object_Test1 " << std::endl <<
				std::endl <<
				"Integer intA 345" << std::endl <<
				"Double   doubleA 12.34" << std::endl <<
				"Boolean boolA false" << std::endl <<
				"Character charA n" << std::endl <<
				"String stringA \"av n\"" << std::endl <<
				std::endl <<
				"Child Here test_three" << std::endl <<
				std::endl <<
				"# this is a comment" << std::endl <<
				std::endl <<
				"Object test_three Object_Test1" << std::endl <<
				std::endl <<
				"Integer intA 565" << std::endl <<
				"Double doubleA 7.8" << std::endl <<
				"Boolean boolA true" << std::endl <<
				"Character charA m" << std::endl <<
				"String stringA \"it is a string\"" << std::endl;
			testFile2.close();
		}

		TEST_METHOD_CLEANUP(ClearFiles) {
			remove("testInst1.inst");
			remove("testInst2.inst");
		}

		TEST_METHOD(LoadSingle) {
			std::string filepath = "testInst1.inst";
			Object_Loader testLoader;
			testLoader.setupBuiltInClasses();
			Object* test_obj = testLoader.createInstance(filepath, "test_one").first;
			delete test_obj;
			Assert::AreEqual((size_t)1, testLoader.loadedObjectInfos.size());
			Assert::AreEqual(filepath + '/' + "test_one", testLoader.loadedObjectInfos.begin()->first);
			Assert::AreEqual(filepath, *testLoader.loadedFiles.begin());
		}

		TEST_METHOD(LoadSingle_values) {
			std::string filepath = "testInst1.inst";
			Object_Loader testLoader;
			testLoader.setupBuiltInClasses();
			Object* test_obj = testLoader.createInstance(filepath, "test_one").first;
			Assert::AreEqual(2556, (int)test_obj->getAttribute("intA"));
			Assert::AreEqual(3.14, (double)test_obj->getAttribute("doubleA"));
			Assert::IsTrue((bool)test_obj->getAttribute("boolA"));
			Assert::AreEqual('f', (char)test_obj->getAttribute("charA"));
			Assert::AreEqual((std::string)"str", (std::string)test_obj->getAttribute("stringA"));
			delete test_obj;
		}


		TEST_METHOD(LoadMultiple) {
			std::string filepath = "testInst2.inst";
			Object_Loader testLoader;
			testLoader.setupBuiltInClasses();
			auto test_obj1 = testLoader.createInstance(filepath, "test_one");
			auto test_obj2 = testLoader.createInstance(test_obj1.second[0].first, test_obj1.second[0].second);
			auto test_obj3 = testLoader.createInstance(test_obj2.second[0].first, test_obj2.second[0].second);
			delete test_obj1.first;
			delete test_obj2.first;
			delete test_obj3.first;
			Assert::AreEqual((size_t)3, testLoader.loadedObjectInfos.size());
			Assert::AreEqual(filepath, *testLoader.loadedFiles.begin());
			Assert::AreEqual((size_t)1, testLoader.loadedObjectInfos.count(filepath + '/' + "test_one"));
			Assert::AreEqual((size_t)1, testLoader.loadedObjectInfos.count(filepath + '/' + "test_two"));
			Assert::AreEqual((size_t)1, testLoader.loadedObjectInfos.count(filepath + '/' + "test_three"));

		}

		TEST_METHOD(LoadMultiple_values) {
			std::string filepath = "testInst2.inst";
			Object_Loader testLoader;
			testLoader.setupBuiltInClasses();
			auto test_obj1 = testLoader.createInstance(filepath, "test_one");
			auto test_obj2 = testLoader.createInstance(test_obj1.second[0].first, test_obj1.second[0].second);
			auto test_obj3 = testLoader.createInstance(test_obj2.second[0].first, test_obj2.second[0].second);
			Assert::AreEqual(2556, (int)test_obj1.first->getAttribute("intA"));
			Assert::AreEqual(345, (int)test_obj2.first->getAttribute("intA"));
			Assert::AreEqual(565, (int)test_obj3.first->getAttribute("intA"));
			Assert::AreEqual((std::string)"str", (std::string)test_obj1.first->getAttribute("stringA"));
			Assert::AreEqual((std::string)"av n", (std::string)test_obj2.first->getAttribute("stringA"));
			Assert::AreEqual((std::string)"it is a string", (std::string)test_obj3.first->getAttribute("stringA"));
			delete test_obj1.first;
			delete test_obj2.first;
			delete test_obj3.first;
		}		

	};
	
	TEST_CLASS(ObjectLoaderPerformance) {
		const size_t numberOfFiles = 10;
		const size_t numberOfObjects = 100;
		TEST_METHOD_INITIALIZE(WriteFile) {
			for (size_t fNum = 1; fNum <= numberOfFiles; ++fNum) {
				std::fstream testFile;
				testFile.open("test_file" + std::to_string(fNum) + ".inst", std::fstream::out);
				Assert::IsTrue(testFile.is_open());
				
				for (size_t objNum = 1; objNum <= numberOfObjects; ++objNum) {
					testFile << std::endl;
					testFile << "# Object trick_comment" << objNum << " " << OBJECT_CLASS_BASE << std::endl;
					testFile << "Object test_object" << objNum << " " << OBJECT_CLASS_TEST1 << std::endl;
					testFile << "Integer intA " << objNum << std::endl;
					testFile << "Double doubleA " << fNum << "." << objNum << std::endl;
					testFile << "Boolean boolA " << (objNum % 2 ? "true" : "false") << std::endl;
					testFile << "Character charA " << (char)(objNum % 26 + 97) << std::endl;
					testFile << "String stringA \"" << "a string " << objNum << "\"" << std::endl;
					if (objNum < numberOfObjects) {
						testFile << "Child Here test_object" << objNum + 1 << std::endl;
					}
					else if (fNum < numberOfFiles) {
						testFile << "Child test_file" << std::to_string(fNum + 1) << ".inst test_object1" << std::endl;
					}
				}
				testFile.close();
			}
		}
		TEST_METHOD_CLEANUP(ClearFile) {
			for (size_t fNum = 1; fNum <= numberOfFiles; ++fNum) {
				remove(("test_file" + std::to_string(fNum) + ".inst").data());
			}
		}

		TEST_METHOD(TestPerformance) {
			const size_t cycles = 100;
			double combined_time = 0;
			for (size_t i = 0; i < cycles; ++i) {
				auto start = std::chrono::steady_clock::now();

				Object_Loader testLoader;
				testLoader.setupBuiltInClasses();
				auto test_obj = testLoader.createInstance("test_file1.inst", "test_object1");
				delete test_obj.first;

				bool has_child = !test_obj.second.empty();
				while (has_child) {
					test_obj = testLoader.createInstance(test_obj.second[0].first, test_obj.second[0].second);
					delete test_obj.first;
					if (test_obj.second.empty()) has_child = false;
				}

				auto end = std::chrono::steady_clock::now();
				std::chrono::duration<double, std::milli> elapsed = end - start;
				combined_time += elapsed.count();
			}
			double avg = combined_time / cycles;
			Logger::WriteMessage(("Average Time: " + std::to_string(avg) + "ms").data());

		}
		
	};



}

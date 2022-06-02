#include "pch.h"
#include "CppUnitTest.h"

#include "../Attribute/Attribute.h"
#include "../Attribute/Attribute.cpp"

#include <iomanip>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(AttributeTests)
	{
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
}

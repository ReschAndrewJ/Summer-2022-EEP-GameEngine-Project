#include "Attribute.h"

#include <stdexcept>


// get attribute type

Attribute::types Attribute::getType() const { return val_type; }


// constructors -------------

Attribute::Attribute() {
	val_type = types::EMPTY;
	value = 0;
}
Attribute::Attribute(types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		value = 0;
		break;
	case types::INTEGER:
		value = 0;
		break;
	case types::DOUBLE:
		value = 0.0;
		break;
	case types::BOOLEAN:
		value = false;
		break;
	case types::CHARACTER:
		value = '\0';
		break;
	case types::STRING:
		value = "";
		break;
	default:
		throw std::runtime_error("unsupported type");
	}
}
Attribute::Attribute(int i, types t) {
	val_type = t;
	switch (t) {
		case types::EMPTY:
			val_type = types::INTEGER;
			value = (long long)i;
			return;
		case types::INTEGER:
			value = (long long)i;
			return;
		case types::DOUBLE:
			value = (double)i;
			return;
		case types::BOOLEAN:
			value = (bool)i;
			return;
		default:
			throw std::runtime_error("unsupported attribute type for value of type int");
	}
}
Attribute::Attribute(short i, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::INTEGER;
		value = (long long)i;
		return;
	case types::INTEGER:
		value = (long long)i;
		return;
	case types::DOUBLE:
		value = (double)i;
		return;
	case types::BOOLEAN:
		value = (bool)i;
		return;
	default:
		throw std::runtime_error("unsupported attribute type for value of type short");
	}
}
Attribute::Attribute(long i, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::INTEGER;
		value = (long long)i;
		return;
	case types::INTEGER:
		value = (long long)i;
		return;
	case types::DOUBLE:
		value = (double)i;
		return;
	case types::BOOLEAN:
		value = (bool)i;
		return;
	default:
		throw std::runtime_error("unsupported attribute type for value of type long");
	}
}
Attribute::Attribute(long long i, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::INTEGER;
		value = (long long)i;
		return;
	case types::INTEGER:
		value = (long long)i;
		return;
	case types::DOUBLE:
		value = (double)i;
		return;
	case types::BOOLEAN:
		value = (bool)i;
		return;
	default:
		throw std::runtime_error("unsupported attribute type for value of type long long");
	}
}
Attribute::Attribute(float f, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::DOUBLE;
		value = (double)f;
		return;
	case types::INTEGER:
		value = (long long)f;
		return;
	case types::DOUBLE:
		value = (double)f;
		return;
	case types::BOOLEAN:
		value = (bool)f;
		return;
	default:
		throw std::runtime_error("unsupported attribute type for value of type float");
	}
}
Attribute::Attribute(double d, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::DOUBLE;
		value = (double)d;
		return;
	case types::INTEGER:
		value = (long long)d;
		return;
	case types::DOUBLE:
		value = (double)d;
		return;
	case types::BOOLEAN:
		value = (bool)d;
		return;
	default:
		throw std::runtime_error("unsupported attribute type for value of type double");
	}
}
Attribute::Attribute(bool b, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::BOOLEAN;
		value = (bool)b;
		return;
	case types::BOOLEAN:
		value = (bool)b;
		return;
	default:
		throw std::runtime_error("unsupported attribute type for value of type bool");
	}
}
Attribute::Attribute(char c, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::CHARACTER;
		value = c;
		return;
	case types::CHARACTER:
		value = c;
		return;
	case types::STRING:
		value = std::string(1, c);
		return;
	default:
		throw std::runtime_error("unsupported attribute type for value of type char");
	}
}
Attribute::Attribute(std::string s, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::STRING;
		value = s;
		return;
	case types::STRING:
		value = s;
		return;
	default:
		throw std::runtime_error("unsupported attribute for value of type string");
	}
}
Attribute::Attribute(const char* s, types t) {
	val_type = t;
	switch (t) {
	case types::EMPTY:
		val_type = types::STRING;
		value = std::string(s);
		return;
	case types::STRING:
		value = std::string(s);
		return;
	default:
		throw std::runtime_error("unsupported attribute for value of type const char*");
	}
}
Attribute::Attribute(const Attribute& rAttr) {
	val_type = rAttr.val_type;
	value = rAttr.value;
}


// casts ---------------

Attribute::operator int() const {
	switch (val_type) {
	case types::INTEGER:
		return (int)std::get<long long>(value);
	case types::DOUBLE:
		return (int)std::get<double>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type int");
	}
}

Attribute::operator short() const {
	switch (val_type) {
	case types::INTEGER:
		return (short)std::get<long long>(value);
	case types::DOUBLE:
		return (short)std::get<double>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type short");
	}
}

Attribute::operator long() const {
	switch (val_type) {
	case types::INTEGER:
		return (long)std::get<long long>(value);
	case types::DOUBLE:
		return (long)std::get<double>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type long");
	}
}

Attribute::operator long long() const {
	switch (val_type) {
	case types::INTEGER:
		return std::get<long long>(value);
	case types::DOUBLE:
		return (long long)std::get<double>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type long long");
	}
}

Attribute::operator float() const {
	switch (val_type) {
	case types::INTEGER:
		return (float)std::get<long long>(value);
	case types::DOUBLE:
		return (float)std::get<double>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type float");
	}
}

Attribute::operator double() const {
	switch (val_type) {
	case types::INTEGER:
		return (double)std::get<long long>(value);
	case types::DOUBLE:
		return std::get<double>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type double");
	}
}

Attribute::operator bool() const{
	switch (val_type) {
	case types::INTEGER:
		return (bool)std::get<int64_t>(value);
	case types::DOUBLE:
		return (bool)std::get<double>(value);
	case types::BOOLEAN:
		return std::get<bool>(value);
	case types::CHARACTER:
		return (bool)std::get<char>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type bool");
	}
}

Attribute::operator char() const {
	switch (val_type) {
	case types::CHARACTER:
		return std::get<char>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type char");
	}
}

Attribute::operator std::string() const {
	switch (val_type) {
	case types::CHARACTER:
		return std::string(1, std::get<char>(value));
	case types::STRING:
		return std::get<std::string>(value);
	default:
		throw std::runtime_error("invalid attribute for cast of type string");
	}
}


// assignment overloads ----

Attribute& Attribute::operator=(int i) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::INTEGER;
		value = (long long)i;
		return *this;
	case types::INTEGER:
		value = (long long)i;
		return *this;
	case types::DOUBLE:
		value = (double)i;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type int");
	}
}
Attribute& Attribute::operator=(short i) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::INTEGER;
		value = (long long)i;
		return *this;
	case types::INTEGER:
		value = (long long)i;
		return *this;
	case types::DOUBLE:
		value = (double)i;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type short");
	}
}
Attribute& Attribute::operator=(long i) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::INTEGER;
		value = (long long)i;
		return *this;
	case types::INTEGER:
		value = (long long)i;
		return *this;
	case types::DOUBLE:
		value = (double)i;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type long");
	}
}
Attribute& Attribute::operator=(long long i) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::INTEGER;
		value = i;
		return *this;
	case types::INTEGER:
		value = i;
		return *this;
	case types::DOUBLE:
		value = (double)i;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type long long");
	}
}
Attribute& Attribute::operator=(float f) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::DOUBLE;
		value = (double)f;
		return *this;
	case types::INTEGER:
		value = (long long)f;
		return *this;
	case types::DOUBLE:
		value = (double)f;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type float");
	}
}
Attribute& Attribute::operator=(double d) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::DOUBLE;
		value = d;
		return *this;
	case types::INTEGER:
		value = (long long)d;
		return *this;
	case types::DOUBLE:
		value = d;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type double");
	}
}
Attribute& Attribute::operator=(bool b) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::BOOLEAN;
		value = b;
		return *this;
	case types::BOOLEAN:
		value = b;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type bool");
	}
}
Attribute& Attribute::operator=(char c) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::CHARACTER;
		value = c;
		return *this;
	case types::CHARACTER:
		value = c;
		return *this;
	case types::STRING:
		value = std::string(1, c);
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type char");
	}
}
Attribute& Attribute::operator=(std::string s) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::STRING;
		value = s;
		return *this;
	case types::STRING:
		value = s;
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of string");
	}
}
Attribute& Attribute::operator=(const char* s) {
	switch (val_type) {
	case types::EMPTY:
		val_type = types::STRING;
		value = std::string(s);
		return *this;
	case types::STRING:
		value = std::string(s);
		return *this;
	default:
		throw std::runtime_error("invalid attribute for assignment of type const char*");
	}
}
Attribute& Attribute::operator=(const Attribute& rAttr) {
	switch (rAttr.getType()) {
	case types::EMPTY:
		if (val_type != types::EMPTY) {
			printf("cannot assign non-empty attribute equal to an empty attribute");
		}
		break;
	case types::INTEGER:
		*this = (long long)rAttr;
		break;
	case types::DOUBLE:
		*this = (double)rAttr;
		break;
	case types::BOOLEAN:
		*this = (bool)rAttr;
		break;
	case types::CHARACTER:
		*this = (char)rAttr;
		break;
	case types::STRING:
		*this = (std::string)rAttr;
		break;
	default:
		printf("unhandled type for attribute assignment: %i", (int)rAttr.getType());
		break;
	}
	return *this;
}


std::ostream& operator<<(std::ostream& os, const Attribute& attr) {
	switch (attr.getType()) {
		case Attribute::types::EMPTY:
			os << "EMPTY";
			return os;
		case Attribute::types::INTEGER:
			os << (long long)attr;
			return os;
		case Attribute::types::DOUBLE:
			os << (double)attr;
			return os;
		case Attribute::types::BOOLEAN:
			os << (attr ? "true" : "false");
			return os;
		case Attribute::types::CHARACTER:
			os << (char)attr;
			return os;
		case Attribute::types::STRING:
			os << (std::string)attr;
			return os;
		default:
			throw std::runtime_error("unsupported type");
	}
}
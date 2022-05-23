#ifndef ATTRIBUTE_CLASS
#define ATTRIBUTE_CLASS

#include <string>
#include <variant>
#include <ostream>

// Class to hold attribute data members of the object classes

class Attribute
{

	// store the type of the attribute for type-safety
public:
	enum class types { EMPTY, INTEGER, DOUBLE, BOOLEAN, CHARACTER, STRING };


private:
	types val_type;
	std::variant<long long, double, bool, char, std::string> value; 
	// because the string class is the largest variant member, there is no reason not to use double and long long


public:
	types getType() const;

	
public:
	Attribute(); // default (empty) constructor

	Attribute(types);
	Attribute(int, types t = types::EMPTY); //////// Multiple overloads for ints
	Attribute(short, types t = types::EMPTY);
	Attribute(long, types t = types::EMPTY); ///////
	Attribute(long long, types t = types::EMPTY); //
	Attribute(float, types t = types::EMPTY); /// Multiple overloads for floating points
	Attribute(double, types t = types::EMPTY); //
	Attribute(bool, types t = types::EMPTY);
	Attribute(char, types t = types::EMPTY);
	Attribute(std::string, types t = types::EMPTY);/// multiple overloads for strings
	Attribute(const char*, types t = types::EMPTY); //

	Attribute(const Attribute&); // copy constructor


	// use cast operators to avoid requiring templated functions --
public:
	// getters
	operator int() const; 
	operator short() const;
	operator long() const; 
	operator long long() const; 
	operator float() const; 
	operator double() const; 
	operator bool() const;
	operator char() const;
	operator std::string() const;


public:
	// setters
	Attribute& operator=(int);
	Attribute& operator=(short);
	Attribute& operator=(long);
	Attribute& operator=(long long);
	Attribute& operator=(float);
	Attribute& operator=(double);
	Attribute& operator=(bool);
	Attribute& operator=(char);
	Attribute& operator=(std::string);
	Attribute& operator=(const char*);
	Attribute& operator=(const Attribute&);


};


// overload ostream insertion operator

std::ostream& operator<<(std::ostream&, const Attribute&);



#endif // ATTRIBUTE_CLASS
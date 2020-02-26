#pragma once

#include <string>


NS_CC_BEGIN


class Object;
class Bool;
class Integer;
class Float;
class Double;
class String;
class Array;
class Dictionary;
class Set;


class CC_DLL DataVisitor
{
public:
	virtual ~DataVisitor() {}

	/** default method, called from non-overloaded methods and for unrecognized objects */
	virtual void visitObject(const Object* p) = 0;

	virtual void visit(const Bool* p);
	virtual void visit(const Integer* p);
	virtual void visit(const Float* p);
	virtual void visit(const Double* p);
	virtual void visit(const String* p);
	virtual void visit(const Array* p);
	virtual void visit(const Dictionary* p);
	virtual void visit(const Set* p);
};


class CC_DLL PrettyPrinter : public DataVisitor
{
public:
	PrettyPrinter(int indentLevel = 0);

	virtual void clear();
	virtual std::string getResult();

	virtual void visitObject(const Object* p);
	virtual void visit(const Bool* p);
	virtual void visit(const Integer* p);
	virtual void visit(const Float* p);
	virtual void visit(const Double* p);
	virtual void visit(const String* p);
	virtual void visit(const Array* p);
	virtual void visit(const Dictionary* p);
	virtual void visit(const Set* p);
private:
	void setIndentLevel(int indentLevel);
	int _indentLevel;
	std::string _indentStr;
	std::string _result;
};





NS_CC_END


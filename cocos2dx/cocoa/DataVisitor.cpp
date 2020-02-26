#include "DataVisitor.h"

#include "Object.h"
#include "Bool.h"
#include "Integer.h"
#include "Float.h"
#include "Double.h"
#include "String.h"
#include "Array.h"
#include "Dictionary.h"
#include "Set.h"



NS_CC_BEGIN

void DataVisitor::visit(const Bool* value)
{
	visitObject(value);
}

void DataVisitor::visit(const Integer* value)
{
	visitObject(value);
}

void DataVisitor::visit(const Float* value)
{
	visitObject(value);
}

void DataVisitor::visit(const Double* value)
{
	visitObject(value);
}

void DataVisitor::visit(const String* value)
{
	visitObject(value);
}

void DataVisitor::visit(const Array* value)
{
	visitObject(value);
}

void DataVisitor::visit(const Dictionary* value)
{
	visitObject(value);
}

void DataVisitor::visit(const Set* value)
{
	visitObject(value);
}




//========================================
// CCPrettyPrinter
//
PrettyPrinter::PrettyPrinter(int indentLevel/* = 0 */)
{
	setIndentLevel(indentLevel);
}

void PrettyPrinter::clear()
{
	_result.clear();
}

std::string PrettyPrinter::getResult()
{
	return _result;
}

void PrettyPrinter::visitObject(const Object* p)
{
	char buf[50] = { 0 };
	sprintf(buf, "%p", p);
	_result += buf;
}

void PrettyPrinter::visit(const Bool* p)
{
	char buf[50] = { 0 };
	sprintf(buf, "%s", p->getValue() ? "true" : "false");
	_result += buf;
}

void PrettyPrinter::visit(const Integer * p)
{
	char buf[50] = { 0 };
	sprintf(buf, "%d", p->getValue());
	_result += buf;
}

void PrettyPrinter::visit(const Float * p)
{
	char buf[50] = { 0 };
	sprintf(buf, "%f", p->getValue());
	_result += buf;
}

void PrettyPrinter::visit(const Double * p)
{
	char buf[50] = { 0 };
	sprintf(buf, "%lf", p->getValue());
	_result += buf;
}

void PrettyPrinter::visit(const String * p)
{
	_result += p->getCString();
}

void PrettyPrinter::visit(const Array * p)
{
	_result += "\n";
	_result += _indentStr;
	_result += "<array>\n";

	setIndentLevel(_indentLevel + 1);
	Object * obj;
	int i = 0;
	char buf[50] = { 0 };
	ARRAY_FOREACH(p, obj)
	{
		if (i > 0) {
			_result += "\n";
		}
		sprintf(buf, "%s%02d: ", _indentStr.c_str(), i);
		_result += buf;
		PrettyPrinter v(_indentLevel);
		obj->acceptVisitor(v);
		_result += v.getResult();
		i++;
	}
	setIndentLevel(_indentLevel - 1);

	_result += "\n";
	_result += _indentStr;
	_result += "</array>";
}

void PrettyPrinter::visit(const Dictionary * p)
{
	_result += "\n";
	_result += _indentStr;
	_result += "<dict>\n";

	setIndentLevel(_indentLevel + 1);
	DictElement * element;
	bool bFirstElement = true;
	char buf[1000] = { 0 };
	DICT_FOREACH(p, element)
	{
		if (!bFirstElement) {
			_result += "\n";
		}
		sprintf(buf, "%s%s: ", _indentStr.c_str(), element->getStrKey());
		_result += buf;
		PrettyPrinter v(_indentLevel);
		element->getObject()->acceptVisitor(v);
		_result += v.getResult();
		bFirstElement = false;
	}
	setIndentLevel(_indentLevel - 1);

	_result += "\n";
	_result += _indentStr;
	_result += "</dict>";
}

void PrettyPrinter::visit(const Set * p)
{
	_result += "\n";
	_result += _indentStr;
	_result += "<set>\n";

	setIndentLevel(_indentLevel + 1);

	int i = 0;
	Set * tmp = const_cast<Set*>(p);
	SetIterator it = tmp->begin();

	for (; it != tmp->end(); ++it, ++i) {
		if (i > 0) {
			_result += "\n";
		}
		_result += _indentStr.c_str();
		PrettyPrinter v(_indentLevel);
		(*it)->acceptVisitor(v);
		_result += v.getResult();
	}
	setIndentLevel(_indentLevel - 1);

	_result += "\n";
	_result += _indentStr;
	_result += "</set>\n";
}

void PrettyPrinter::setIndentLevel(int indentLevel)
{
	_indentLevel = indentLevel;
	_indentStr.clear();
	for (int i = 0; i < _indentLevel; ++i) {
		_indentStr += "\t";
	}
}

NS_CC_END

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

#include "LogicExpr.h"

// A empty constructor
LogicExpr::LogicExpr() {
	op = Operand::Empty;
}


// Constructs the expression from a string of simple charecters:
// NOT    OR    AND    Implication    Biconditional
//  ~     |      &	       ->              <->
// This suports multiple place and's and or's
LogicExpr::LogicExpr(const std::string& const_string_rep) {
	if (const_string_rep.size() == 0) {
		op = Operand::None;
		return;
	}
	// Remove spaces.  These will be added back in when printing
	std::string string_rep(const_string_rep);
	std::string::iterator it = string_rep.begin();
	bool is_literal = true;
	while (it != string_rep.end()) {
		if (isspace(*it)) {
			it = string_rep.erase(it);
		}
		else {
			// Keep an eye on whether or not this is a simple literal
			if (is_literal && (!((isalpha(*it)) || (*it == '(') || (*it == ')')))) {
				is_literal = false;
			}
			it++;
		}
	}
	if (is_literal) {
		// If it is a literal, get rid of excess parenthesis and then store the remainer
		while ((string_rep[0] == '(') && (string_rep[string_rep.size() - 1] == ')')) {
			string_rep = string_rep.substr(1, string_rep.size() - 2);
		}
		op = Operand::Literal;
		literal = string_rep;
	}
	else {
		// If it is not a literal, call the complex constructor
		set_from_string(string_rep);
	}
}

void LogicExpr::set_from_string(const std::string& string_rep) {
	if (string_rep[0] == '~') {
		// If it starts with a negation, see what happens if we look at only the rest of the string
		op = Operand::Not;
		LogicExpr theRest(string_rep.substr(1));
		// If the string without the negation is a literal or a negation itself, we good 
		if ((theRest.op == Operand::Literal) || (theRest.op == Operand::Not)) {
			propositions.push_back(theRest);
			return;
		}
		else if ((string_rep[1] == '(') && (string_rep[string_rep.size() - 1] == ')')) {
			LogicExpr theRest2(string_rep.substr(2, string_rep.size() - 3));
			if (theRest2.op != Operand::None) {
				propositions.push_back(theRest2);
				return;
			}
		}
	}
	int level = 0;
	op = Operand::None;
	unsigned int sub_prop_start = string_rep.size();

	// The whole idea is keeping trak of parethesis level
	// we only care about operands that arent in parethesis, and where the outhermost levels start and end
	for (unsigned int i = 0; i<string_rep.size(); ++i) {
		if (string_rep[i] == '(') {
			// Start a new parenthesis level
			if (level == 0) {
				if (sub_prop_start != string_rep.size()) {
					if (string_rep[sub_prop_start] != '~') {
						op = Operand::None;
						break;
					}
				}
				else {
					sub_prop_start = i;
				}
			}
			level++;
		}
		else if (string_rep[i] == ')') {
			// End a parenthesis level
			level--;
			if (level == 0) {
				// If this ended the outer most level, make a logic expression out of the contents
				if (sub_prop_start == string_rep.size()) {
					op = Operand::None;
					break;
				}
				else if ((sub_prop_start == 1) && (i == (string_rep.size() - 1))) {
					// If the whole thing was in parenthesis, drop them and try again
					set_from_string(string_rep.substr(1, string_rep.size() - 2));
					return;
				}
				if (string_rep[sub_prop_start] == '(') {
					// if this entire sub expression was in parethesis, drop them and make and expression out of the contents
					propositions.push_back(LogicExpr(string_rep.substr(sub_prop_start + 1, i - sub_prop_start - 1)));
				}
				else {
					// otherwaise, keep them (in case it started with a negation)
					propositions.push_back(LogicExpr(string_rep.substr(sub_prop_start, i - sub_prop_start + 1)));
				}
				sub_prop_start = string_rep.size();
			}
			else if (level < 0) {
				op = Operand::None;
				break;
			}
		}
		else if (level == 0) {
			// If we are not in parethesis, find the operand or the literal
			if (isalpha(string_rep[i]) || string_rep[i] == '~') {
				if (sub_prop_start == string_rep.size())
					sub_prop_start = i;
			}
			else {
				if (sub_prop_start != string_rep.size()) {
					propositions.push_back(LogicExpr(string_rep.substr(sub_prop_start, i - sub_prop_start)));
					sub_prop_start = string_rep.size();
				}
				if (string_rep[i] == '&') {
					if (op == Operand::None) {
						op = Operand::And;
					}
					else if (op != Operand::And) {
						op = Operand::None;
						break;
					}
				}
				else if (string_rep[i] == '|') {
					if (op == Operand::None) {
						op = Operand::Or;
					}
					else if (op != Operand::Or) {
						op = Operand::None;
						break;
					}
				}
				else if (string_rep.substr(i, 2) == "->") {
					i += 1;
					if (op == Operand::None) {
						op = Operand::Implication;
					}
					else {
						// Multi-place not allowed
						op = Operand::None;
						break;
					}
				}
				else if (string_rep.substr(i, 3) == "<->") {
					i += 2;
					if (op == Operand::None) {
						op = Operand::Biconditional;
					}
					else {
						// Multi-place not allowed
						op = Operand::None;
						break;
					}
				}
			}
		}
	}
	// if we were building an propostion, finish it
	if ((sub_prop_start != string_rep.size()) && (op != Operand::None)) {
		propositions.push_back(LogicExpr(string_rep.substr(sub_prop_start)));
	}
}



// Recursively checks if the expression is valid, as the constructor is set to never fail
// Note: It never fails to help indicated where the parsing error was,
//        as the chunk that was unreadable will be displayed as such
bool LogicExpr::isValid() const {
	if ((op == Operand::None) || (op == Operand::Empty)) {
		return false;
	}
	else if (op == Operand::Literal) {
		return true;
	}
	for (unsigned int i = 0; i < propositions.size(); ++i) {
		if (!propositions[i].isValid()) {
			return false;
		}
	}
	return true;
}



// Handles breaking down multi-place AND's and OR's one 
// proposition at a time
LogicExpr oneLessPlace(const LogicExpr& longExpr) {
	if (longExpr.propositions.size() == 2) {
		return longExpr.propositions[1];
	}
	else {
		LogicExpr shortExpr;
		shortExpr.op = longExpr.op;
		for (unsigned int i = 1; i<longExpr.propositions.size(); ++i)
			shortExpr.propositions.push_back(longExpr.propositions[i]);
		return shortExpr;
	}
}



// Gets a vector to handle decomposition Truth Tree style
// [0] and [1] are the left (or only) branch, [2] and [3] are the right
std::vector<LogicExpr> LogicExpr::getVector() const {
	std::vector<LogicExpr> decomp(4, LogicExpr());
	if (isLiteral()) {
		return decomp;
	}
	else if (isNotNot()) {
		decomp[0] = propositions[0].propositions[0];
	}
	else if (isAnd()) {
		decomp[0] = propositions[0];
		decomp[1] = oneLessPlace(*this);
	}
	else if (isNotAnd()) {
		decomp[0].op = Operand::Not;
		decomp[0].propositions.push_back(propositions[0].propositions[0]);
		decomp[2].op = Operand::Not;
		decomp[2].propositions.push_back(oneLessPlace(propositions[0]));
	}
	else if (isOr()) {
		decomp[0] = propositions[0];
		decomp[2] = oneLessPlace(*this);
	}
	else if (isNotOr()) {
		decomp[0].op = Operand::Not;
		decomp[0].propositions.push_back(propositions[0].propositions[0]);
		decomp[1].op = Operand::Not;
		decomp[1].propositions.push_back(oneLessPlace(propositions[0]));
	}
	else if (isConditional()) {
		decomp[0].op = Operand::Not;
		decomp[0].propositions.push_back(propositions[0]);
		decomp[2] = propositions[1];
	}
	else if (isNotConditional()) {
		decomp[0] = propositions[0].propositions[0];
		decomp[1].op = Operand::Not;
		decomp[1].propositions.push_back(propositions[0].propositions[1]);
	}
	else if (isBiconditional()) {
		decomp[0] = propositions[0];
		decomp[1] = propositions[1];
		decomp[2].op = Operand::Not;
		decomp[2].propositions.push_back(propositions[0]);
		decomp[3].op = Operand::Not;
		decomp[3].propositions.push_back(propositions[1]);
	}
	else if (isNotBiconditional()) {
		decomp[0] = propositions[0].propositions[0];
		decomp[1].op = Operand::Not;
		decomp[1].propositions.push_back(propositions[0].propositions[1]);
		decomp[2].op = Operand::Not;
		decomp[2].propositions.push_back(propositions[0].propositions[0]);
		decomp[3] = propositions[0].propositions[1];
	}
	return decomp;
}



// Visualizes as a simple string
std::string LogicExpr::toString() const {
	std::string string_rep;
	std::string op_rep;
	if (op == Operand::Empty) {
		return "";
	}
	else if (op == Operand::None) {
		string_rep += "EMPTY/INVALID";
		return string_rep;
	}
	else if (op == Operand::Literal) {
		string_rep += literal;
		return string_rep;
	}
	else if (op == Operand::Not) {
		if ((propositions[0].op == Operand::Literal) || (propositions[0].op == Operand::Not)) {
			string_rep += "~" + propositions[0].toString();
		}
		else {
			string_rep += "~(" + propositions[0].toString() + ")";
		}
		return string_rep;
	}
	else if (op == Operand::Implication) {
		op_rep = " -> ";
		if (propositions.size() != 2) {
			string_rep += "INVALID";
			return string_rep;
		}
	}
	else if (op == Operand::Biconditional) {
		op_rep = " <-> ";
		if (propositions.size() != 2) {
			string_rep += "INVALID";
			return string_rep;
		}
	}
	else if (op == Operand::And) {
		op_rep = " & ";
	}
	else if (op == Operand::Or) {
		op_rep = " | ";
	}
	int i;
	for (i = 0; i<(((int)propositions.size()) - 1); ++i) {
		if ((propositions[i].op == Operand::Literal) || (propositions[i].op == Operand::Not)) {
			string_rep += propositions[i].toString() + op_rep;
		}
		else {
			string_rep += "(" + propositions[i].toString() + ")" + op_rep;
		}
	}
	if ((propositions[i].op == Operand::Literal) || (propositions[i].op == Operand::Not)) {
		string_rep += propositions[i].toString();
	}
	else {
		string_rep += "(" + propositions[i].toString() + ")";
	}
	return string_rep;
}



// Visualizes as a string with fancy html charecters for the operands
std::string LogicExpr::toString_html() const {
	std::string string_rep;
	std::string op_rep;
	if (op == Operand::Empty) {
		return "";
	}
	else if (op == Operand::None) {
		string_rep += "????";
		return string_rep;
	}
	else if (op == Operand::Literal) {
		if (literal == "OO") {
			string_rep += "<span style=\\\"color: #ff0000\\\"><b>&#9711;</b></span>";
		}
		else if (literal == "XX") {
			string_rep += "<span style=\\\"color: #01DF01\\\">&#10005;</span>";
		}
		else {
			string_rep += literal;
		}
		return string_rep;
	}
	else if (op == Operand::Not) {
		if ((propositions[0].op == Operand::Literal) || (propositions[0].op == Operand::Not)) {
			string_rep += "&#172;" + propositions[0].toString_html();
		}
		else {
			string_rep += "&#172;(" + propositions[0].toString_html() + ")";
		}
		return string_rep;
	}
	else if (op == Operand::Implication) {
		op_rep = " &#8594; ";
		if (propositions.size() != 2) {
			string_rep += "INVALID";
			return string_rep;
		}
	}
	else if (op == Operand::Biconditional) {
		op_rep = " &#8596; ";
		if (propositions.size() != 2) {
			string_rep += "INVALID";
			return string_rep;
		}
	}
	else if (op == Operand::And) {
		op_rep = " &#8743; ";
	}
	else if (op == Operand::Or) {
		op_rep = " &#8744; ";
	}
	int i;
	for (i = 0; i<(((int)propositions.size()) - 1); ++i) {
		if ((propositions[i].op == Operand::Literal) || (propositions[i].op == Operand::Not)) {
			string_rep += propositions[i].toString_html() + op_rep;
		}
		else {
			string_rep += "(" + propositions[i].toString_html() + ")" + op_rep;
		}
	}
	if ((propositions[i].op == Operand::Literal) || (propositions[i].op == Operand::Not)) {
		string_rep += propositions[i].toString_html();
	}
	else {
		string_rep += "(" + propositions[i].toString_html() + ")";
	}
	return string_rep;
}



std::ostream& operator<< (std::ostream& os, LogicExpr const &expr) {
	os << expr.toString();
	return os;
}

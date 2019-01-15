#ifndef logic_expr_h
#define logic_expr_h

#include <iostream>
#include <string>
#include <vector>
#include <cctype>


// A better way to store operands
namespace Operand
{
	enum Operand { Literal, And, Or, Not, Implication, Biconditional, None, Empty };
}


class LogicExpr {
private:
	std::vector<LogicExpr> propositions;
	std::string literal;
	Operand::Operand op;


	void set_from_string(const std::string& string_rep);

public:
	LogicExpr();

	LogicExpr(const std::string& string_rep);

	// Implement later, will need to be recursive:
	// LogicExpr(const LogicExpr& old_expr);   

	Operand::Operand get_operand() const { return op; };
	const std::vector<LogicExpr> get_propostions() const { return propositions; };

	bool isValid() const;

	// Quick checks for expression (main connective) types, (or negated types, as needed for truth trees)
	bool isLiteral() const { return ((op == Operand::Literal) || ((op == Operand::Not) && (propositions[0].op == Operand::Literal))); };
	bool isNotNot() const { return ((op == Operand::Not) && (propositions[0].op == Operand::Not)); };
	bool isAnd() const { return (op == Operand::And); };
	bool isNotAnd() const { return ((op == Operand::Not) && (propositions[0].op == Operand::And)); };
	bool isOr() const { return (op == Operand::Or); };
	bool isNotOr() const { return ((op == Operand::Not) && (propositions[0].op == Operand::Or)); };
	bool isConditional() const { return (op == Operand::Implication); };
	bool isNotConditional() const { return ((op == Operand::Not) && (propositions[0].op == Operand::Implication)); };
	bool isBiconditional() const { return (op == Operand::Biconditional); };
	bool isNotBiconditional() const { return ((op == Operand::Not) && (propositions[0].op == Operand::Biconditional)); };

	std::vector<LogicExpr> getVector() const;

	std::string toString() const;
	std::string toString_html() const;

	friend std::ostream& operator<<(std::ostream& os, const LogicExpr& expr);
	friend LogicExpr oneLessPlace(const LogicExpr& longExpr);
};


#endif
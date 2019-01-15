#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

#include "AutoSolver.h"
#include "LogicExpr.h"

#include <utility>
using namespace std;


//Needed a local version of this function as it needs to be called
//before a Tree object is made.
pair<LogicExpr, bool > pairToAddToStatements(LogicExpr sentence) {
	bool isLiteral = false; //Is a literal or negation of a literal
	isLiteral = sentence.isLiteral();
	pair<LogicExpr, bool > pairToBePushedBackInStatements(sentence, isLiteral);
	return pairToBePushedBackInStatements;
}


int main() {

	int numberOfPremises;
	vector<pair<LogicExpr, bool > > statements;
	string sentence;

	cout << "Enter the number of premises you have:" << endl;
	cin >> numberOfPremises;
	cout << "Enter your premises now:" << endl;

	for (int i = 0; i < numberOfPremises; i++) {
		cin >> sentence;
		LogicExpr sent(sentence);
		pair<LogicExpr, bool > insertOne = pairToAddToStatements(sent);
		statements.push_back(insertOne);
		sentence = "";
	}

	Tree solverTree(statements);
	solverTree.isValidTree();

	std::ofstream outstr("tree.js");
	solverTree.visualize(outstr);
	outstr.close();
	
	return 0;
}
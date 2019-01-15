#ifndef AutoSolver_h_
#define AutoSolver_h_
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <utility>
#include <algorithm>

#include "LogicExpr.h"

using namespace std;


// ==============================================================
// Node class. Each Node class represents a Branch. 
// ==============================================================

class Node {
public:
	//Constructor 
	Node() { left = NULL; right = NULL; parent = NULL; closed = false; open = false; }

	//Member variables

	//Each Node or Branch has a list of statements. We will store our statement as a LogicExpr object,
	//and if it has been composed or is a literal/negation of a literal.
	vector<pair<LogicExpr, bool > > statements;
	Node* left; //left branch
	Node* right; //right branch
	Node* parent; //branch above this branch
				  //Closed and open are for closing/opening the branch. Both start as 'false'.
	bool closed;
	bool open;
};

// ==============================================================
// Tree class. This is our automated validity checker.
// ==============================================================

class Tree {
public:
	//Constructor
	Tree(vector<pair<LogicExpr, bool > > statements_);

	//Copy constructor functions
	Node* copy_tree(Node* old_root); //Driver function for copy_tree constructor helper
	Node* copy_tree(Node* old_root, Node*& new_root); //Actual recursive function for copy tree constructor helper
	Tree(const Tree& old) { root_ = this->copy_tree(old.root_); } //Copy constructor

																  //Destructor functions
	void destroy_tree(Node* root_); //Destructor helper function
	~Tree() { this->destroy_tree(root_);  root_ = NULL; } //Destructor

														  //Assignment operator
	Tree& operator=(const Tree& old);

	//Functions to count leaves:
	int countLeavesDriver() { return countLeaves(root_); } //Driver function for counting leafs
	int countLeaves(Node* currentNode); //Counts leafs and checks if we can close/open leaf nodes
	int areThereAnyOpenBranches(const Node* currentNode) const; //Returns how many open branches there are after we solve the tree (>0 = invalid argument)

																//Helper functions for validity algorithm
	pair<LogicExpr, bool > pairToAddToStatements(const LogicExpr sentence) const; //Turns a string into a pair<LogicExpr, bool> so that we can push_back to statements in our nodes
	bool isThisTheOpposite(const string sentenceOne, const string sentenceTwo, const LogicExpr sentOne, const LogicExpr sentTwo) const; //Checks to see if literal A is the opposite of literal B
	bool seeIfWeCanCloseBranch(Node* currentNode);

	//Creating new nodes after a decomposition rule is applied
	void determine(const LogicExpr mainExpression, Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo, LogicExpr sentenceThree, LogicExpr sentenceFour);
	void decomposeAtLeafNode(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo);
	void decomposingSinglesWithChildren(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo);
	void decomposeNewBranchAtRootNode(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo, LogicExpr sentenceThree, LogicExpr sentenceFour);
	void decomposingMultiWithChildren(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo, LogicExpr sentenceThree, LogicExpr sentenceFour);

	//Actually solving
	void solver(Node* currentNode); //Driver function to solve the tree.
	void solve() { solver(root_); } //Solves the tree
	void isValidTree(); //Calls Solve and a little extra

						//Printing in CL and html
	void print_sideways(Node* currentNode, const string &indent) const;
	void visualize(std::ostream& os) const;

private:
	Node * root_;
	void visualize_core(Node* currentNode, std::ostream& os, int tabs) const;

};

#endif
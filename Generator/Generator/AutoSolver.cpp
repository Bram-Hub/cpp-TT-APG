#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <utility>
#include <algorithm>

#include "LogicExpr.h"
#include "AutoSolver.h"

using namespace std;


//==============================================================================
// Constructor, copy_constructor, destructor, assignment operator. 
//==============================================================================

//Constructor
Tree::Tree(vector<pair<LogicExpr, bool > > statements_) {
	root_ = new Node;
	for (unsigned int i = 0; i < statements_.size(); i++) {
		pair<LogicExpr, bool > insertOne = pairToAddToStatements(statements_[i].first);
		string temp = statements_[i].first.toString();
		root_->statements.push_back(insertOne);
	}
	//For the special scenario where the given premises can immediately close the root branch
	seeIfWeCanCloseBranch(root_);
}

//Driver function for copy_tree constructor helper
Node* Tree::copy_tree(Node* old_root) {
	Node* new_root = new Node;
	new_root->statements = old_root->statements;
	copy_tree(old_root, new_root);
	return new_root;
}

//Actual recursive function for copy tree constructor helper
Node* Tree::copy_tree(Node* old_root, Node*& new_root) {
	//If there's no where else to go, go back up the tree.
	if (old_root->left == NULL && old_root->right == NULL) {
		return new_root;
	}
	else {
		if (old_root->left != NULL) { //If there's a node to the left
			new_root->left = new Node;
			new_root->left->statements = old_root->left->statements;
			copy_tree(old_root->left, new_root->left);
		}
		if (old_root->right != NULL) { //If there's a node to the right
			new_root->right = new Node;
			new_root->right->statements = old_root->right->statements;
			copy_tree(old_root->right, new_root->right);
		}
		return NULL;
	}
}

//Destructor helper function
void Tree::destroy_tree(Node* root_) {
	if (root_ != NULL) {
		destroy_tree(root_->left);
		destroy_tree(root_->right);
		delete root_;
		root_ = NULL;
	}
}

//Assignment operator
Tree& Tree::operator=(const Tree& old) {
	if (&old != this) {
		this->destroy_tree(root_);
		root_ = this->copy_tree(old.root_);
	}
	return *this;
}


//==============================================================================
// Functions that involve leaf nodes
//==============================================================================

/* This function is called after we solve the tree entirely (everything has been decomposed).
* It does a final check on all of our leaf nodes to determine which branches are closed or open.
*/
int Tree::countLeaves(Node* currentNode) {
	bool canClose = false;

	if (currentNode == NULL) {
		return 0;
	}

	if (currentNode->left == NULL && currentNode->right == NULL) {
		canClose = seeIfWeCanCloseBranch(currentNode);
		if (canClose == false) {
			currentNode->open = true;
			LogicExpr openSymbol("OO"); //OO visually represents an open branch.
			pair<LogicExpr, bool> insertClosedOO(openSymbol, true);
			currentNode->statements.push_back(insertClosedOO);
		}
		else if (canClose == true) {
			currentNode->closed = true;
			LogicExpr closedSymbol("XX"); //XX visually represents a closed branch.
			pair<LogicExpr, bool> insertClosedXX(closedSymbol, true);
			currentNode->statements.push_back(insertClosedXX);
		}
		return 1;
	}
	else {
		int temp1 = countLeaves(currentNode->left);
		int temp2 = countLeaves(currentNode->right);
		return temp1 + temp2;
	}
}

//Returns how many branches are open in the tree after being solved. 
int Tree::areThereAnyOpenBranches(const Node* currentNode) const {
	if (currentNode == NULL) {
		return 0;
	}
	if (currentNode->left == NULL && currentNode->right == NULL && currentNode->open == true) {
		return 1;
	}
	else {
		return areThereAnyOpenBranches(currentNode->left) + areThereAnyOpenBranches(currentNode->right);
	}
}

//==============================================================================
//Helper functions for validity algorithm
//==============================================================================

/*
* This function handles the scenario where the newly decomposed statement is a literal or the negation of a literal.
* When this happens, we set isLiteral to true so that our algorithm that decomposes our tree knows that
* the statement cannot be decomposed any further.
* If the statement is NOT a literal, then isLiteral is left as false, and our algorithm knows that it must be handled at some point.
*/
pair<LogicExpr, bool > Tree::pairToAddToStatements(const LogicExpr sentence) const {
	bool isLiteral = false; //Is a literal or negation of a literal
	isLiteral = sentence.isLiteral();
	pair<LogicExpr, bool > pairToBePushedBackInStatements(sentence, isLiteral);
	return pairToBePushedBackInStatements; //This is what will be inserted to the nodes Statements vector.
}

/*
* This function checks to see if statement A and statement B are literals, and if they are, returns true if
* literal A is the negation of literal B (ex, A and ~A), or the other way around (~A and A).
*/
bool Tree::isThisTheOpposite(const string sentenceOne, const string sentenceTwo, const LogicExpr sentOne, const LogicExpr sentTwo) const {
	char sentenceOneAsAChar, sentenceTwoAsAChar;

	//We can immediately exit if either sentenceOne or sentenceTwo is >2 or =0.
	if (sentenceOne.size() > 2 || sentenceTwo.size() > 2 || sentenceOne.size() == 0 || sentenceTwo.size() == 0) {
		return false;
	}
	//Can also exit if X = X (where X is any alphabet letter).
	if (sentenceOne == sentenceTwo) {
		return false;
	}

	//Case one: sentenceOne is X and sentenceTwo is ~X:
	if ((sentenceOne.size() == 1) && (sentenceTwo.size() == 2) && (sentOne.isLiteral()) && (sentTwo.isLiteral())) {
		sentenceOneAsAChar = sentenceOne[0];
		sentenceTwoAsAChar = sentenceTwo[1];
		if (sentenceOneAsAChar == sentenceTwoAsAChar) {
			return true;
		}
		//Case two: sentenceOne is ~X and sentenceTwo is X:
	}
	else if ((sentenceOne.size() == 2) && (sentenceTwo.size() == 1) && (sentOne.isLiteral()) && (sentTwo.isLiteral())) {
		sentenceOneAsAChar = sentenceOne[1];
		sentenceTwoAsAChar = sentenceTwo[0];
		if (sentenceOneAsAChar == sentenceTwoAsAChar) {
			return true;
		}
	}
	else {
		return false;	//Cannot close
	}
	return false; //Cannot close
}

/*
* This function takes a leaf node, and climbs up its parent nodes to see if it can close the branch by checking against all parents
* for an opposite literal (ex. A and ~A).
* This function is called whenever a new leaf node is added to the tree or a newly decomposed statement is added to a leaf node.
*/
bool Tree::seeIfWeCanCloseBranch(Node* currentNode) {
	bool canWeClose = false;
	Node* tempNode = currentNode;
	LogicExpr currentStatement;
	string curStatement = "";
	LogicExpr comparedAgainstStatement;
	string comparedAgnstStment;

	//Check against itself, before attempting to find an opposite in a parent node.
	for (unsigned int i = 0; i < currentNode->statements.size(); i++) {
		for (unsigned int j = 0; j < currentNode->statements.size(); j++) {
			if (j != i) {
				currentStatement = currentNode->statements[i].first;
				curStatement = currentStatement.toString();
				comparedAgainstStatement = currentNode->statements[j].first;
				comparedAgnstStment = comparedAgainstStatement.toString();
				canWeClose = isThisTheOpposite(curStatement, comparedAgnstStment, currentStatement, comparedAgainstStatement);
				if (canWeClose == true) {
					//If we've reached here, then our currentNode can close.
					currentNode->closed = true;
					break;
				}
			}
		}
		//Just exiting our loop.
		if (canWeClose == true) { break; }
	}

	//Check against parent nodes.
	if (canWeClose == false) {
		for (unsigned int i = 0; i < currentNode->statements.size(); i++) {
			currentStatement = currentNode->statements[i].first;
			curStatement = currentStatement.toString();
			while (tempNode->parent != NULL) {
				//Compare currentStatement against all of the statements in the next branch up.
				for (unsigned int j = 0; j < tempNode->parent->statements.size(); j++) {
					comparedAgainstStatement = tempNode->parent->statements[j].first;
					comparedAgnstStment = comparedAgainstStatement.toString();
					canWeClose = isThisTheOpposite(curStatement, comparedAgnstStment, currentStatement, comparedAgainstStatement);
					if (canWeClose == true) {
						//If we've reached here, then our currentNode can close.
						currentNode->closed = true;
						break;
					}
				}
				if (canWeClose == true) { break; }
				tempNode = tempNode->parent; //Climb up the tree one branch at a time.
			}
			if (canWeClose == true) { break; }
			//We will now be checking the next statement in currentNode against all parents. 
			tempNode = currentNode;
		}
	}
	return canWeClose;
}


//==============================================================================
//How to handle decompositions:
//==============================================================================
/*
* Decomposing a statement:
* 1. Decompose statement inside current node, when at a leaf node, if possible
*	 Possible when:
*		* current node is not closed
*       * At a leaf node, otherwise call (3) on this list
*		* isNotNot, isAnd, isNotConditional, isNotOr are true
* 2. Decompose statement into two children, when at a leaf node, if possible
*		* current node is not closed
*  	    * At a leaf node, otherwise call (4) on this list
*		* Possible when: isNotAnd,  isOr, isConditional, isBiconditional, isNotBiconditional
* 3. Decomposing isNotNot, isAnd, isNotConditional, isNotOr are true, when there are children:
*		* Can only add to leaf nodes that are not closed
*		* Note - No new nodes will be created
* 4. Decomposing isNotAnd, isOr, isConditional, isBiconditional, isNotBiconditional when there are children:
*		* Can only add children to the leaf nodes that are not closed
*/

/* This function determines if either (3) or (4) on the list above will be called, based off the main connective of the statement that we are decomposing.
* Note that 3 and 4 ultimately call 1 and 2, respectively.
* sentenceOne,Two,Three,Four represent the newly decomposed statements from decomposing our original statement.
* Some decomposition rules use all four sentences, some don't. For those that don't, the LogicExpr will convert to an empty string ("") when toString() is called.
*/
void Tree::determine(const LogicExpr mainExpression, Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo, LogicExpr sentenceThree, LogicExpr sentenceFour) {
	if (mainExpression.isNotNot() || mainExpression.isAnd() || mainExpression.isNotConditional() || mainExpression.isNotOr()) {
		decomposingSinglesWithChildren(currentNode, sentenceOne, sentenceTwo);
	}
	else if (mainExpression.isNotAnd() || mainExpression.isOr() || mainExpression.isConditional() || mainExpression.isBiconditional() || mainExpression.isNotBiconditional()) {
		decomposingMultiWithChildren(currentNode, sentenceOne, sentenceTwo, sentenceThree, sentenceFour);
	}
}

/*
* This function represents (1) on our 'Decomposing a statement' list.
* sentenceOne is the first newly decomposed line, and sentenceTwo is the second newly decomposed line.
*/
void Tree::decomposeAtLeafNode(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo) {
	//If we're at a leaf node, and the leaf node isn't closed
	if ((currentNode->left == NULL) && (currentNode->right == NULL) & (currentNode->closed == false)) {
		//Check to see if our newly decomposed statement is in a literal or negation, and add it to the list of statements on the branch.
		pair<LogicExpr, bool > insertOne = pairToAddToStatements(sentenceOne);
		currentNode->statements.push_back(insertOne);
		//If there is a sentence two
		string sentTwo = sentenceTwo.toString();
		if (sentTwo != "") {
			pair<LogicExpr, bool > insertTwo = pairToAddToStatements(sentenceTwo);
			currentNode->statements.push_back(insertTwo);
		}
		//We check to see if we can close our leaf node after new statements have been successfully added to the node.
		seeIfWeCanCloseBranch(currentNode);
	}
}

/*
* This function represents (3) on our 'Decomposing a statement' list.
* Decomposing isNotNot, isAnd, isConditionalNot, isOrNot are true, when there are children:
*		* Can only add to leaf nodes that are not closed
*		* Note - No new nodes will be created
*		* Note - this function uses (1)
*/
void Tree::decomposingSinglesWithChildren(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo) {
	if (currentNode->left != NULL) { //If there's a node to the left
		decomposingSinglesWithChildren(currentNode->left, sentenceOne, sentenceTwo);
	}
	if (currentNode->right != NULL) { //If there's a node to the right
		decomposingSinglesWithChildren(currentNode->right, sentenceOne, sentenceTwo);
	}
	//See if we're at a leaf, node, is so, add our decomposed sentences into our leaf node
	decomposeAtLeafNode(currentNode, sentenceOne, sentenceTwo);
}

/*
* This function represents (2) on our 'Decomposing a statement' list.
* SentenceOne belongs to the left child, as does sentenceTwo when applicable (ex. biconditional).
* sentenceThree and sentenceFour belong to the right child.
*/
void Tree::decomposeNewBranchAtRootNode(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo, LogicExpr sentenceThree, LogicExpr sentenceFour) {
	//If we're at a leaf node and the branch isn't closed
	if ((currentNode->left == NULL) && (currentNode->right == NULL) && (currentNode->closed == false)) {
		//Creating the new left child
		Node* leftNode = new Node;
		leftNode->parent = currentNode;

		//Creating the new right child
		Node* rightNode = new Node;
		rightNode->parent = currentNode;

		//Creating the link between the current node and its new children
		currentNode->left = leftNode;
		currentNode->right = rightNode;

		//Adding the newly decomposed sentences our left child node
		pair<LogicExpr, bool > insertOne = pairToAddToStatements(sentenceOne);
		leftNode->statements.push_back(insertOne);

		//If there's a second decomposed statement, which happens in the biconditional decompositions
		string sentTwo = sentenceTwo.toString();
		if (sentTwo != "") {
			pair<LogicExpr, bool > insertTwo = pairToAddToStatements(sentenceTwo);
			leftNode->statements.push_back(insertTwo);
		}

		//Adding the newly decomposed sentences to our right child node
		pair<LogicExpr, bool > insertThree = pairToAddToStatements(sentenceThree);
		rightNode->statements.push_back(insertThree);

		//If there's a second decomposed statement, which happens in the biconditional decompositions
		string sentFour = sentenceFour.toString();
		if (sentFour != "") {
			pair<LogicExpr, bool > insertFour = pairToAddToStatements(sentenceFour);
			rightNode->statements.push_back(insertFour);
		}

		//We check to see if we can close our new leaf nodes after new statements have been successfully added.
		seeIfWeCanCloseBranch(leftNode);
		seeIfWeCanCloseBranch(rightNode);
	}
}


/*
*	4. Decomposing isAndNot, isOr, isConditional, isBiconditional, isBiconditionalNot, when there are children:
*	*		* Can only add children to the leaf nodes that are not closed
*	*		* Will need drawn algorithm to find the leaf nodes, and add TWO CHILDREN to all leaf nodes, and to the NEWLY added
*	*		  children, place the newly decomposed statements into their vectors
*	*		* New leaf nodes WILL be created
*	*		* Note - this function builds on (2)
*/
void Tree::decomposingMultiWithChildren(Node* currentNode, LogicExpr sentenceOne, LogicExpr sentenceTwo, LogicExpr sentenceThree, LogicExpr sentenceFour) {
	if (currentNode->left != NULL) { //If there's a node to the left
		decomposingMultiWithChildren(currentNode->left, sentenceOne, sentenceTwo, sentenceThree, sentenceFour);
	}
	if (currentNode->right != NULL) { //If there's a node to the right
		decomposingMultiWithChildren(currentNode->right, sentenceOne, sentenceTwo, sentenceThree, sentenceFour);
	}
	decomposeNewBranchAtRootNode(currentNode, sentenceOne, sentenceTwo, sentenceThree, sentenceFour);
}

//==============================================================================
// Solving algorithms
//==============================================================================

/*
* This function performs a depth-first search, along the way decomposing everything that hasn't been decomposed already,
* and constantly checking to see if we can close a branch.
*/
void Tree::solver(Node* currentNode) {
	//We start at the root premises. We will decompose any statement that's not a literal or the
	//negation of a literal.
	for (unsigned int i = 0; i < currentNode->statements.size(); i++) {
		//If our current premise is not a literal or negation of a literal (false) , we have work to do
		if (currentNode->statements[i].second == false) {
			LogicExpr mainExpression = currentNode->statements[i].first;
			vector<LogicExpr> decomposedSentences = mainExpression.getVector();
			determine(mainExpression, currentNode, decomposedSentences[0], decomposedSentences[1], decomposedSentences[2], decomposedSentences[3]);
			//By now we have decomposed this sentence
			currentNode->statements[i].second = true;
		}
	}
	if (currentNode->left != NULL) {
		solver(currentNode->left);
	}
	if (currentNode->right != NULL) {
		solver(currentNode->right);
	}
}

/*
* This function calls our solve function, does a final close/open determination on all leaf nodes, checks if any branches are open
* (invalid argument), and prints the tree in the CL.
*/
void Tree::isValidTree() {
	int numberOfOpenBranches = 0;

	solve();
	countLeavesDriver();

	numberOfOpenBranches = areThereAnyOpenBranches(root_);
	if (numberOfOpenBranches > 0) {
		cout << "Invalid argument." << endl;
	}
	else {
		cout << "Valid argument." << endl;
	}

	print_sideways(root_, "         ");
}


//==============================================================================
// Printing functions
//==============================================================================
void Tree::print_sideways(Node* currentNode, const string &indent) const {
	if (currentNode == NULL) return;
	// print out this node
	cout << endl;
	cout << "-------------- New Branch ---------------" << endl;

	for (unsigned int i = 0; i < currentNode->statements.size(); i++) {
		cout << indent << currentNode->statements[i].first << " (" << currentNode->statements[i].second << ")" << endl;
	}

	// recurse on each of the children trees
	// increasing the indentation
	print_sideways(currentNode->left, indent + "                   ");
	print_sideways(currentNode->right, indent + "                   ");
}

void Tree::visualize(std::ostream& os) const {
	if (root_ == NULL) return;
	os << "var simple_chart_config = {\n\tchart: {\n\t\tcontainer: \"#OrganiseChart-simple\"\n\t},\n\n\tnodeStructure: {\n";
	visualize_core(root_, os, 2);
	os << "}\n};";
}

void Tree::visualize_core(Node* currentNode, std::ostream& os, int tabs) const {
	if (currentNode == NULL) return;
	os << "innerHTML: \"<div style=\\\"text-align:center\\\"><p>";
	for (unsigned int i = 0; i < currentNode->statements.size() - 1; i++) {
		os << currentNode->statements[i].first.toString_html() << "<br>";
	}
	os << currentNode->statements[currentNode->statements.size() - 1].first.toString_html() << "</p></div>\"";

	if (currentNode->left != NULL) {
		os << ",\n" << "children: [\n" << "{\n";
		visualize_core(currentNode->left, os, tabs + 1);
		if (currentNode->right != NULL) {
			os << "},\n{\n";
			visualize_core(currentNode->right, os, tabs + 1);
		}
		os << "}\n]\n";
	}
	else {
		os << "\n";
	}
}
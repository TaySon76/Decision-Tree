#include "decision-tree.h"

using namespace tree;

struct tree::treeNode
{
    Label label;				//Node label (DecisionTree variable)
    treeNode *firstChild;		//Pointer to the first child
    treeNode *nextSibling;		//Pointer to the next sibling
    TextValue value;			//Value of the variable (condition on the arc)
};

/*******************************************************************************************************/
// Function createEmptyTree: returns the empty tree
Tree tree::createEmpty()
{
    return emptyTree;
}

/*******************************************************************************************************/
// Query function: returns true if the tree is empty
bool tree::isEmpty(const Tree &t)
{
    return (t == emptyTree);
}

/*******************************************************************************************************/
// AUXILIARY FUNCTION: given a label l, it creates the node with that label and returns its pointer
Tree createNode(const Label l)
{
    Tree t = new treeNode;
    t->label = l;
    t->firstChild = t->nextSibling = emptyTree;
    t->value = noTextValue;
    return t;
}

// AUXILIARY FUNCTION: given a label l and the value tt, it creates the node with that label and that value and returns its pointer
Tree createNodeWithValue(const Label l, TextValue tt)
{
	Tree t = new treeNode;
	t->label = l;
	t->firstChild = t->nextSibling = emptyTree;
	t->value = tt;
	return t;
}

// Auxiliary function used to derive the name of the node from the label (removing for example the 'Risk' from Risk_1)
Label trueLabel(Label l) {
	long unsigned int i = 0;			//the long unsigned is used to compare with the length of the Label string without warning
	for (; i < l.length() && l[i] != '_'; i++);
	return l.substr(0, i);
}

// Auxiliary function used to derive the value of the condition (removing for example the 23 from '<=23' or the Sportiva from '=Sportiva')
string cleanText(string str) {
	int i = 0;
	while (str[i] == '=' || str[i] == '!' || str[i] == '<' || str[i] == '>')
		i++;
	return str.substr(i, string::npos);
}

// Function that returns true if a character is a valid character to identify the variables
bool isCondition(char c, char c2) {
	return (c == '=' || (c == '!' && c2 == '=') || c == '<' || c == '>');
}

// Function that checks for alphabetic characters in the string
bool hasAlphabets(char* arr) {
	for (int i = 0; arr[i] != 0; i++)
	{
		if (arr[i] >= 'A' && arr[i] <= 'z')
			return true;
	}
	return false;
}

// Auxiliary function that returns the number of siblings in a node (counting itself)
int siblingCount(Tree tree) {
	int i = 0;
	for (; tree != emptyTree; i++)
		tree = tree->nextSibling;
	return i;
}

// Auxiliary function that randomly chooses which arc (branch of the tree) to traverse
// in case you have more than one true condition on the arcs
void condition(int branchChosen, int branchItr, int sibCount) {
	branchChosen = rand() % sibCount;
	branchItr = -1;
}

//***************AUXILIARY FUNCTIONS TO THOSE THAT MODIFY THE DECISION TREE***************//
// AUXILIARY FOR THE ADDELEM, DELETEELEM AND MODIFYNODE
// Given a label l and a tree t (that is, a tree whose root is the node pointed to by t), returns the
// pointer to the node of the tree t that has that label (emptyTree if no node has that label)
Tree tree::getNode(const Label l, const Tree t)
{
    if (isEmpty(t) || l == emptyLabel)
        return emptyTree;
    if (t->label == l)
        return t;
    Tree auxT = t->firstChild;
    Tree resNode;
    while (auxT != emptyTree)
    {
        resNode = getNode(l, auxT);
        if (resNode == emptyTree)
            auxT = auxT->nextSibling;
        else
            return resNode;
    }
    return emptyTree;
}

/*******************************************************************************************************/
 // AUXILIARY FOR THE DELETE// Auxiliary function: given a pointer to a node it returns the pointer to its last child that is
 // to the one farthest to the right in the chain of SIBLINGS - if it exists.
Tree lastChild(const Tree &t)
{
    if (isEmpty(t) || isEmpty(t->firstChild))
        return emptyTree;

    Tree prevChild = t->firstChild;
    Tree currentChild = prevChild->nextSibling;
    while (!isEmpty(currentChild))
    {
        prevChild = currentChild;
        currentChild = currentChild->nextSibling;
    }
    return prevChild;
}

/*******************************************************************************************************/
// AUXILIARY FOR THE DELETE
// Auxiliary function: given a tree t and a label l, it returns the pointer to the previous sibling
// (more to the left) than the one with the given label, if it exists in the tree
// it is used in the deleteElem
Tree prevSibling(const Label l, const Tree &t)
{
    if (isEmpty(t) || isEmpty(t->firstChild) || l == emptyLabel)
        return emptyTree;

    if ((t->firstChild)->label == l)
        return emptyTree;
    else
    {
        Tree prevChild = t->firstChild;
        Tree currentChild = prevChild->nextSibling;
        Tree auxT;
        while (!isEmpty(currentChild) && currentChild->label != l)
        {
            prevChild = currentChild;
            currentChild = currentChild->nextSibling;
        }
        if (!isEmpty(currentChild) && currentChild->label == l)
            return prevChild;
        else
        {
            currentChild = t->firstChild;
            while (!isEmpty(currentChild))
            {
                auxT = prevSibling(l, currentChild);
                if (!isEmpty(auxT))
                    return auxT;
                else
                    currentChild = currentChild->nextSibling;
            }
        }
    }
    return emptyTree;
}


/*******************************************************************************************************/
// AUXILIARY FOR THE DELETE
// auxiliary function of father: returns true if the node pointed to by t has a child with the label l
bool hasChildWithLabel(const Label l, const Tree &t)
{
    if (isEmpty(t))
        return false;

    Tree child = t->firstChild;
    while (!isEmpty(child))
    {
        if (child->label == l)
            return true;
        else
            child = child->nextSibling;
    }
    return false;
}
/*******************************************************************************************************/
// AUXILIARY FOR THE DELETE
// Auxiliary function that returns the label of the parent of the node labeled l if the node exists in the tree
// (or subtree) t and if the father exists, and returns the constant empty Label otherwise.
Label tree::father(const Label l, const Tree &t)
{
    if (isEmpty(t))
        return emptyLabel;

    if (hasChildWithLabel(l, t))
        return t->label;

    Tree child = t->firstChild;
    Label auxL;
    while (!isEmpty(child))
    {
        auxL = father(l, child);
        if (auxL != emptyLabel)
            return auxL;
        else
            child = child->nextSibling;
    }
    return emptyLabel;
}

/*******************************************************************************************************/
// AUXILIARY FOR THE DELETE
// Auxiliary function that returns a list of Labels, containing the labels
// of all children in the tree t of the node labeled with l
listt::List tree::children(const Label l, const Tree &t)
{
    Tree auxT = getNode(l, t);
    listt::List lst = listt::createEmptyList();

    if (!isEmpty(auxT))
    {
        Tree child = auxT->firstChild;
        while (!isEmpty(child))
        {
            listt::addBack(child->label, lst);
            child = child->nextSibling;
        }
    }
    return lst;
}

/*******************************************************************************************************/
// AUXILIARY FOR THE DELETE
// Auxiliary function that returns the number of arcs outgoing from the node labeled with l; returns -1 if it does not exist
// no label l in the tree
int tree::degree(const Label l, const Tree &t)
{
    if (!member(l, t))
        return -1;
    else
        return listt::size(children(l, t)); 
}

/*******************************************************************************************************/
// AUXILIARY FOR THE DELETE
// Auxiliary function that returns true if the node labeled with the label belongs to the tree and false otherwise
bool tree::member(const Label l, const Tree &t)
{
	// if the tree is empty, the label is missing and it returns false
    if (isEmpty(t))
        return false;
    // if the tree label is the one sought, I return true
    if (t->label == l)
        return true;
	// Recursive call of member on each of the children of t, until one of the calls returns a value other than false
    Tree auxT = t->firstChild;
    while (auxT != emptyTree)
    {
        if (!member(l, auxT)) // haven't found it searching in this subtree, I have to continue scanning the brothers
            auxT = auxT->nextSibling;
        else // found it, return true
            return true;
    }
    return false; // if I get here, it means that at the end of an exhaustive search in the tree the node was not found
}

//***************OPERATIONS THAT CHANGE THE DECISION TREE***************//
// The function addElem inserts a node identified by the label labelOfNodeToAdd by attaching it to a node
// already existing in the tree labelOfNodeInTree. It fails if the labelOfNodeInTree label node is not
// present in the tree, if labelOfNodeToAdd is already present in the tree, if you try to insert the node by attaching it to labelOfNodeInTree if it is a
// leaf (which corresponds to the final end in the prediction) or if you try to insert the node by attacking it
// a labelOfNodeInTree if it is the father of a leaf (it would not allow to produce the prediction)
Error tree::addElem(const Label labelOfNodeInTree, const Label labelOfNodeToAdd, TextValue value, Tree &t)
{
	if ((labelOfNodeInTree == emptyLabel) && isEmpty(t)) // labelOfNodeInTree is the empty label and the tree is empty
    	{
      	t = createNodeWithValue(labelOfNodeToAdd, value); // I create the node; t which was empty becomes the tree with the only node just created
      	return OK;	
    	}
	//If the value of the variable (the condition) is not valid, it fails
	if (!isCondition(value[0], value[1])) {	
		cout << "The condition entered is not valid" << endl;
		return FAIL;
	}
	//If a node already exists in the tree with label labelOfNodeToAdd, it fails
    	if (member(labelOfNodeToAdd, t))
      	return FAIL;

    	Tree auxT = getNode(labelOfNodeInTree, t);
	//If you try to add labelOfNodeToAdd to a leaf, it fails
	if (auxT->value[1] == 'A' || auxT->value[1] == 'B') {
		cout << "It is not allowed to add knots to a leaf" << endl;
		return FAIL;
	}
	//If you try to add labelOfNodeToAdd to the father of a leaf, it fails
	if (auxT->firstChild != emptyTree) {
		if (auxT->firstChild->value[1] == 'A' || auxT->firstChild->value[1] == 'B')
		{
			cout << "It is not allowed to add nodes to the father of a leaf" << endl;
			return FAIL;
		}
	}
	//If there is no node in the tree with label labelOfNodeInTree, it fails
	if (auxT == emptyTree) {
		cout << "There is no node with this label in the tree" << endl;
		return FAIL;
	}
    	else
    	{
       	Tree child = createNodeWithValue(labelOfNodeToAdd, value); // otherwise I create a child node with the label labelOfNodeToAdd
        	child->nextSibling = auxT->firstChild;			       // the first child's brother will be the one who was the first child of auxT
        	auxT->firstChild = child;                              // child becomes the first child of auxT
    	}
    	return OK;
}

/*******************************************************************************************************/
// The function deleteElemI searches for and deletes the node identified by the label l and the entire subtree
// identified by it. It fails if the node identified by the label l is not present in the tree or
// if you try to delete a leaf (it would not be possible to do the prediction)
Error tree::deleteElemI(const Label l, Tree &t)
{
	if (!member(l, t)) {
		cout << "There is no node with this label in the tree" << endl;
		return FAIL; // in the tree there is no node with label l, returns FAIL
	}
    	Tree fatherTree = getNode(father(l, t), t); // I look for the pointer to the father of the node to be removed
    	if (isEmpty(fatherTree))                    // if fatherTree is empty, it means that I am trying to delete the root
    	{
      	if (degree(l, t) == 0) // I can remove the root only if it has no children, in which case
        	{
           		delete t;      // we dealloc
            	t = emptyTree; // and t becomes the empty tree
        	}
        	else
            	return FAIL; // otherwise I cannot remove it and return FAIL
    	}
    	else // I'm trying to remove an internal node, of which I found the father fatherTree
    	{
      	Tree nodeToRemove = getNode(l, t);              // retrieve the pointer to the node to be removed
		if (nodeToRemove->firstChild == emptyTree) {
			cout << "It is not allowed to delete a leaf" << endl;
			return FAIL;
		}
		Tree lastCh = lastChild(fatherTree);            // retrieve the pointer to the node of the last sibling of the node to be removed (it could be the node itself, if it is the last one)
		lastCh->nextSibling = emptyTree; // the children of the knot to be removed become his brothers
		Tree prevSibl = prevSibling(l, t);              // retrieve the pointer to the previous sibling compared to the node to be removed
		if (isEmpty(prevSibl))                          // if there is no previous sibling, the node to be removed is the first one: I have to change the pointer to the firstChild in the father
			fatherTree->firstChild = (fatherTree->firstChild)->nextSibling;
		else // otherwise, I have to "jump" the knot to be removed in the chain of brothers
		      prevSibl->nextSibling = emptyTree;
		delete nodeToRemove; // in any case, in the end dealloco the node to be removed
	}
    return OK;
}

/*******************************************************************************************************/
// The modifyNode function modifies the node identified by the label labelToModify with the new label
// newLabel and the new condition on the arc incident to the newValue node. In the event that the variable
// on the original node is of a different type than the one entered, the function asks to insert the
// conditions on all children of the modified node. It fails if there is no node in the identified tree
// from the label labelToModify, the variable entered as a condition is not valid and if you try to edit a leaf.
Error tree::modifyNode(Label labelToModify, TextValue newLabel, TextValue newValue, Tree &tree) {
	Tree auxT = getNode(labelToModify, tree); //Find the node to modify
	//It is not allowed to modify the leaves (End termination nodes) - it fails
	if (auxT->firstChild == emptyTree) {
		cout << "It is not allowed to modify a leaf" << endl;
		return FAIL;
	}
	//If there is no node in the tree identified by the inserted label - it fails
	if (auxT == emptyTree) {
		cout << "There is no node with this label in the tree" << endl;
		return FAIL;
	}
	//If the entered variable is not valid - it fails
	if (!isCondition(newValue[0], newValue[1])) {
		cout << "The variable entered is not valid" << endl;
		return FAIL;
	}
	
	//Maintained value of the variable from user input (e.g. '99' if user entered '!=99')
	string trimmedLabel = cleanText(newValue);
	//Maintained value of the variable from the original node (e.g. '99' if the original node had '!=99' as a variable)
	string trimmedTreeLabel = cleanText(auxT->value);

	// If both variables are integers (quantitative)
	if(atoi(trimmedLabel.c_str()) > 0 && atoi(trimmedTreeLabel.c_str()) > 0){
		if(newLabel != emptyLabel)
			auxT->label = newLabel;
		
		auxT->value = newValue;
	}
	// If both variables are strings (categorical)
	else if (atoi(trimmedLabel.c_str()) == 0 && atoi(trimmedTreeLabel.c_str()) == 0) {
		if(newLabel != emptyLabel)
			auxT->label = newLabel;
		
		auxT->value = newValue;
	}
	// If the variable on the original node is of a different type than the one entered:
	else {
		if(newLabel != emptyLabel)
			auxT->label = newLabel;
		auxT->value = newValue;
		auxT = auxT->firstChild;
		// If he has no children, I do nothing if he has them
		if(auxT->firstChild != emptyTree)
			cout << "The original condition and the inserted one are of different types, enter the conditions on his children" << endl;
		int counter = 1;
		// Scroll through all the children of the modified node asking for each the possible new condition (if the user does not want to change it, just enter the previous condition, shown alongside)
		while (auxT != emptyTree) {
			string newVal;
			if (auxT->firstChild != emptyTree) {
				cout << endl << "Figlio numero " << counter << endl;
				cout << "Enter the new condition of the node " << auxT->label << " whose old condition was: "
					<< auxT->value << endl;
				cin >> newVal;
				// If the variable entered is not valid (it is not preceded by the symbols that characterize the variables, check by calling the isCondition)
				if (!isCondition(newVal[0], newVal[1]))
				{
					cout << "The variable entered is not valid" << endl;
					continue;
				}
				auxT->value = newVal;
				counter++;			//Increase the counter (number of children)
			}
			auxT = auxT->nextSibling;	//move to the next sibling among the children of the modified node
		}	
	}
	return OK;
}

/*********************************PREDICTION FUNCTIONS**********************************************************************/
// The predictionFromInputAtBeginning function carries out the prediction by taking pairs as input
// (value variable), where the variable and the value are separated by a space and the pairs are separated by ';'.
// Once all the values have been entered the function will instantiate the conditions on the arcs in order to carry out the prediction:
// in the case where there are more true conditions on the arcs, it chooses one randomly, in case there is none, print an error message and return NOTHING.
End tree::predictionFromInputAtBeginning(const string filename, Tree &tree){
	char args[10][100] = {};
	
	unsigned int argcount = 0;
	unsigned int i = 0;			//is used to compare with the length of the Label string
	int d = 0;

	//divide the arguments according to ';'
	for (; i < filename.length(); i++) {
		if (filename[i] != ';')
			args[argcount][d] = filename[i];
		else {
			args[argcount++][d] = 0;
			d = -1;
		}
		d++;
	}
	args[argcount++][d] = 0;
	string fArgs[10];
	//string fArgsPre[10];
	//Retrieve the actual value of these arguments (such as '49' from (Age 49) based on space '')
	for (i = 0; i < argcount; i++) {
		if (i > 0)
		{
			int j = 1;
			for (; args[i][j] != 0; j++)
				args[i][j - 1] = args[i][j];
			args[i][j - 1] = 0;
		}
		
		int f = 0;

		for (; args[i][f] != ' '; f++);
		string tempo(args[i]);
		fArgs[i] =  tempo.substr(f + 1, string::npos);
		//fArgsPre[i] = tempo.substr(1, f - 1); 
		// pop_back the string to remove the last parenthesis (the ') after 49)
		fArgs[i].pop_back();
	}
	//call srand (time(NULL)) to make sure that the values are actually random with each call
	srand(time(NULL));
	while (tree != emptyTree) {

		argcount--;
		//if the child is a leaf we can return the prediction
		if ((tree->firstChild->value[1] == 'A' || tree->firstChild->value[1] == 'B') && tree->firstChild->value[2] == 0) {
			if (tree->firstChild->value[1] == 'A')
				return HIGH;
			else
				return LOW;
		}

		int branchItr = 0, branchChosen = 0, sibCount = 0;
		bool found = false;
		// If the tag of the variable entered does not match that of the node in the tre
		// if (fArgsPre[argcount].compare(trueLabel(tree->label))! = 0)
		// {
		// 		cout << "Invalid label entry, '" << fArgsPre[argcount] << "' does not match the label '" << trueLabel (tree-> label) << "'" << endl;
		// 		return NOTHING;
		//}
		// Print the label of the first variable and its entered value
		cout << trueLabel(tree->label) << ": " << fArgs[argcount] << endl;
		
		int num = atoi(fArgs[argcount].c_str());

		Tree tree2 = tree->firstChild;
		Tree tree3 = tree2;

		sibCount = siblingCount(tree3); //store in the variable sibCount the number of siblings (tree3 included) of node tree3

		//The cycle trasverses all his children, sibling by sibling
		while (tree2 != emptyTree) {
		// Case 1: the user has entered an integer (quantitative variable)
			if (num != 0) {
				Tree fChild = tree2;
				int childVal = atoi(cleanText(fChild->value).c_str());

				char ibc[50];
				strcpy(ibc, fArgs[argcount].c_str());
				//The entered quantitative value contains letters: the variable is not valid
				if (hasAlphabets(ibc)) {		
					cout << "Invalid value" << endl;
					return NOTHING;
				}
				//If a number is entered and the value of the node is a string
				if (childVal == 0) {
					cout << "Invalid value" << endl;
					return NOTHING;
				}
				if (fChild->value[0] == '<') {
					if (fChild->value[1] == '=') {
						if (num <= childVal) {
						// If no branch has been found yet mark it as found and store its index
							if (!found) {
								branchChosen = branchItr;
								found = true;
							} // If the branch has already been found and there is another true condition (the conditions are not mutually exclusive), I call
							  // the function condition that randomly chooses the branch to go and I exit the loop
							else {	
								condition(branchChosen, branchItr, sibCount);
							break;
							}
						}
					}		
					else {
						if (num < childVal) {
							if (!found) {
								branchChosen = branchItr;
								found = true;
							}
							else {	
								condition(branchChosen, branchItr, sibCount);
								break;
							}
						}
					}
				}
				else if (fChild->value[0] == '>') {
					if (fChild->value[1] == '=') {
						if (num >= childVal) {
							if (!found) {
								branchChosen = branchItr;
								found = true;
							}
							else {	
								condition(branchChosen, branchItr, sibCount);
								break;
							}
						}
					}
					else {
						if (num > childVal) {
							if (!found) {
								branchChosen = branchItr;
								found = true;
							}
							else {	
								condition(branchChosen, branchItr, sibCount);
								break;
							}
						}
					}
				}
				else if (fChild->value[0] == '=') {
					if (num == childVal) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {	
							condition(branchChosen, branchItr, sibCount);
							break;
						}
					}
				}
				else if (fChild->value[0] == '!') {
					if (num != childVal) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {	
							condition(branchChosen, branchItr, sibCount);
							break;
						}
					}
				}
			}
			//Case 2: the user has entered a string (categorical variable)
			else {
				Tree fChild = tree2;
				TextValue childVal = cleanText(fChild->value).c_str();
				//If a string is entered and the value of the node is a number
				if (atoi(childVal.c_str()) > 0) {
					cout << "Invalid value" << endl;
					return NOTHING;
				}

				if (atoi(childVal.c_str()) > 0) {
					cout << "Invalid value" << endl;
					return NOTHING;
				}

				if (fChild->value[0] == '<') {
					if (strcmp(fArgs[argcount].c_str(), childVal.c_str()) < 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {	
							condition(branchChosen, branchItr, sibCount);
							break;
						}
					}
				}
				else if (fChild->value[0] == '>') {
					if (strcmp(fArgs[argcount].c_str(), childVal.c_str()) > 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {	
							condition(branchChosen, branchItr, sibCount);
							break;
						}
					}
				}
				else if (fChild->value[0] == '=') {
					if (strcmp(fArgs[argcount].c_str(), childVal.c_str()) == 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {	
							condition(branchChosen, branchItr, sibCount);
							break;
						}
					}
				}
				else if (fChild->value[0] == '!') {
					if (strcmp(fArgs[argcount].c_str(), childVal.c_str()) != 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {	
							condition(branchChosen, branchItr, sibCount);
							break;
						}
					}
				}
			}
			branchItr++;
			tree2 = tree2->nextSibling;
		}
		//The function did not find any arc viable on some node
		if (!found) {
			cout << "The prediction cannot take place as there is a node for which there is no viable arc" << endl;
			return NOTHING;
		}
		//The function has always found one and only one arc that can be traveled on each node
		if (branchItr != -1) {
			for (int y = 0; y < branchChosen; y++)
				tree3 = tree3->nextSibling;

			tree = tree3;
		}
		//The function has found more than one arc that can be traversed on the node and will randomly select the arc to be covered
		else {
			cout << "There is more than one valid condition, choosing randomly the arc to follow" << endl;
			for (int y = 0; y < branchChosen; y++)
				tree3 = tree3->nextSibling;

			tree = tree3;
		}
	}

	return NOTHING;	//We arrived at the bottom and none of the conditions of the function was met, I return NOTHING
}
/*******************************************************************************************************/
// The predictionFromInput function performs the prediction by asking the user to associate the values one by one
// to variables during the path leading to the prediction. In case there are more real conditions on the arcs
// choose one randomly, in case there is none print an error message and return NOTHING.
End tree::predictionFromInput(Tree &tree){
	srand(time(NULL)); 	//I call srand (time(NULL)) to make sure that the values are actually random with each call
	
	char userInput[50];
	while (tree != emptyTree) {
		
		//If the child is a leaf I can return the prediction
		if ((tree->firstChild->value[1] == 'A' || tree->firstChild->value[1] == 'B') && tree->firstChild->value[2] == 0) {
			if (tree->firstChild->value[1] == 'A')
				return HIGH;
			else
				return LOW;
		}

		int branchItr = 0, branchChosen = 0, sibCount = 0;
		bool found = false;
		string uinput;
		cout << "Insert " << trueLabel(tree->label) << ":" << endl;
		cin >> uinput;
		strcpy(userInput, uinput.c_str());

		int num = atoi(userInput);
		
		Tree tree2 = tree->firstChild;
		Tree tree3 = tree2;
		
		sibCount = siblingCount(tree3); //store in the sibCount variable the number of siblings (itself included) of the tree3 node

		//The cycle visits all his children, brother by brother
		while (tree2 != emptyTree) {
			//Case 1: the user has entered an integer (quantitative variable)
			if (num != 0) {
				Tree fChild = tree2;
				int childVal = atoi(cleanText(fChild->value).c_str());
				//The entered quantitative value contains letters: the variable is not valid
				if (hasAlphabets(userInput)) {
					cout << "Invalid value" << endl;
					return NOTHING;
				}

				//If a number is entered and the value of the node is a string
				if (childVal == 0)
				{
					cout << "Invalid value" << endl;
					return NOTHING;
				}

				if (fChild->value[0] == '<') {
					if (fChild->value[1] == '=') {
						if (num <= childVal) {
						//If no branch has been found yet mark it as found and store its index
							if (!found) {
								branchChosen = branchItr;
								found = true;
							} //If the branch has already been found and there is another true condition (the conditions are not mutually exclusive), call
							  // the function condition that randomly chooses the branch to go and I exit the loop
							else {
								condition(branchChosen, branchItr, sibCount);
								break;
							}
						}
					}
					else {
						if (num < childVal) {
							if (!found) {
								branchChosen = branchItr;
								found = true;
							}
							else {
								branchChosen = rand() % sibCount;
								branchItr = -1;
								break;
							}
						}
					}

				}
				else if (fChild->value[0] == '>') {
					if (fChild->value[1] == '=') {
						if (num >= childVal) {
							if (!found) {
								branchChosen = branchItr;
								found = true;
							}
							else {
								branchChosen = rand() % sibCount;
								branchItr = -1;
								break;
							}
						}
					}
					else {
						if (num > childVal) {
							if (!found) {
								branchChosen = branchItr;
								found = true;
							}
							else {
								branchChosen = rand() % sibCount;
								branchItr = -1;
								break;
							}
						}
					}
				}
				else if (fChild->value[0] == '=') {
					if (num == childVal) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {
							branchChosen = rand() % sibCount;
							branchItr = -1;
							break;
						}
					}
				}
				else if (fChild->value[0] == '!') {
					if (num != childVal) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {
							branchChosen = rand() % sibCount;
							branchItr = -1;
							break;
						}
					}
				}
			}
			//Case 2: the user has entered a string (categorical variable)
			else {
				Tree fChild = tree2;
				TextValue childVal = cleanText(fChild->value).c_str();

				//If a string is entered and the value of the node is a number
				if (atoi(childVal.c_str()) > 0)
				{
					cout << "Invalid value" << endl;
					return NOTHING;
				}

				if (fChild->value[0] == '<') {
					if (strcmp(userInput, childVal.c_str()) < 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {
							branchChosen = rand() % sibCount;
							branchItr = -1;
							break;
						}
					}
				}
				else if (fChild->value[0] == '>') {
					if (strcmp(userInput, childVal.c_str()) > 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {
							branchChosen = rand() % sibCount;
							branchItr = -1;
							break;
						}
					}
				}
				else if (fChild->value[0] == '=') {
					if (strcmp(userInput, childVal.c_str()) == 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {
							branchChosen = rand() % sibCount;
							branchItr = -1;
							break;
						}
					}
				}
				else if (fChild->value[0] == '!') {
					if (strcmp(userInput, childVal.c_str()) != 0) {
						if (!found) {
							branchChosen = branchItr;
							found = true;
						}
						else {
							branchChosen = rand() % sibCount;
							branchItr = -1;
							break;
						}
					}
				}
			}

			branchItr++;
			tree2 = tree2->nextSibling;
		}
		//The function did not find any arc viable on some node
		if (!found) {
			cout << "The prediction cannot take place as there is a node for which there is no viable arc" << endl;
			return NOTHING;
		}
		//The function has always found one and only one arc that can be traveled on each node
		if (branchItr != -1) {
			for (int y = 0; y < branchChosen; y++)
				tree3 = tree3->nextSibling;

			tree = tree3;
		}
		//The function has found more than one arc that can be traversed on the node and will randomly select the arc to be transversed
		else {
			cout << "There is more than one valid condition, choosing randomly the arc to follow" << endl;
			for (int y = 0; y < branchChosen; y++)
				tree3 = tree3->nextSibling;

			tree = tree3;
		}
	}
	return NOTHING;
}

/***********************************PRINT FUNCTIONS********************************************************************/
// Auxiliary function for structured display
void printTree(const Tree &t, int depth)
{
    if (isEmpty(t))
        return;
    string indent = "--";
    // insert indentation corresponding to the depth reached
    for (int i = 0; i < depth; i++)
        cout << indent;
    // display the information content associated with t: label and value of the associated condition
	if (t->value != noTextValue)
		cout << t->label << ", " << t->value << endl;
	else
		cout << t->label << endl;
    // Recursive call of printTree on each of the children of t (depth increased by 1)
    Tree auxT = t->firstChild;
    while (!isEmpty(auxT))
    {
        printTree(auxT, depth + 1);
        auxT = auxT->nextSibling;
    }
}

// Structured display: starting from level 0
void printTree(const Tree &t)
{
    printTree(t, 0);
}
/*******************************************************************************************************/
// Auxiliary function for printing variables in the tree
void printVariablesInner(tree::Tree &tree, string& variables) {
	if (tree != emptyTree) {

		//If the node is not a leaf, it adds the variable to 'variables' if it is not already there
		if (variables.find(trueLabel(tree->label)) == string::npos && tree->firstChild != emptyTree)
			variables = variables + ", " + trueLabel(tree->label);

		//Visit each tree node
		printVariablesInner(tree->firstChild, variables);
		printVariablesInner(tree->nextSibling, variables);
	}
}

//Print variables - calling function
void printVariables(tree::Tree tree)
{
	string variables;
	printVariablesInner(tree, variables);
	cout << "Le variabili nell'albero di decisione sono: " << variables.substr(2, string::npos) << endl;
}

/************************READING FUNCTIONS****************************************/
Tree readFromStream(istream &str)
{
    Tree t = createEmpty();
    string line;
    Label rootLabel, fatherLabel, childLabel = emptyLabel;
    TextValue rootValue, fatherValue, childValue = noTextValue;
    getline(str, line);
    istringstream instream; // we use a variable of type istringstream to be able to scan the pieces of each line using >>
    instream.clear();
    instream.str(line);
    instream >> rootLabel;             // the first element encountered in the file is the root label, by convention on how the file should be done
    addElem(emptyLabel, rootLabel, noTextValue, t); // it is inserted in the empty tree, indicating that the father is not there (first argument emptyLabel)
    getline(str, line);                // then we start scanning the following lines
    instream.clear();
    instream.str(line);
    while (!str.eof())
    {
        instream >> fatherLabel;           // in each line of the file, the first element is the label of the parent node and the others are the labels of the children
        
        while (!instream.eof())            // until the current line is finished
        {
			TextValue temp;
            instream >> temp;              

			if (isCondition(temp[0], temp[1]) || isCondition(temp[temp.length() - 1], ' '))
				childValue = temp;
			else 
				childLabel = temp;

			if (childLabel != emptyLabel && childValue != noTextValue) {
				addElem(fatherLabel, childLabel, childValue, t); // attach the readen label to the father
				childLabel = emptyLabel;
				childValue = noTextValue;
			}
        }
        getline(str, line);
        instream.clear();
        instream.str(line);
    }
    str.clear();
    return t;
}

/****************************************************************/
Tree readFromFile(string nome_file)
{
    ifstream ifs(nome_file.c_str()); // opening of a stream associated with a file, in reading mode
    if (!ifs)
    {
        cout << "\nErrore apertura file, verificare di avere inserito un nome corretto\n";
        return createEmpty();
    }
    return readFromStream(ifs);
}
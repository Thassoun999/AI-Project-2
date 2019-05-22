// AIProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

//Structure node to hold all relevant data we need to preserve through iterations
struct stateData {
	int heuristic;
	int depth;
	vector<vector<int>> state = { {0, 0, 0} , {0,0,0}, {0,0,0} };
	vector<char> directionSequence;
	vector<int> heuristicSequence;


	stateData() {}
	stateData(int heuristic, char direction) : heuristic(heuristic) {}

};

//Changing compare function in priority queue so it works like a minheap and only evaluates the heuristic
struct compareHeuristic {
	bool operator()(stateData const& p1, stateData const& p2) {
		return p1.heuristic > p2.heuristic;
	}
};

//Checks for equivalency between current state and goal state!
bool checkGoal(vector<vector<int>> initial, vector<vector<int>> goal) {
	for (size_t i = 0; i < initial.size(); i++) {
		for (size_t j = 0; j < initial[i].size(); j++) {
			if (initial[i][j] != goal[i][j]) {
				return false;
			}
		}
	}
	return true;
}


//Heuristic Function (1) --> Sum of Manhattan Distances of tiles from their goal position
//find goal tile here and calculate difference 
int heuristicHelper(vector<vector<int>>& init, vector<vector<int>>& goal, int i, int j) {
	for (int gi = 0; gi < 3; gi++) {
		for (int gj = 0; gj < 3; gj++) {
			if (goal[gi][gj] == init[i][j]) {
				//cout << i << " and " << j << endl;
				//cout << gi << " et " << gj << endl;
 				return abs(gi - i) + abs(gj - j);

			}
		}
	}
	return 0;
}

//For each value, find its goal position and calculate the manhattan distance!
//Find init tile here
int heuristicfunc(vector<vector<int>>& init, vector<vector<int>>& goal) {
	int heurVal = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (init[i][j] != goal[i][j] && init[i][j] != 0) {
				heurVal += heuristicHelper(init, goal, i, j);
			}
		}
	}

	return heurVal;
}

//On same line and goal positions are both in that line
//On the same line means each tile pair is on the same row OR column
//Additionally we are checking if their goal positions are on the same line mentioned above
bool check1(int i, int j, int i1, int j1, vector<vector<int>> &init, vector<vector<int>>& goal) {
	//Same line check
	if (i1 == i || j1 == j) {
		//Find goal tile positions
		int val1 = init[i][j];
		int val2 = init[i1][j1];

		int row1;
		int col1;
		int row2;
		int col2;

		for (int gi = 0; gi < 3; gi++) {
			for (int gj = 0; gj < 3; gj++) {
				if (goal[gi][gj] == val1) {
					row1 = gi;
					col1 = gj;
				}
				if (goal[gi][gj] == val2) {
					row2 = gi;
					col2 = gj;
				}
			}
		}

		//Goal same line check
		if (i1 == i) {
			if (row1 == i && row2 == i1) {
				return true;
			}
		}

		if (j1 == j) {
			if (col1 == j && col2 == j1) {
				return true;
			}
		}

	}
	return false;
}

//Goal position of tj and tk on same line
//Then if they are, check for a block
bool check2(int i, int j, int i1, int j1, vector<vector<int>> &init, vector<vector<int>>& goal) {
	//Using a 1D array helps a lot better for this application
	//Find goal positions of our two values and the initial positions of our two values
	int val1 = init[i][j];
	int val2 = init[i1][j1];

	int initindex1;
	int initindex2;
	int goalindex2;
	int goalindex1;

	vector<int> init1D;
	vector<int> goal1D;

	//1D construction
	for (int gi = 0; gi < 3; gi++) {
		for (int gj = 0; gj < 3; gj++) {

			init1D.push_back(init[gi][gj]);
			goal1D.push_back(goal[gi][gj]);
		}
	}

	//Find goal indices
	for (int gi = 0; gi < 9; gi++) {
		if (init1D[gi] == val1) {
			initindex1 = gi;
		}
		if (init1D[gi] == val2) {
			initindex2 = gi;
		}

		if (goal1D[gi] == val1) {
			goalindex1 = gi;
		}
		if (goal1D[gi] == val2) {
			goalindex2 = gi;
		}
	}

	//AT GOAL POSITIONS
	if (initindex1 == goalindex1 && initindex2 == goalindex2) {
		return false;
	}

	
	//Goal pos of tj + tk on same line as eval line
	//The idea is tj is to the right of tk, and goal position of tj is the left of the goal position of tk
	//tj and tk interchangeable between our pairs, thus the OR statement
	if ((initindex1 > initindex2 && goalindex2 >= goalindex1) || (initindex2 > initindex1 && goalindex1 >= goalindex2)) {
		return true;
	}
	
	

	//No linear conflict
	return false;

}


//Heuristic Function (2) --> Sum of Manhattan Distances of tiles from their goal position + 2 * (# of linear conflicts)
int heuristicfunc2(vector<vector<int>>& init, vector<vector<int>>& goal) {
	int heurVal = heuristicfunc(init, goal); //First portion --> easy access to finding the manhattan distances
	//Now to find linear conflict


	int linearConflict = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			
			for (int i1 = i; i1 < 3; i1++) {
				for (int j1 = j; j1 < 3; j1++) {

					//Check to make sure they are not same tile OR one of their tiles isn't 0
					if (!(i1 == i && j1 == j) && init[i][j] != 0 && init[i1][j1] != 0) {
						//check 1, see comments
						if (check1(i, j, i1, j1, init, goal)) {
							//check 2, see comments
							if (check2(i, j, i1, j1, init, goal)) {
								//cout << init[i][j] << " " << init[i1][j1] << endl;
								linearConflict++;
							}
						}
					}
				}
			}


		}
	}
	//cout << linearConflict << endl;
	//Heuristic return
	return heurVal + 2 * linearConflict;
}

//THIS ONE USES HEURISTIC FUNCTION 1
stateData aStarSearch1(stateData initial, stateData goal, int& numNodes, priority_queue< stateData, vector< stateData>, compareHeuristic>& priQ) {
	//Quick initialization since no movement has been done yet
	char lastMove = 'N';


	while (!(priQ.empty())) {

		//Evaluate top item in our priority queue (organized via min heap)
		stateData evaluation = priQ.top();
		priQ.pop();
		
		//Done Check!!!!!
		if (checkGoal(evaluation.state, goal.state)) {
			return evaluation;
		}

		
		//Find 0 position
		int row = 0;
		int col = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (evaluation.state[i][j] == 0) {
					row = i;
					col = j;
					break;
				}
			}
		}

		//Movement evaluation
		bool up = true;
		bool down = true;
		bool left = true;
		bool right = true;

		if (row == 0) {
			up = false;
		}
		else if (row == 2) {
			down = false;
		}

		if (col == 0) {
			left = false;
		}
		else if (col == 2) {
			right = false;
		}

		//Kill Repeated Movements!!!!!
		if (lastMove != 'N') {

			if (lastMove == 'U') {
				down = false;
			}
			else if (lastMove == 'D') {
				up = false;
			}
			else if (lastMove == 'L') {
				right = false;
			}
			else if (lastMove == 'R') {
				left = false;
			}
		}


		//Find children (nodes) and Heuristic evaluations!
		//Heuristic Evaluation
		//h(n) is from the heuristic func
		//Since g(n) is just total path cost from root to node, we can use depth+1 as the resultant
		int heurVal = 0;
		char direction;
		//4 possible children!!
		if (up) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row - 1][col], evaluation.state[row][col]);
			heurVal = heuristicfunc(evaluation.state, goal.state);
			direction = 'U';
			//Child created, data accumulated, inserted into priority Q
			stateData child1 = evaluation;
			child1.directionSequence.push_back(direction);
			child1.heuristic = heurVal + evaluation.depth + 1;
			child1.depth += 1;
			child1.heuristicSequence.push_back(heurVal + evaluation.depth + 1);



			priQ.push(child1);
			swap(evaluation.state[row - 1][col], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}
		if (down) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row + 1][col], evaluation.state[row][col]);
			heurVal = heuristicfunc(evaluation.state, goal.state);
			direction = 'D';
			//Child created, data accumulated, inserted into priority Q
			stateData child2 = evaluation;
			child2.directionSequence.push_back(direction);
			child2.heuristic = heurVal + evaluation.depth + 1;
			child2.depth += 1;
			child2.heuristicSequence.push_back(heurVal + evaluation.depth + 1);

			priQ.push(child2);
			swap(evaluation.state[row + 1][col], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}
		if (left) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row][col - 1], evaluation.state[row][col]);
			heurVal = heuristicfunc(evaluation.state, goal.state);
			direction = 'L';
			//Child created, data accumulated, inserted into priority Q
			stateData child3 = evaluation;
			child3.directionSequence.push_back(direction);
			child3.heuristic = heurVal + evaluation.depth + 1;
			child3.depth += 1;
			child3.heuristicSequence.push_back(heurVal + evaluation.depth + 1);


			priQ.push(child3);
			swap(evaluation.state[row][col - 1], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}
		if (right) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row][col + 1], evaluation.state[row][col]);
			heurVal = heuristicfunc(evaluation.state, goal.state);
			direction = 'R';
			//Child created, data accumulated, inserted into priority Q
			stateData child4 = evaluation;
			child4.directionSequence.push_back(direction);
			child4.heuristic = heurVal + evaluation.depth + 1;
			child4.depth += 1;
			child4.heuristicSequence.push_back(heurVal + evaluation.depth + 1);


			priQ.push(child4);
			swap(evaluation.state[row][col + 1], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}

		//We mark the last move for killing the repeated movement portion of the code!!!!
		lastMove = priQ.top().directionSequence[priQ.top().directionSequence.size() - 1];
		

	}
	
	return initial;
}

//THIS ONE USES HEURISTIC FUNCTION 2
stateData aStarSearch2(stateData initial, stateData goal, int& numNodes, priority_queue< stateData, vector< stateData>, compareHeuristic>& priQ) {
	//Quick initialization since no movement has been done yet
	char lastMove = 'N';


	while (!(priQ.empty())) {

		//Evaluate top item in our priority queue (organized via min heap)
		stateData evaluation = priQ.top();
		priQ.pop();

		//Done Check!!!!!
		if (checkGoal(evaluation.state, goal.state)) {
			return evaluation;
		}


		//Find 0 position
		int row = 0;
		int col = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (evaluation.state[i][j] == 0) {
					row = i;
					col = j;
					break;
				}
			}
		}

		//Movement evaluation
		bool up = true;
		bool down = true;
		bool left = true;
		bool right = true;

		if (row == 0) {
			up = false;
		}
		else if (row == 2) {
			down = false;
		}

		if (col == 0) {
			left = false;
		}
		else if (col == 2) {
			right = false;
		}

		//Kill Repeated Movements!!!!!
		if (lastMove != 'N') {

			if (lastMove == 'U') {
				down = false;
			}
			else if (lastMove == 'D') {
				up = false;
			}
			else if (lastMove == 'L') {
				right = false;
			}
			else if (lastMove == 'R') {
				left = false;
			}
		}


		//Find children (nodes) and Heuristic evaluations!
		//Heuristic Evaluation
		//h(n) is from the heuristic func
		//Since g(n) is just total path cost from root to node, we can use depth+1 as the resultant
		int heurVal = 0;
		char direction;
		//4 possible children!!
		if (up) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row - 1][col], evaluation.state[row][col]);
			heurVal = heuristicfunc2(evaluation.state, goal.state);
			direction = 'U';
			//Child created, data accumulated, inserted into priority Q
			stateData child1 = evaluation;
			child1.directionSequence.push_back(direction);
			child1.heuristic = heurVal + evaluation.depth + 1;
			child1.depth += 1;
			child1.heuristicSequence.push_back(heurVal + evaluation.depth + 1);



			priQ.push(child1);
			swap(evaluation.state[row - 1][col], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}
		if (down) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row + 1][col], evaluation.state[row][col]);
			heurVal = heuristicfunc2(evaluation.state, goal.state);
			direction = 'D';
			//Child created, data accumulated, inserted into priority Q
			stateData child2 = evaluation;
			child2.directionSequence.push_back(direction);
			child2.heuristic = heurVal + evaluation.depth + 1;
			child2.depth += 1;
			child2.heuristicSequence.push_back(heurVal + evaluation.depth + 1);

			priQ.push(child2);
			swap(evaluation.state[row + 1][col], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}
		if (left) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row][col - 1], evaluation.state[row][col]);
			heurVal = heuristicfunc2(evaluation.state, goal.state);
			direction = 'L';
			//Child created, data accumulated, inserted into priority Q
			stateData child3 = evaluation;
			child3.directionSequence.push_back(direction);
			child3.heuristic = heurVal + evaluation.depth + 1;
			child3.depth += 1;
			child3.heuristicSequence.push_back(heurVal + evaluation.depth + 1);


			priQ.push(child3);
			swap(evaluation.state[row][col - 1], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}
		if (right) {
			//We swap so we can evaluate the heuristic at a child state!
			//Second swap to fix what we did and work on our other children
			swap(evaluation.state[row][col + 1], evaluation.state[row][col]);
			heurVal = heuristicfunc2(evaluation.state, goal.state);
			direction = 'R';
			//Child created, data accumulated, inserted into priority Q
			stateData child4 = evaluation;
			child4.directionSequence.push_back(direction);
			child4.heuristic = heurVal + evaluation.depth + 1;
			child4.depth += 1;
			child4.heuristicSequence.push_back(heurVal + evaluation.depth + 1);


			priQ.push(child4);
			swap(evaluation.state[row][col + 1], evaluation.state[row][col]);
			//Node evaluation
			numNodes++;
		}

		//We mark the last move for killing the repeated movement portion of the code!!!!
		lastMove = priQ.top().directionSequence[priQ.top().directionSequence.size() - 1];


	}

	return initial;
}

//Read file
void inFile(vector<vector<int>>& initial, vector<vector<int>>& goal, string documentName) {
	ifstream myfile;
	myfile.open(documentName);
	if (!myfile) {
		cerr << "Unable to open file datafile.txt";
		exit(1);   // call system to stop
	}
	int value = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			myfile >> value;
			initial[i][j] = value;
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			myfile >> value;
			goal[i][j] = value;
		}
	}

	myfile.close();
}

//Make file
void outFile(int numNodes, stateData solution, string destinationName, stateData initialState, stateData goalState) {
	ofstream myfile(destinationName);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			myfile << initialState.state[i][j] << " ";
		}
		myfile << endl;
	}

	myfile << endl;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			myfile << goalState.state[i][j] << " ";
		}
		myfile << endl;
	}


	myfile << endl;
	
	myfile << solution.depth << endl;
	myfile << numNodes << endl;

	for (size_t i = 0; i < solution.directionSequence.size(); i++) {
		myfile << solution.directionSequence[i] << " ";
	}
	myfile << endl;
	for (size_t i = 0; i < solution.heuristicSequence.size(); i++) {
		myfile << solution.heuristicSequence[i] << " ";
	}

	myfile.close();

}

int main()
{
	//Empty "tile" states
	vector<vector<int>> initialState = { {0, 0, 0} , {0,0,0}, {0,0,0} };
	vector<vector<int>> goalState = { {0, 0, 0} , {0,0,0}, {0,0,0} };

	string documentName;
	string destinationName;
	int heuristic;
	

	//String inputs are pretty explanatory
	cout << "Enter a file name with .txt extension: " << endl;
	cin >> documentName;
	cout << "Create a destination name with .txt extension:" << endl;
	cin >> destinationName;
	
	cout << "Enter '1' for Heuristic Function 1 and '2' for Heuristic Function 2: ";
	cin >> heuristic;
	if (!(heuristic == 1 || heuristic == 2)) {
		cerr << "Not a valid function!!!";
		exit(1);   // call system to stop
	}
	
	//Takes in file, reads and updates initialState and goalState
	inFile(initialState, goalState, documentName);

	//Create our nodes and put in those tiles
	stateData initial;
	stateData goal;

	initial.state = initialState;
	goal.state = goalState;

	
	//Some necessary items
	int numNodes = 1;
	int depth = 0;
	int newHeur;

	if (heuristic == 1) {
		newHeur = heuristicfunc(initial.state, goal.state);
	}
	else if (heuristic == 2) {
		newHeur = heuristicfunc2(initial.state, goal.state);
	}

	//File our initial node, goal node does not need to have information apart from its tile state
	initial.heuristic = newHeur;
	initial.heuristicSequence.push_back(newHeur);
	initial.depth = depth;
	

	//Create a priority queue that uses the nodee and uses our comparison function, push in initial
	priority_queue< stateData, vector< stateData>, compareHeuristic> priQ;
	priQ.push(initial);
	//Solution will be updated based on aStarSearch with respective heuristic function
	stateData solution;
	
	if (heuristic == 1) {
		solution = aStarSearch1(initial, goal, numNodes, priQ);
	}
	else if (heuristic == 2) {
		solution = aStarSearch2(initial, goal, numNodes, priQ);
	}
	

	
	//These are just console prints, these are just for testing
	cout << solution.depth << endl;
	cout << numNodes << endl;

	for (size_t i = 0; i < solution.directionSequence.size(); i++) {
		cout << solution.directionSequence[i] << " ";
	}
	cout << endl;
	for (size_t i = 0; i < solution.heuristicSequence.size(); i++) {
		cout << solution.heuristicSequence[i] << " ";
	}
	
	cout << endl;
	
	//Here is where the file will be created with all of our accumulated data
	outFile(numNodes, solution, destinationName, initial, goal);
  
}



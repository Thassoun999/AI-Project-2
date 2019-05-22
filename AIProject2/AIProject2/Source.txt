// AIProject2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>

using namespace std;


//Separate structure keep track of each tile's LEGAL values (it's a 9x9x9 deal)
struct legalValues {
	legalValues() {}
	vector<bool> legals = { true, true, true, true, true, true, true, true, true };
};

struct TileLegals {
	TileLegals() {}
	vector<vector<legalValues>> legalTilez;
};


struct Tile {
	Tile () {}
	int data;
	//Degree Heuristic
	int degreeHeuristic = 0;
	//MRV Heuristic
	int amountLegal = 9;
	//vector<bool> legalValues = { true, true, true, true, true, true, true, true, true };
};


struct stateData {
	//For rows and columns
	stateData() {}
	int amountFilled = 0;
	vector<vector<Tile>> tilesState;
	
};

void amountFilledFunc(stateData& initial) {
	int amountFilledEval = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (initial.tilesState[i][j].data != 0) {
				amountFilledEval++;
			}
		}
	}

	initial.amountFilled = amountFilledEval;
}

void printBoard(stateData initial) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << initial.tilesState[i][j].data << " ";
		}
		cout << endl;
	}
}


vector<Tile*> minimumRemainingValueHeuristics(stateData& copyInitial, TileLegals& legVals, vector<legalValues*>& evalLegalTileVec) {

	int miniHeur = INT_MAX;
	Tile* miniHeurTile = &copyInitial.tilesState[0][0];
	legalValues* miniHeurTileLegal = new legalValues;


	//Run this initial check trying to find the minimum remaining values heurstic for each tile, returning the min tile
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {

			if (copyInitial.tilesState[i][j].data != 0) {
				continue;
			}

			if (copyInitial.tilesState[i][j].amountLegal < miniHeur) {
				miniHeurTile = &copyInitial.tilesState[i][j];
				miniHeur = copyInitial.tilesState[i][j].amountLegal;
				miniHeurTileLegal = &(legVals.legalTilez[i][j]);
			}


		}
	}

	vector<Tile*> miniHeurTileVector;
	miniHeurTileVector.push_back(miniHeurTile);
	//Vec of legals, should have matching index with optimal tile
	evalLegalTileVec.push_back(miniHeurTileLegal);

	//Want to run this check a second time to see if there are any tiles with same MRV value!!
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (copyInitial.tilesState[i][j].data != 0) {
				continue;
			}

			if (copyInitial.tilesState[i][j].amountLegal == miniHeur) {
				//The fact we are here means we need to run Heuristic 2
				miniHeurTile = &copyInitial.tilesState[i][j];
				miniHeurTileLegal = &(legVals.legalTilez[i][j]);
				miniHeurTileVector.push_back(miniHeurTile);
				evalLegalTileVec.push_back(miniHeurTileLegal);
			}
		}
	}


	return miniHeurTileVector;


}


//Highest Number of unassigned neighbors!
Tile* degreeHeuristic(vector<Tile*>& tileToConsider, vector<legalValues*>& evalLegalTileVec, legalValues*& evalLegalTile) {
	int miniHeur = INT_MAX;
	Tile* miniHeurTile = tileToConsider[0];


	//Run this initial check trying to find the minimum degree heuristics for each tile, returning the min tile
	//This list is obtained from the tiles we got in our first heuristic!! The ties!

	for (size_t i = 0; i < tileToConsider.size(); i++) {
		if (tileToConsider[i]->data != 0) { //This probably won't happen
			continue;
		}

		if (tileToConsider[i]->degreeHeuristic < miniHeur) {
			miniHeur = tileToConsider[i]->degreeHeuristic;
			miniHeurTile = tileToConsider[i];
			//Get legals
			evalLegalTile = evalLegalTileVec[i];
		}
	}

	

	return miniHeurTile;
}


//Has a recursive problem! Need to fix, may need to make iterative, save the previous values or something?
bool forwardChecking(stateData& copyInitial, TileLegals& legVals) {

	//Needed for later
	//bool failure = false;
	bool reDo = true;
	while (reDo == true) {


		//Make copies of our state, that way if we have a failure we can go to the previous iteration of our data 
		//Same for legal values
		//Problem here?

		reDo = false;

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {


				int amountLegalTemp = 9;
				int amountDegreeTemp = 0;

				int evalValue = copyInitial.tilesState[i][j].data;
				if (evalValue != 0) {
					continue;
				}


				//Col evaluation[colEvalLegal - 1]
				for (int col = 0; col < 9; col++) {
					//Find out column discrepencies for filled tiles
					int colEvalLegal = copyInitial.tilesState[i][col].data;
					if (colEvalLegal != 0 && (legVals.legalTilez[i][j].legals[colEvalLegal - 1]) == true) {
						legVals.legalTilez[i][j].legals[colEvalLegal - 1] = false;
						//Just to state, this will be our MRV evaluation
						amountLegalTemp -= 1;
					}
					else if (colEvalLegal == 0) {
						//This will be our degree Heurstic Evaluation
						amountDegreeTemp++;
					}
				}

				//Row Evaluation
				for (int row = 0; row < 9; row++) {
					//Find out row discrepencies for filled tiles
					int rowEvalLegal = copyInitial.tilesState[row][j].data;
					if (rowEvalLegal != 0 && (legVals.legalTilez[i][j].legals[rowEvalLegal - 1]) == true) {
						legVals.legalTilez[i][j].legals[rowEvalLegal - 1] = false;
						amountLegalTemp -= 1;
					}
					else if (rowEvalLegal == 0) {
						amountDegreeTemp++;
					}
				}

				//Block Evaluation

				

				//Find the block row and column

				int blockRow = i / 3;
				int blockCol = j / 3;

				

				//Evaluate all 9 blocks, from 1st to 9nth
				if (blockRow == 0 && blockCol == 0) {
					for (int row = 0; row < 3; row++) {
						for (int col = 0; col < 3; col++) {
							//Find out block discrepencies for filled tiles
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 0 && blockCol == 1) {
					for (int row = 0; row < 3; row++) {
						for (int col = 3; col < 6; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 0 && blockCol == 2) {
					for (int row = 0; row < 3; row++) {
						for (int col = 6; col < 9; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 1 && blockCol == 0) {
					for (int row = 3; row < 6; row++) {
						for (int col = 0; col < 3; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 1 && blockCol == 1) {
					for (int row = 3; row < 6; row++) {
						for (int col = 3; col < 6; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 1 && blockCol == 2) {
					for (int row = 3; row < 6; row++) {
						for (int col = 6; col < 9; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 2 && blockCol == 0) {
					for (int row = 6; row < 9; row++) {
						for (int col = 0; col < 3; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 2 && blockCol == 1) {
					for (int row = 6; row < 9; row++) {
						for (int col = 3; col < 6; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}
				else if (blockRow == 2 && blockCol == 2) {
					for (int row = 6; row < 9; row++) {
						for (int col = 6; col < 9; col++) {
							int blockEvalLegal = copyInitial.tilesState[row][col].data;
							if (blockEvalLegal != 0 && (legVals.legalTilez[i][j].legals[blockEvalLegal - 1]) == true) {
								legVals.legalTilez[i][j].legals[blockEvalLegal - 1] = false;
								amountLegalTemp -= 1;
							}
							else if (blockEvalLegal == 0) {
								amountDegreeTemp++;
							}
						}
					}
				}

				//THE ABOVE ARE REPEATS!! PLEASE REFER TO FIRST FEW COMMENTS

				//Check if one more legal values!!! If not we need to assign the value and then REDO forward checking
				int cccount = 0; //ignore this, not that important, look to remove later
				if (amountLegalTemp == 1) {
					//Find out which one is true!
					for (int newValLegal = 0; newValLegal < 9; newValLegal++) {
						if (legVals.legalTilez[i][j].legals[newValLegal] == true && cccount == 0) {
							copyInitial.tilesState[i][j].data = newValLegal + 1;
							cccount = 1;
						}
					}

					reDo = true;
					//cout << "there" << endl;
				}


				//If no legal values, we want to RETURN FALSE
				if (amountLegalTemp <= 0) {
					//failure = true;
					//If failure, remove legal value from it! --> in backtracking
					
					return false;
				}

				copyInitial.tilesState[i][j].amountLegal = amountLegalTemp;
				copyInitial.tilesState[i][j].degreeHeuristic = amountDegreeTemp;

			}
		}

		//Figure out the code for this portion.

		
	}


	//Done forward checking with no hassles!
	
	return true;

}

//Have a helper function to be able to copy everything correctly;

//FORWARD CHECKING LAST!

//Maybe keep TileLegals as a pass by copy!!!
bool backTracking_helper(stateData& initial, stateData copyInitial, TileLegals legVals) { //Will either return a solution or a failure

	//Base case, don't continue if our entire board is filled up!
	amountFilledFunc(copyInitial);
	if (copyInitial.amountFilled == 81) {
		initial.amountFilled = copyInitial.amountFilled;
		initial.tilesState = copyInitial.tilesState;
		return true;
	}
	

	//printBoard(copyInitial);
	//cout << endl;

	//Select an unassigned Tile and from there we want to test a value

	//Run minimumRemaininValueHeuristics to pick unassigned Tile

	//Gotta also figure out the indices that correlated to our legVals!
	vector<legalValues*> evalLegalTileVec;
	vector<Tile*> checkVec = minimumRemainingValueHeuristics(copyInitial, legVals, evalLegalTileVec);

	//Tile* check = &copyInitial.tilesState[0][0];

	Tile* check = checkVec[0];
	legalValues* evalLegalTile = new legalValues;

	if (checkVec.size() > 1) {
		//Run degreeHeuristics to pick unassigned Tile since first one failed
		//cout << evalLegalTileVec.size() << " " << checkVec.size() << endl;
		check = degreeHeuristic(checkVec, evalLegalTileVec, evalLegalTile); //Something wrong here!!!
		//cout << "there1 and " << bigCounter << endl;
	}
	else if (checkVec.size() == 1) {
		evalLegalTile = evalLegalTileVec[0];
	}

	//We picked an unassigned tile!!! We want to assign it a value!
	bool finalForwardCheck = false;
	bool finalBackTrackCheck = false;
	
	for (int valuetoAdd = 0; valuetoAdd < 9; valuetoAdd++) {
		//Check for consistency! If inconsistent, go to next value!
		if (evalLegalTile->legals[valuetoAdd] == false) {
			continue;
		}

		//ASSIGN AND DO A FORWARD CHECK WITH THAT VALUE
		check->data = valuetoAdd + 1;

		//We pass in a copy of our main two structures, if forward checking succeeds we apply them
		stateData copyOfCopyInitial;
		TileLegals copyOfLegalVals;

		copyOfCopyInitial.amountFilled = copyInitial.amountFilled;
		copyOfCopyInitial.tilesState = copyInitial.tilesState;
		copyOfLegalVals.legalTilez = legVals.legalTilez;

		finalForwardCheck = forwardChecking(copyOfCopyInitial, copyOfLegalVals);
		if (finalForwardCheck == false) {
			//Re-evaluate the heuristics!
			continue; //Try with next value
		}
		//If true we assign it
		copyInitial.amountFilled = copyOfCopyInitial.amountFilled;
		copyInitial.tilesState = copyOfCopyInitial.tilesState;
		legVals.legalTilez = copyOfLegalVals.legalTilez;
		
		finalBackTrackCheck = backTracking_helper(initial, copyInitial, legVals);
		if (finalBackTrackCheck == true) {
			return true;
		}
	}
	//cout << "here" << endl; // Check if this ever happens
	return false;
}

bool backTracking(stateData& initial, TileLegals& legVals) {
	stateData copyInitial;
	copyInitial.amountFilled = initial.amountFilled;
	copyInitial.tilesState = initial.tilesState;
	return backTracking_helper(initial, copyInitial, legVals);
}


//Create all of our structures from file input
void inFile(vector<vector<Tile>>& tileStateVec, string documentName) {
	ifstream myfile;
	myfile.open(documentName);

	if (!myfile) {
		cerr << "Unable to open file datafile.txt";
		exit(1);   // call system to stop
	}

	//Copy all our values into our 2D array of Tiles!
	int value = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			myfile >> value;
			tileStateVec[i][j].data = value;

		}
	}


	myfile.close();
}

void outFile(stateData& initial, string documentName) {
	ofstream myfile(documentName);

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			myfile << initial.tilesState[i][j].data << " ";
		}
		myfile << endl;
	}
	
	myfile.close();
}

int main()
{
	//Empty Initial State Vector
	vector<vector<Tile>> tileStateVec(9, vector<Tile>(9));

	string documentName;
	string destinationName;

	//String inputs are pretty explanatory
	cout << "Enter a file name with .txt extension: " << endl;
	cin >> documentName;
	

	cout << "Create a destination name with .txt extension:" << endl;
	cin >> destinationName;


	//We are keeping the list of our legal moves for each tile (9x9) in this structure! Each tile can have 9 moves!
	TileLegals legVals;
	vector<vector<legalValues>> legValsVec(9, vector<legalValues>(9));
	legVals.legalTilez = legValsVec;


	//Read and put new values into our 9x9 2D vector
	inFile(tileStateVec, documentName);

	//Create our struct
	stateData initial;
	initial.tilesState = tileStateVec;

	//Find out the amount of filled tiles!
	amountFilledFunc(initial);

	//Forward Checking Here (initially)
	//THIS WILL ALSO UPDATE OUR HEURISTIC VALUES!!!!!!!!
	bool solvable = forwardChecking(initial, legVals);
	if (!solvable) {
		//If Forward Checking fails with our initial input, do not bother updating anything!
		cout << "We can't solve this Sudoku board, please input a new one!" << endl;
		exit(1);
	}

	//BACKTRACKING ALGORITHM
	/*
	cout << initial.tilesState[0][4].data << endl;
	for (int u = 0; u < 9; u++) {
		cout << legVals.legalTilez[0][1].legals[u] << " ";
	}
	cout << endl;
	cout << endl;
	*/
	solvable = backTracking(initial, legVals);
	if (!solvable) {
		cout << "We can't solve this Sudoku board, please input a new one!" << endl;
		exit(1);
	}


	//PRINTING SOLUTION TO CONSOLE
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << initial.tilesState[i][j].data << " ";
		}
		cout << endl;
	}

	outFile(initial, destinationName);
}


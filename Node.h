/*
Seneca College - School of Information and Communications Technology
Filename:  Node.h
Author:    Dr. Elliott Coleshill
Purpose:   The class definition of a single node within a standard artificial neural
           network.
*/

#ifndef _NODE_H_
#define _NODE_H_

#include <iostream>
#include <math.h>
#include <ctime>
#include <vector>
using namespace std;

struct NodeConnection;			//Forward declaration

//The different types of nodes allowed in the system
static enum NODE_TYPE {INPUT = 0, HIDDEN, OUTPUT};

class Node
{
	int NodeID;										//Unique ID for the node
	NODE_TYPE nType;								//The type of Node that has been created
	vector<NodeConnection> InputConnections;		//Vector space containing pointers to all input nodes
	vector<NodeConnection> OutputConnections;		//Vector space containing pointers to all output nodes
	float NodeValue;								//The calculated output values
	float NodeBias;								//The nodes bias value
	float NodeDelta;								//Learning difference delta value for node

	//Private helper functions
	float CalcNeti();

public:

	//Initialization
	Node(NODE_TYPE type, int ID);					//Constructor
	void AddInput(Node& NodeToAdd);					//Adds a Node connection to the inputs
	void AddOutput(Node& NodeToAdd);				//Adds a Node connection to the outputs
	void Init();									//Performs random initialization of weights & bias values
	Node* getAddr() { return this; };				//Returns the address of current object

	//NN Execution
	bool Run(const float& InputValue);						//Executes the node and calculates the output based on input stimulis
	float getNodeValue() const { return NodeValue; };		//Query function to get the address of the output values

	//NN Training
	bool CalcDeltas(float Target);									//Calculates the delta values for the given node
	void UpdateWeights(const float ada, const float OutputDelta);	//Updates the input weights and bias values for the node
	float getWeight(int NodeID);									//Returns the weight value of the given NodeID connection
	int getNodeID() { return NodeID; };								//Returns the NodeID of the current object

	//Debugging support
	void DisplayContents(ostream& os);						//Prints the contents of the node to the screen
	void DisplayConnections(ostream& os);					//Outputs just the input and output connections
};

struct NodeConnection
{
	Node* NodeObj;						//Pointer to the Node
	float Weight;						//Weighted value of the connection
};

#endif
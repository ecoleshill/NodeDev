/*
Seneca College - School of Information and Communications Technology
Filename:  Node.cpp
Author:    Dr. Elliott Coleshill
Purpose:   Implementation file for Class Node functionality
*/

#include "Node.h"

/************* INITIALIZATION ***************/
Node::Node(NODE_TYPE type, int ID)
{
	NodeID = ID;
	nType = type;
}

void Node::AddInput(Node& NodeToAdd)
{
	NodeConnection NewConnection;
	NewConnection.NodeObj = &NodeToAdd;
	NewConnection.Weight = 0.0;

	InputConnections.push_back(NewConnection);
}

void Node::AddOutput(Node& NodeToAdd)
{
	NodeConnection NewConnection;
	NewConnection.NodeObj = &NodeToAdd;
	NewConnection.Weight = 0.0;

	OutputConnections.push_back(NewConnection);
}
void Node::Init()
{
	NodeValue = 0.0;
	NodeBias = 0.0;

	srand((unsigned int)time(NULL));
	float RandomValue;					//Holds the randomly generated number
	int Sign;							//Used to determine +/-

	switch (nType)
	{
	case INPUT:
		return;							//Just return.  No init required for input nodes

	case HIDDEN:
	case OUTPUT:
		//Randomly set the node weights, bias and values
		for (int x = 0; x < (int)InputConnections.size(); x++)
		{
			//InputWeights
			RandomValue = (float)(rand() % 10000000) / 10000000;
			Sign = rand() % 2;

			if (Sign == 0)
				RandomValue = RandomValue * -1;

			InputConnections[x].Weight = RandomValue;
		}

		//Only Hidden nodes have bias weights
		if (nType == HIDDEN)
		{
			RandomValue = (float)(rand() % 10000000) / 10000000;
			Sign = rand() % 2;

			if (Sign == 1)
				NodeBias = RandomValue;
			else
				NodeBias = RandomValue * -1;
		}
		else
		{
			NodeBias = 0.0;			//INPUT and OUTPUT Nodes have no bias
		}

		return;

	default:
		cerr << "FATAL ERROR:  Unable to initialize Node Object - NodeInfo: " << nType << " " << NodeID << endl;
	}
}

/************* NN EXECUTION   ***************/

bool Node::Run(const float& InputValue)
{
	//Verify that there is a set of inputs flowing into the node, if not we
	//can do anything
	if ((nType == INPUT) && (!InputValue))
		return false;

	float Neti = 0.0;

	switch (nType)
	{
	case INPUT:
		NodeValue = InputValue;			//Assume only a single input for this type of node
		return true;

	case HIDDEN:
		Neti = CalcNeti();
		Neti += NodeBias;
		NodeValue = (float)(1 / (1 + exp(-1 * Neti)));
		return true;

	case OUTPUT:
		Neti = CalcNeti();
		NodeValue = (float)(1 / (1 + exp(-1 * Neti)));
		return true;
		break;

	default:
		cerr << "ERROR:  For some reason a Node has an invalid NodeID?" << endl;	//Technically should never hit
		return false;
	}
}

/************* NN TRAINING    ***************/

bool Node::CalcDeltas(float Target)
{
	float sum = 0.0;
	switch (nType)
	{
	case INPUT:
		return true;			//Nothing to do here, just return

	case HIDDEN:
		for (int x = 0; x < (int)OutputConnections.size(); x++)
			sum += OutputConnections[x].NodeObj->getWeight(NodeID) * Target;

		NodeDelta = NodeValue * (1 - NodeValue) * sum;

		return true;

	case OUTPUT:
		NodeDelta = NodeValue * (1 - NodeValue) * (Target - NodeValue);
		return true;

	default:
		cerr << "ERROR:  CalcDeltas - Invalid NodeID found" << endl;   //Technically should never hit
		return false;
	}
}

void Node::UpdateWeights(const float ada, const float OutputDelta)
{
	switch (nType)
	{
	case INPUT:
		return;						//Nothing to do, just return

	case HIDDEN:
		for (int x = 0; x < (int)InputConnections.size(); x++)
			InputConnections[x].Weight += ada * InputConnections[x].NodeObj->getNodeValue() * NodeDelta;

		NodeBias += ada * NodeDelta;
		return;

	case OUTPUT:

		for (int x = 0; x < (int)InputConnections.size(); x++)
			InputConnections[x].Weight += ada * InputConnections[x].NodeObj->getNodeValue() * OutputDelta;
		
		return;

	default:
		cerr << "ERROR: For some reason UpdateWeights has an invalid NodeID?" << endl;
	}
}

float Node::getWeight(int NodeID)
{
	for (int x = 0; x < (int)InputConnections.size(); x++)
	{
		if (InputConnections[x].NodeObj->getNodeID() == NodeID)
			return InputConnections[x].Weight;
	}

	cerr << "ERROR:  getWeight called with NodeID not associated with InputConnection" << endl;		//should never hit
	return 0.0;		//default return zero
}

/************* DEBUGGING SUPPORT ************/

void Node::DisplayContents(ostream& os)
{
	os << "****************************************" << endl;
	os << "Node Type: ";
	switch (nType)
	{
	case INPUT:
		os << "INPUT";
		break;
	case HIDDEN:
		os << "HIDDEN";
		break;
	case OUTPUT:
		os << "OUTPUT";
		break;
	default:
		os << "INVALID";
	}

	//TBD - Create a nice output display for generating a visual representation of the node
}

void Node::DisplayConnections(ostream& os)
{
	os << "*****************************************************************" << endl;
	os << "NodeID: " << NodeID << endl;
	os << "Inputs: ";
	for (int x = 0; x < (int)InputConnections.size(); x++)
		os << std::hex << InputConnections[x].NodeObj << " ";

	os << endl;
	os << "Outputs: ";
	for (int x = 0; x < (int)OutputConnections.size(); x++)
		os << std::hex << OutputConnections[x].NodeObj << " ";

	os << endl;
	os << "*****************************************************************" << endl;
}


/*********************************************
***  Class Private Helper Functions **********
**********************************************/

float Node::CalcNeti()
{
	float Neti = 0.0;

	for (int x = 0; x < (int)InputConnections.size(); x++)
		Neti += InputConnections[x].Weight * InputConnections[x].NodeObj->getNodeValue();

	return Neti;
}
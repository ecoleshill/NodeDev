/*
Seneca College - School of Information and Communications Technology
Filename:  NodeDev.cpp
Author:    Dr. Elliott Coleshill
Purpose:   Testing application to verify the coding and functionality of the Node object.
			This code has been written to build the same configuration as the BB_NeuralNetwork
			program written back in June 2011.
*/
#include <iostream>
#include <vector>
#include "Node.h"
#include "../NodeDev/datatype.h"
using namespace std;

//SYSTEM DEFINITIONS
#define NUM_INPUTS		4
#define NUM_HIDDEN		10
#define NUM_OUTPUT		1
#define TRAINING_COUNT	10
#define ADA				0.02			//learning constant

//Function Declarations
void CompTarget(int, float&, float&);
int CompareOtoT(float, float&, float&, int);
float ActivationFunction(float nNeti) { return (1 / (1 + exp(-1 * nNeti))); };
float TrainingFnHidden(float nValue, float nSum, float nTarget) { return (nValue * (1 - nValue) * nSum); };

int main(int argc, char *argv[])
{
	//Vector spaces to hold the Node objects that make up the neural network
	vector<Node>InputNodes;
	vector<Node>HiddenNodes;
	vector<Node>OutputNodes;

	//Create all the nodes required for this implementation
	for (int x = 0; x < NUM_INPUTS; x++)
	{
		Node NewNode(INPUT, x);
		InputNodes.push_back(NewNode);
	}
	for (int x = 0; x < NUM_HIDDEN; x++)
	{
		Node NewNode(HIDDEN, x);
		HiddenNodes.push_back(NewNode);
	}
	for (int x = 0; x < NUM_OUTPUT; x++)
	{
		Node NewNode(OUTPUT, x);
		OutputNodes.push_back(NewNode);
	}

	//Build a fully connected network
	for (int x = 0; x < (int)InputNodes.size(); x++)
	{
		for (int z = 0; z < (int)HiddenNodes.size(); z++)
			InputNodes[x].AddOutput(HiddenNodes[z]);
	}

	for (int x = 0; x < (int)HiddenNodes.size(); x++)
	{
		for (int z = 0; z < (int)InputNodes.size(); z++)
			HiddenNodes[x].AddInput(InputNodes[z]);

		for (int z = 0; z < (int)OutputNodes.size(); z++)
			HiddenNodes[x].AddOutput(OutputNodes[z]);
	}

	for (int x = 0; x < (int)OutputNodes.size(); x++)
	{
		for (int z = 0; z < (int)HiddenNodes.size(); z++)
			OutputNodes[x].AddInput(HiddenNodes[z]);
	}

	//Initalize the network weights and bias values
	for (int x = 0; x < (int)HiddenNodes.size(); x++)
		HiddenNodes[x].Init();

	for (int x = 0; x < (int)OutputNodes.size(); x++)
		OutputNodes[x].Init();

	//Training the Neural Network
	cout << "SYSTEM EVENT:   Training the NN" << endl;
	float Input[4];
	int TrainingCount = 0;
	float Target, IntTarget;	//Training variables

	while (TrainingCount < 35500)
	{
		//Loop through the training dataset
		for (int x = 0; x < 112; x++)
		{
			CompTarget(TrainingData[x][0], Target, IntTarget);		//Get the target output data

			//Get the 4 input values and set the INPUT Nodes of the network
			for (int y = 0; y < NUM_INPUTS; y++)
				Input[y] = (float)TrainingData[x][y + 1];

			//Run the NN
			for (int iCount = 0; iCount < (int)InputNodes.size(); iCount++)
				InputNodes[iCount].Run(Input[iCount], ActivationFunction);

			for (int hCount = 0; hCount < (int)HiddenNodes.size(); hCount++)
				HiddenNodes[hCount].Run(0, ActivationFunction);

			for (int oCount = 0; oCount < (int)OutputNodes.size(); oCount++)
				OutputNodes[oCount].Run(0, ActivationFunction);

			//Compare the output
			CompareOtoT(OutputNodes[0].getNodeValue(), Target, IntTarget, TrainingCount);

			float Output = OutputNodes[0].getNodeValue();			//There is only 1 output node in this network
			float OutputDelta = Output * (1 - Output) * (Target - Output);
			
			//Run CalcDeltas
			for (int hCount = 0; hCount < (int)HiddenNodes.size(); hCount++)
				HiddenNodes[hCount].CalcDeltas(OutputDelta, TrainingFnHidden);

			//Run UpdateWeights
			for (int hCount = 0; hCount < (int)HiddenNodes.size(); hCount++)
				HiddenNodes[hCount].UpdateWeights((float)ADA, OutputDelta);

			for (int oCount = 0; oCount < (int)OutputNodes.size(); oCount++)
				OutputNodes[oCount].UpdateWeights((float)ADA, OutputDelta);

			TrainingCount++;
		}
	}

	return 1;
}

void CompTarget(int OutputValue, float& Target, float& IntTarget)
{
	switch (OutputValue)
	{
	case 0:
		Target = (float)0.49;
		IntTarget = 0;
		break;
	case 1:
		Target = (float)0.50;
		IntTarget = 1;
		break;
	case 2:
		Target = (float)0.51;
		IntTarget = 2;
		break;
	}
}

int CompareOtoT(float Output, float& Target, float& IntTarget, int TrainingCount)
{
	if ((IntTarget == 0) && (Output < 0.500000))
	{
		printf(" -----> MATCH           [%f, %f] ** %d\n", Output, Target, TrainingCount);
		return (1);  //TRUE
	}
	else if ((IntTarget == 2) && (Output > 0.500000) && (Output < 0.505000))
	{
		printf(" -----> MATCH           [%f, %f] ** %d\n", Output, Target, TrainingCount);
		return (1);  //TRUE
	}
	else if ((IntTarget == 1) && (Output > 0.505000))
	{
		printf(" -----> MATCH           [%f, %f] ** %d\n", Output, Target, TrainingCount);
		return (1);  //TRUE
	}
	else
	{
		printf(" -----> NO MATCH        [%f, %f]    %d\n", Output, Target, TrainingCount);
		return (0);  //FALSE
	}
}
/*
Seneca College - School of Information and Communications Technology
Filename:  Functions.h
Author:    Dr. Elliott Coleshill
Purpose:   This header file is designed to hold all training and activation function
           implementation for the Node class to use.  It's pulled out into a separate
		   header file as the Node's can be configured in a distributed style system
*/

float ActivationFunction(float nNeti) 
{
	return (1 / (1 + exp(-1 * nNeti))); 
};


float TrainingFnHidden(float nValue, float nSum, float nTarget) 
{ 
	return (nValue * (1 - nValue) * nSum); 
};

float TrainingFnOutput(float nValue, float nSum, float nTarget)
{
	return (nValue * (1 - nValue) * (nTarget - nValue));
};
/*
 *  RadixSort.cpp
 *  openFrameworks
 *
 *  Created by Aaron Meyers on 5/14/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "RadixSort.h"

RadixSort::RadixSort(int maxNumVals)
{
	this->maxNumVals=maxNumVals;
	orderSwap=new int[maxNumVals];
	valueSwap=new float[maxNumVals];
}
RadixSort::~RadixSort()
{
	delete [] orderSwap;
	delete [] valueSwap;
}

//doesn't sort negative numbers (they are placed last)

//outputOrder should be in order

void RadixSort::sort(	float * input, 
					 int numvals, 
					 int * outputOrder)
{
	if (numvals>maxNumVals) numvals=maxNumVals;
	
	int a;
	Byte * c;
	for (int p=0;p<4;++p)
	{
		memset((void *)counter,0,sizeof(int)*256);
		
		for (a=0;a<numvals;++a)
		{
			c=((Byte *)&input[a])+p;
			counter[*c]++;
		}
		
		offset[0]=0;
		for (a=1;a<256;++a)
		{
			offset[a]=offset[a-1]+counter[a-1];
		}
		
		for (a=0;a<numvals;++a)
		{
			c=((Byte *)&input[a])+p;
			valueSwap[ offset[*c] ]=input[a];
			orderSwap[offset[*c]]=outputOrder[a];
			//outputOrder[ a ]=offset[*c];
			
			//outputOrder[a]=offset[*c];
			
			++offset[*c];
		}
		float * tmp=input;
		input=valueSwap;
		valueSwap=tmp;
		
		int * tmpi=outputOrder;
		outputOrder=orderSwap;
		orderSwap=tmpi;
	}
}

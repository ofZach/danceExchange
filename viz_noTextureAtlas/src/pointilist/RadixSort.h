class RadixSort
{
public:
	RadixSort(int maxNumVals);
	virtual ~RadixSort();
	void sort(	float * input, 
			  int numvals, 
			  int * outputOrder);
	
protected:
	unsigned int counter[256];
	unsigned int offset[256];
	
	float * valueSwap;
	int * orderSwap;
	int maxNumVals;
};
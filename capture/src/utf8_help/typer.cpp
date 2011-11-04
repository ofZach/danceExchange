
#include "typer.h"





int numNonCharacters(string & temp){
	
	int count = 0;
	
	unsigned char * tempp = (unsigned char * ) temp.c_str();
	
	for (int i = 0; i < temp.length(); i++){
		unsigned char *a = tempp + i;
		if ((*a&224)==192) count ++;
		else if ((*a&240)==224) count ++;
	}
	
	return count;
	
	
}


bool bThisKeyAModifier(int test){
	
	unsigned char a = test;
	if ((a&224)==192) return true;
	else if ((a&240)==224) return true;
	return false;
	
}


void typer::setup(){
	
	hmm = "";
	characters.clear();
}




void typer::reset(){
	
	characters.clear();
	hmm = "";
}

void typer::keyPressed (int key){
	
	
	if (key != OF_KEY_RETURN && 
		key != OF_KEY_BACKSPACE){
	
		if ((strlen(hmm.c_str()) - numNonCharacters(hmm)) >= 159){			// some max length. 
			return;
		}
		characters.push_back(  key );
		hmm.push_back((char)(key));
	}
	
	
	if (key == OF_KEY_DEL || 
		key == OF_KEY_BACKSPACE){
		if (characters.size() > 0){
			characters.pop_back();	
		}
		
		if (hmm.size() > 1){
			bool amIaFuckingUtf8Character = (bThisKeyAModifier((int)hmm[hmm.size()-2]));
											 
			if (amIaFuckingUtf8Character){
				hmm.erase(hmm.size() - 2);
			} else {
				hmm.erase(hmm.size() - 1);
			}

		} else if (hmm.size() == 1) {
			hmm.erase(hmm.size() - 1);
		}
	}
	
	if (!bThisKeyAModifier(key)){
		
	
		
	}
	
	
}

string typer::getString(){
	
	return hmm;
}
	







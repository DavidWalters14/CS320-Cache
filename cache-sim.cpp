#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

struct input{
	string behave;
	unsigned long long address;
};
struct cacheline{
	int validbit;
	unsigned long long tag;
};

unsigned int directMap(int cachesize, vector<input> input){
	int cachehits = 0;
	vector<cacheline> cache;
	int cachelines = cachesize/32;
	for(int i = 0 ; i < cachelines ; i++){
		cacheline c;
		c = {0 , 0};
		cache.push_back(c);
	}
	for(int i = 0 ; i < input.size() ; i++){
		/*int index = input[i].address % cachelines;
		unsigned int mask = pow(2,32)-1;
		unsigned int shiftamount = log2(cachelines);
		mask = mask << shiftamount;
		unsigned int indextag = mask & input[i].address;*/
		unsigned long long currindex = (input[i].address >> 5) % (int)pow(2,log2(cachelines));
		int currtag = input[i].address / pow(2,log2(cachelines));
		if(cache[currindex].validbit==1&&cache[currindex].tag==currtag){
			cachehits++;
		}
		else{
			cache[currindex].tag = currtag;
			cache[currindex].validbit = 1;
		}
	}
	return cachehits;
}

int main(int argc, char** argv){
	ifstream infile(argv[1]);
	vector<input> inputs;
  	string behavior;
	unsigned long long target;
	while(infile >> behavior >> std::hex >> target) {
		input i;
		i.behave = behavior;
		i.address = target;
		inputs.push_back(i);
  	}
	cout << directMap(1024,inputs) << endl;
	infile.close();

}

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
	int time;
	int validbit;
	unsigned long long tag;
};

unsigned int directMap(int cachesize, vector<input> input){
	int cachehits = 0;
	vector<cacheline> cache;
	int cachelines = cachesize/32;
	for(int i = 0 ; i < cachelines ; i++){
		cacheline c;
		c = {0, 0 , 0};
		cache.push_back(c);
	}
	for(int i = 0 ; i < input.size() ; i++){
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

unsigned int setAssociate(int associative, vector<input> input){
	vector<cacheline> cache;
	unsigned int cachehits = 0;
	for(int i = 0 ; i < 512 ; i++){
		cacheline c;
		c = {0,0,0};
		cache.push_back(c);
	}
	int indexsize = 512/associative;
	for(int i = 0 ; i < input.size() ; i++){
		unsigned long long currindex = (input[i].address >> 5) % indexsize;
		unsigned int mask = pow(2,32)-1;
		unsigned int currtag = mask & (input[i].address >> (int)(5 + (log2(indexsize))));
		bool placed = false;
		while(currindex<512&&placed==false){
			if(cache[currindex].validbit==0){
				cache[currindex].tag = currtag;
				cache[currindex].validbit = 1;
				cache[currindex].time = i;
				placed = true;
			}
			else if(cache[currindex].validbit==1&&cache[currindex].tag==currtag){
				cachehits++;
				cache[currindex].time = i;
				placed = true;
			}
			currindex+=indexsize;
		}
		unsigned int LRUtime = input.size()+1;
		unsigned int LRUindex = 0;
		if(placed==false){
			unsigned long long currindex = (input[i].address >> 5) % indexsize;
			while(currindex < 512){
				if(cache[currindex].time < LRUtime){
					LRUtime = cache[currindex].time;
					LRUindex = currindex;
				}
				currindex+=indexsize;
			}
			cache[LRUindex].tag = currtag;
			cache[LRUindex].validbit = 1;
			cache[LRUindex].time = i;
		}
	}
	return cachehits;
}

void changeHotCold(int index, vector<int> &hc){
	int mid = 256;
	int start = 0;
	int end = 512;
	while(start < end){
		if(index < mid){
			end = mid;
			hc[mid] = 1;
			mid = mid/2;
		}
		else if(index > mid){
			start = mid+1;
			mid = (start+end)/2;
			hc[index] = 0;
		}
		else{
			return;
		}
	}
}
unsigned int fullAssociative(vector<input> input){
	vector<cacheline> cache;
	unsigned int cachehits = 0;
	for(int i = 0 ; i < 512 ; i++){
		cacheline c;
		c = {0,0,0};
		cache.push_back(c);
	}
	vector<int> hotcold(512, 0);
	for(int i = 0 ; i < input.size() ; i++){
		unsigned int currindex = 0;
		unsigned int currtag = (input[i].address >> 5);
		bool placed = false;
		while(currindex<512&&placed==false){
			if(cache[currindex].validbit==0){
				cache[currindex].tag = currtag;
				cache[currindex].validbit = 1;
				cache[currindex].time = i;
				hotcold[currindex] = 1;
				placed = true;
			}
			else if(cache[currindex].validbit==1&&cache[currindex].tag==currtag){
				cachehits++;
				cache[currindex].time = i;
				placed = true;
			}
			currindex++;
		}
		currindex = 0;
		unsigned int LRUtime = input.size()+1;
		unsigned int LRUindex = 0;
		if(placed==false){
			int startindex = 0;
			int endindex = 511;
			while(startindex!=endindex){
				if(hotcold[endindex/2]==0){
					startindex = endindex/2+1;
				}
				else{
					startindex = endindex/2;
				}
			}
			cache[LRUindex].tag = currtag;
			cache[LRUindex].validbit = 1;
			cache[LRUindex].time = i;
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
	cout << directMap(4096,inputs) << endl;
	cout << directMap(16384,inputs) << endl;
	cout << directMap(32768,inputs) << endl;
	cout << setAssociate(2,inputs) << endl;
	cout << setAssociate(4,inputs) << endl;
	cout << setAssociate(8,inputs) << endl;
	cout << setAssociate(16,inputs) << endl;
		cout << fullAssociative(inputs) << endl;


	infile.close();

}

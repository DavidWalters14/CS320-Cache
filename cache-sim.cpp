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
vector<int> changeHotCold(int index, vector<int> &hc){
	int hcindex = index+511;
	//finding coldest index
	while(hcindex>0){
		int newindex = (hcindex-1)/2;
		if(hcindex%2==0){
			hc[newindex]=1;
			
		}
		else{
			hc[newindex]=0;
		}
		hcindex = newindex;
	}
	return hc;
}


int findLRUhotcold(vector<int> hc){
	int hcindex = 0;
	while(hcindex<511){
		if(hc[hcindex]==0){
			hcindex=(hcindex*2)+2;
		}
		else{
			hcindex = (2*hcindex)+1;		
		}
	}
	hcindex = hcindex-511;
	return hcindex;
}
unsigned int fullAssociative(vector<input> input){
	vector<cacheline> cache;
	unsigned int cachehits = 0;
	for(int i = 0 ; i < 512 ; i++){
		cacheline c;
		c = {0,0,0};
		cache.push_back(c);
	}
	vector<int> hotcold(511, 0);
	for(int i = 0 ; i < input.size() ; i++){
		unsigned int currindex = 0;
		unsigned int currtag = (input[i].address >> 5);
		bool placed = false;
		while(currindex<512&&placed==false){
			if(cache[currindex].validbit==1&&cache[currindex].tag==currtag){
				cachehits++;
				placed = true;
				hotcold = changeHotCold(currindex, hotcold);
			}
			currindex++;
		}
		currindex = 0;
		if(placed==false){
			int bootindex = findLRUhotcold(hotcold);
			//cout << bootindex << endl;
			cache[bootindex].tag = currtag;
			cache[bootindex].validbit = 1;
			hotcold = changeHotCold(bootindex, hotcold);
		}
	}
	return cachehits;
}

int setAssociatewritemiss(int associative, vector<input> input){
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
			if(cache[currindex].validbit==0&&input[i].behave!="S"){
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
		if(placed==false&&input[i].behave!="S"){
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
int setAssociatewithPrefetch(int associative, vector<input> input){
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
		//cout << "done with i" << endl;
		unsigned long long nextindex = ((input[i].address+32) >> 5) % indexsize;
		//cout << "nextindex: " << nextindex << endl;
		unsigned int nextag = mask & ((input[i].address+32) >> (int)(5 + (log2(indexsize))));
		//cout << "nextag: " << nextag << endl;
		bool placed2 = false;
		while(nextindex<512&&placed2==false){
			//cout << "inside while loop of next block" << endl;
			if(cache[nextindex].validbit==0){
				//cout << "inside if" << endl;
				cache[nextindex].tag = nextag;
				cache[nextindex].validbit = 1;
				cache[nextindex].time = i;
				placed2 = true;
			}
			else if(cache[nextindex].validbit==1&&cache[nextindex].tag==nextag){
				//cout << "else if" << endl;
				cache[nextindex].time = i;
				placed2 = true;
			}
			nextindex+=indexsize;
		}
		unsigned int LRUtime2 = input.size()+3;
		unsigned int LRUindex2 = 0;
		if(placed2==false){
			nextindex = ((input[i].address+32) >> 5) % indexsize;
			while(nextindex < 512){
				//cout << "inside of while in LRU" << endl;
				if(cache[nextindex].time < LRUtime2){
					LRUtime2 = cache[nextindex].time;
					LRUindex2 = nextindex;
				}
				nextindex+=indexsize;
			}
			cache[LRUindex2].tag = nextag;
			cache[LRUindex2].validbit = 1;
			cache[LRUindex2].time = i;
		}
	}
	return cachehits;
}
int setAssociatewithPrefetchOnMiss(int associative, vector<input> input){
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
		bool fetch = false;
		while(currindex<512&&placed==false){
			if(cache[currindex].validbit==0){
				cache[currindex].tag = currtag;
				cache[currindex].validbit = 1;
				cache[currindex].time = i;
				fetch = true;
				placed = true;
			}
			else if(cache[currindex].validbit==1&&cache[currindex].tag==currtag){
				cachehits++;
				cache[currindex].time = i;
				placed = true;
			}
			currindex+=indexsize;
		}
		if(placed == false){
			unsigned int LRUtime = input.size()+1;
			unsigned int LRUindex = 0;
			currindex = (input[i].address >> 5) % indexsize;
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
			fetch=true;
		}
		if(fetch==true){//cache miss, prefetch next line and do LRU for initial address
			unsigned long long nextindex = ((input[i].address+32) >> 5) % indexsize;
			unsigned int nextag = mask & ((input[i].address+32) >> (int)(5 + (log2(indexsize))));
			bool placed2 = false;
			while(nextindex<512&&placed2==false){
				if(cache[nextindex].validbit==0){
					//cout << "inside if" << endl;
					cache[nextindex].tag = nextag;
					cache[nextindex].validbit = 1;
					cache[nextindex].time = i;
					placed2 = true;
				}
				else if(cache[nextindex].validbit==1&&cache[nextindex].tag==nextag){
					//cout << "else if" << endl;
					cache[nextindex].time = i;
					placed2 = true;
				}
				nextindex+=indexsize;
			}
			unsigned int LRUtime2 = input.size()+3;
			unsigned int LRUindex2 = 0;
			if(placed2==false){
				nextindex = ((input[i].address+32) >> 5) % indexsize;
				while(nextindex < 512){
					//cout << "inside of while in LRU" << endl;
					if(cache[nextindex].time < LRUtime2){
						LRUtime2 = cache[nextindex].time;
						LRUindex2 = nextindex;
					}
					nextindex+=indexsize;
				}
				cache[LRUindex2].tag = nextag;
				cache[LRUindex2].validbit = 1;
				cache[LRUindex2].time = i;
			}
		}
	}
	return cachehits;
}
int main(int argc, char** argv){
	ifstream infile(argv[1]);
	ofstream output(argv[2]);
	vector<input> inputs;
  	string behavior;
	unsigned long long target;
	while(infile >> behavior >> std::hex >> target) {
		input i;
		i.behave = behavior;
		i.address = target;
		inputs.push_back(i);
  	}
	output << directMap(1024,inputs) << ","<< inputs.size()<<"; " << directMap(4096,inputs) << "," << inputs.size() <<"; " << directMap(16384,inputs) <<"," << inputs.size() <<"; " << directMap(32768,inputs) << "," << inputs.size() <<";"<<endl;
	output << setAssociate(2,inputs) << ","<< inputs.size()<<"; " << setAssociate(4,inputs)<< "," << inputs.size() <<"; " << setAssociate(8,inputs) <<"," << inputs.size() <<"; " << setAssociate(16,inputs) << "," << inputs.size() <<";"<<endl;
	output << setAssociate(512,inputs) << "," << inputs.size() << ";"<< endl;
	output << fullAssociative(inputs) <<"," <<inputs.size() <<";" << endl;
	output << setAssociatewritemiss(2,inputs) << ","<< inputs.size()<<"; " << setAssociatewritemiss(4,inputs)<< "," << inputs.size() <<"; " << setAssociatewritemiss(8,inputs) <<"," << inputs.size() <<"; " << setAssociatewritemiss(16,inputs) << "," << inputs.size() <<";"<<endl;
	output << setAssociatewithPrefetch(2,inputs) << ","<< inputs.size()<<"; " << setAssociatewithPrefetch(4,inputs)<< "," << inputs.size() <<"; " << setAssociatewithPrefetch(8,inputs) <<"," << inputs.size() <<"; " << setAssociatewithPrefetch(16,inputs) << "," << inputs.size() <<";"<<endl;
	output << setAssociatewithPrefetchOnMiss(2, inputs) << ","<< inputs.size()<<"; " << setAssociatewithPrefetchOnMiss(4, inputs)<< "," << inputs.size() <<"; " << setAssociatewithPrefetchOnMiss(8, inputs) <<"," << inputs.size() <<"; " << setAssociatewithPrefetchOnMiss(16, inputs) << "," << inputs.size() <<";"<<endl;
	
	infile.close();
	output.close();
}

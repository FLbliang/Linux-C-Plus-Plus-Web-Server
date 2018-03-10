#include <iostream>
#include <unistd.h>
#include <boost/algorithm/string.hpp>
#include <cstring>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;
using namespace boost::algorithm;

int main(int argc, char * argv[]){

	if(argc < 4){
		perror("error");
		exit(1);
	}

	vector<string> keys;
	vector<string> values;
	split(keys, argv[2], is_any_of("&"));	
	split(values, argv[3], is_any_of("&"));
	int len = keys.size();

	if(strcmp(argv[1], "GET") == 0){
		cout << "<h1>Hello, world!</h1>" << endl;
		for(int i = 0; i < len; ++i){
			cout << "<strong style='color:red;font-weight:900;'>&nbsp;" << keys[i] << "&nbsp;</strong>";
			cout << "<span style='color:green;font-weight:700;'>&nbsp;" << values[i] << "&nbsp;</span><br/>";
		}
			
	}else if(strcmp(argv[1], "POST") == 0){
		
		vector<string> questions;
		questions.push_back("My Name is: ");
		questions.push_back("My Favourite Song is: ");
		
		cout << "<h1>Hello, My friend!</h1>";
		vector<string> val;
		for(int i = 0; i < len; ++i){
			cout << "<strong style='color:orangered;font-weight:900'>" << questions[i] << "</strong>";
			split(val, values[i], is_any_of("+"));
			cout << "<span style='color:green; font-weight:700;'>&nbsp;"; 
			for(auto & v : val){
				cout << v << " ";
			}
			cout << "</span><br />";

		}
	}

	fflush(stdout);

	return 0;

}

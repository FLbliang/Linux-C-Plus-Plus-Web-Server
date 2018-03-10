#ifndef __SETTING__
#define __SETTING__

#include <map>
#include <string>

using namespace std;

class Setting{
	public:
		map<string, string> url_get_handle_from;
		map<string, string> url_post_handle_from;

		explicit Setting(){
			url_get_handle_from["/index.html"] = "index";
			url_post_handle_from["/index"] = "index";
		}

};

#endif

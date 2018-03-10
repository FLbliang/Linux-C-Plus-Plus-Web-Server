#ifndef __DAEMOND__
#define __DAEMOND__

class Daemond{

public:

	Daemond(const char * dir, const char * file_name);
	Daemond();
	
	inline void set_dir(const char * dir){ this->dir = dir; }
	inline void set_fileName(const char * file_name){ this->file_name = file_name; }
	void create_daemond();

private:
	
	const char * dir;
	const char * file_name;


};

#endif

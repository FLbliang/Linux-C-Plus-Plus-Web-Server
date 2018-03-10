#ifndef __TASK__
#define __TASK__

#include <map>
#include <list>
#include <string>
#include <vector>

#include "setting.h"

using namespace std;

class Task{
	public:
		Task();
		void set_fd(const int client_fd){ m_client_fd = client_fd; }
		void handle_request(const char * str);
		void response_send();	
		int get_isFork(){ return m_is_fork; }

	private:
		void response_get(const string & url); //响应 GET 请求
		void response_post(const string & url, const string & args);  // 响应 POST 请求
		void response_file(); // 响应返回静态文件
		void set_content(const char * content_type, const int len);// 封装响应头发送响应信息
		void explain_args(const vector<string> & strs); // 解析请求的参数
		void set_argv(char argv[][BUFSIZ]); // 封装传递到CGI程序的参数
		void execl_cgi(); // 执行CGI程序
		void set_error();

	private:
		char m_buf[4096];
		char m_head_buf[1024];
		bool m_send_file = false;
		bool m_is_fork = false;
		int m_status;
		string m_method;
		string m_file_type;
		string m_send_filename;
		string m_error_file = "404.html";
		list<string> m_keys; // 保存参数的键
		map<string, string> m_values; // 参数映射的值
		int m_client_fd = 0;
		const char * m_static_dir = "./static";
		map<string, const char *> m_content_type;	

		Setting m_setting;


};



#endif

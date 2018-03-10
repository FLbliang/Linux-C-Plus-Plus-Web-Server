#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <map>
#include <list>

#include "task.h"

using namespace std;
using namespace boost::algorithm;

Task::Task(){
	m_content_type["html"] = "text/html; charset=utf-8";
	m_content_type["js"] = "application/javascript";
	m_content_type["css"] = "text/css";
	m_content_type["jpg"] = "image/jpeg";
	m_content_type["png"] = "image/png";
}

// 处理客户请求 (接口)
void Task::handle_request(const char * str){

	string msg = str;		
	vector<string> strs;
	split(strs, msg, is_any_of(" \n"));	
	int len = strs.size();
	if(len < 5){
		set_error();
		return;
	}

	m_method = strs[0];
	
	if(m_method.compare("GET") == 0){
		response_get(strs[1]);	
	}else if(m_method.compare("POST") == 0){
		response_post(strs[1], strs[len-1]);
	}else{
		set_error();
	}

}

void Task::set_error(){
	m_status = 404;
	m_send_filename = m_static_dir;
	m_send_filename += "/";
	m_send_filename += m_error_file;
}

void Task::set_argv(char argv[][BUFSIZ]){

	if(argv == NULL){
		return;
	}

	bool begin_flag = false;
	for(auto & key : m_keys){
		if(begin_flag){
			sprintf(argv[0], "%s&%s", argv[0], key.data());
			sprintf(argv[1], "%s&%s", argv[1], m_values[key].data());
		}else{
			strcpy(argv[0], key.data());
			strcpy(argv[1], m_values[key].data());
			begin_flag = true;
		}
	}
}

void Task::execl_cgi(){
	// 带参数的请求, 执行响应的CGI程序
	pid_t pid = fork();
	if(pid == -1){
		return;
	}
	if(pid == 0){
		char argv[2][BUFSIZ] = {"", ""};
		set_argv(argv);
		dup2(m_client_fd, STDOUT_FILENO);
		string cgi = "CGI/";

		const char * file_name = (m_method.compare("GET") == 0 ? 
					string(cgi + m_setting.url_get_handle_from[m_send_filename]).data()
						:
					string(cgi + m_setting.url_post_handle_from[m_send_filename]).data());

		execl(file_name, file_name, m_method.data(), argv[0], argv[1]);

	}else{
		wait(NULL);
	}
}

// 发送响应数据 (接口)
void Task::response_send(){
		
	// 状态被设置为404，直接返回404文件
	if(m_status == 404){
		response_file();		
	}else if(m_method.compare("GET") == 0){

		if(m_send_file){
			response_file();
			return;
		}

		execl_cgi();

	}else if(m_method.compare("POST") == 0){
		execl_cgi();	

	}else{
		set_error();
		response_file();
	}

}

// 响应 GET 请求
void Task::response_get(const string & url){
	int len = url.length();
	vector<string> strs;
	vector<string> file_type;
	split(strs, url, is_any_of("?&")); 
	string file_name = strs[0].compare("/") == 0 ? "/index.html" : strs[0];
	len = strs.size();
	//cout << "file_name: " << file_name << endl;
	printf("GET request %s\n", file_name.data());

	// 如果请求的是不带参数的文件且文件不存在, 返回404文件
	if(len <= 1 && access(string(m_static_dir + file_name).data(), F_OK) != 0){
		set_error();	
		return;
	}

	split(file_type, file_name, is_any_of("."));
	if(file_type.size() < 2){
		set_error();
		return;
	}

	m_file_type = file_type[file_type.size()-1];

	m_status = 200;
	m_send_filename = file_name;
	if(len == 1){
		m_send_file = true;
		return;
	}
	m_send_file = false;

	// 带参数的GET请求, 保存文件名和参数
	explain_args(vector<string>(strs.begin()+1, strs.end())); 

}

// 响应 POST 请求
void Task::response_post(const string & url, const string & args){
	// 解析参数
	m_status = 200;
	m_send_filename = url;
	printf("POST request %s\n", m_send_filename.data());
	vector<string> strs;
	split(strs, args, is_any_of("&"));
	explain_args(strs);	
}

// 解析请求参数
void Task::explain_args(const vector<string> & strs){

	m_keys.clear();
	m_values.clear();
	vector<string> key_value;
	for(auto & str : strs){
		split(key_value, str, is_any_of("="));
		if(key_value.size() != 2){
			set_error();
			return;
		}
		m_keys.push_back(key_value[0]);
		m_values[key_value[0]] = key_value[1];
	}
}

// 响应返回静态文件
void Task::response_file(){
	m_send_filename = m_static_dir + m_send_filename;
	if(m_status == 404 && access(m_send_filename.data(), F_OK) != 0){
		char body[BUFSIZ];
		strcpy(body, "<!DOCTYPE html><html>");
		sprintf(body, "%s<head><meta charset='utf-8' /><meta http-equiv='X-UA-Compatible' content='IE=edge'>", body);
		sprintf(body, "%s<title>404 Error</title></head>", body);
		sprintf(body, "%s<body><h1>The file is not found!</h1></body></html>", body);
		set_content(m_content_type["html"], strlen(body)); 
		sprintf(m_buf, "%s%s", m_head_buf, body);
		write(m_client_fd, m_buf, strlen(m_buf));
	}else{

		struct stat file_stat;
		const char * file_name = m_send_filename.data();
		int ret = stat(file_name, &file_stat);
		if(ret < 0){
			return;
		}
		int file_fd = open(file_name, O_RDONLY);
		set_content(m_content_type[m_file_type.data()], file_stat.st_size);	
		write(m_client_fd, m_head_buf, strlen(m_head_buf));
		sendfile(m_client_fd, file_fd, 0, file_stat.st_size);
		close(file_fd);
		
	}
}

// 封装 HTTP 响应头发送响应信息
void Task::set_content(const char * content_type, const int len){

//	cout << "content_type: " << content_type << endl;
	sprintf(m_head_buf, "HTTP/1.1 %d OK\r\n", m_status);
	sprintf(m_head_buf, "%sAccept-Ranges: bytes\r\n", m_head_buf);
	sprintf(m_head_buf, "%sContent-Type: %s\r\n", m_head_buf, content_type);
	sprintf(m_head_buf, "%sContent-Length: %d\r\n\r\n", m_head_buf, len);
}


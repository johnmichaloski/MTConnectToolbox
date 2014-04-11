#ifndef URL_HH_
#define URL_HH_    
#include <string>
#include <map>
#include <vector>
struct Url {
    Url(const std::string& url_s); // omitted copy, ==, accessors, ...
    std::string url_s, _protocol, _host, _path, _query;
	std::map<std::string,std::string> queries;
private:
    void parse(const std::string& url_s);
};
#endif /* URL_HH_ */
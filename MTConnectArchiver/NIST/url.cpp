//
//  Url.cpp
//

/** ...
    url u("HTTP://stackoverflow.com/questions/2616011/parse-a.py?url=1");
    cout << u.protocol() << '\t' << u.host() << ..
*/

#include "stdafx.h"
#include "url.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>
using namespace std;

#include "StdStringFcn.h"

// ctors, copy, equality, ...
Url::Url(const std::string& url_s)
{
	this->url_s=url_s;
	parse(url_s);
}
void Url::parse(const string& url_s)
{
    const string prot_end("://");
    string::const_iterator prot_i = search(url_s.begin(), url_s.end(),
                                           prot_end.begin(), prot_end.end());
    _protocol.reserve(distance(url_s.begin(), prot_i));
    transform(url_s.begin(), prot_i,
              back_inserter(_protocol),
              ptr_fun<int,int>(tolower)); // protocol is icase
    if( prot_i == url_s.end() )
        return;
    advance(prot_i, prot_end.length());
    string::const_iterator path_i = find(prot_i, url_s.end(), '/');
    _host.reserve(distance(prot_i, path_i));
    transform(prot_i, path_i,
              back_inserter(_host),
              ptr_fun<int,int>(tolower)); // host is icase
    string::const_iterator query_i = find(path_i, url_s.end(), '?');
    _path.assign(path_i, query_i);
    if( query_i != url_s.end() )
        ++query_i;
    _query.assign(query_i, url_s.end());

	std::vector<std::string> parts = Tokenize(_query,"&");
	for(int i=0; i< parts.size(); i++)
	{
		std::vector<std::string> tokens = Tokenize(parts[i],"=");
		if(tokens.size() < 1) 
			continue;
		queries[tokens[0]]=tokens[1];
	}
}
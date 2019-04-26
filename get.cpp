#include <iostream>
#include <string>
#include <set>
#include <regex>
#include <curl/curl.h>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::set;
using std::vector;
using std::smatch;
using std::regex;
using std::regex_search;

size_t curl_to_string(void *ptr, size_t size, size_t nmemb, void *data)
{
	string *str = (string *) data;
	char *sptr = (char *) ptr;
	for (unsigned int x = 0; x < size * nmemb; x++)
		(*str) += sptr[x];
	return size * nmemb;
}

string perform(string url)
{
	CURL *curl;
	CURLcode res;
	string page;
	curl = curl_easy_init();
	if (curl) {
		cout << "Request to: " << url << " ";
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			cout << "Failed." << endl;
		else
			cout << "Success!" << endl;
		curl_easy_cleanup(curl);
	}
	return page;
}

vector<string> findLinks(string page)
{
	regex expr("http[s]?://[\\w\\.\\?\\$\\-\\+#/%=]+");
	smatch m;
	vector<string> links;
	while (regex_search(page, m, expr)) {
		for (auto x : m)
			links.push_back(x);
		page = m.suffix().str();
	}
	return links;
}

set<string> getSet(string user_url, int level)
{
	set<string> url_set { user_url };
	vector<string> urls { user_url };
	for (int i = 0; i < level; i++) {
		cout << "---------------------------" << endl;
		cout << "Level: "<< i << endl;
		cout << "---------------------------" << endl;
		vector<string> new_urls, cur_urls;
		for (string url : urls) {
			if (url_set.find(url) == url_set.end())
				continue;
			cur_urls = findLinks(perform(url));
			for (string url : cur_urls) {
				url_set.insert(url);
				new_urls.push_back(url);
			}
		}
		urls = new_urls;
	}
	return url_set;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "Too few arguments. Usage:\n./get 5\nwhere 5 is depth." << endl;
		return 0;
	}
	int level = atoi(argv[1]);
	cout << "Input web address to start:" << endl;
	string url;
	cin >> url;
	set<string> url_set = getSet(url, level);
	for (string url : url_set)
		cout << url << endl;
	return 0;
}

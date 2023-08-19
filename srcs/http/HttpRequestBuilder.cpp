#include "HttpRequestBuilder.hpp"

HttpRequestBuilder::HttpRequestBuilder()
{
	this->method_type = METHOD_TYPE_NONE;
	this->build_step = BUILD_FIRST;
	this->path = "";
	this->http_version = "";
	this->content_length = 0;
	this->is_chunked = false;
	this->body = "";
}

HttpRequestBuilder::~HttpRequestBuilder()
{
}

void HttpRequestBuilder::setBuildStep(HttpRequestBuilderBuildStep build_step)
{
	this->build_step = build_step;
}

void HttpRequestBuilder::erase(void)
{
	this->method_type = METHOD_TYPE_NONE;
	this->build_step = BUILD_FIRST;
	this->path = "";
	this->http_version = "";
	this->headers.erase(this->headers.begin(), this->headers.end());
	this->content_length = 0;
	this->is_chunked = false;
	this->body = "";
}

HttpRequestBuilder::HttpRequestBuilderBuildStep HttpRequestBuilder::getBuildStep(void)
{
	return this->build_step;
}

void HttpRequestBuilder::setMethodType(HttpMethodType method_type)
{
	this->method_type = method_type;
}
HttpRequestBuilder::HttpMethodType HttpRequestBuilder::getMethodType(void)
{
	return this->method_type;
}
void HttpRequestBuilder::setPath(string path)
{
	this->path = path;
}
string HttpRequestBuilder::getPath(void)
{
	return this->path;
}
void HttpRequestBuilder::setHttpVersion(string http_version)
{
	this->http_version = http_version;
}
string HttpRequestBuilder::getHttpVersion(void)
{
	return this->http_version;
}
int HttpRequestBuilder::getContentLength(void)
{
	return this->content_length;
}
bool HttpRequestBuilder::getIsChunked(void)
{
	return this->is_chunked;
}
void HttpRequestBuilder::setBody(string body)
{
	this->body = body;
}
void HttpRequestBuilder::appendBody(string body)
{
	this->body += body;
}
string HttpRequestBuilder::getBody(void)
{
	return this->body;
}

string HttpRequestBuilder::getMethod(const HttpMethodType & method_type) const
{
	switch(method_type) {
		case 0:
			return "NONE";
		case 1:
			return "GET";
		case 2:
			return "POST";
		case 3:
			return "PUT";
		case 4:
			return "HEAD";
		case 5:
			return "DELETE";
		case 6:
			return "OPTIONS";
		case 7:
			return "PATCH";
		case 8:
			return "TRACE";
	}
}

bool HttpRequestBuilder::buildFirstLine(string str, bool check_only)
{
	pair<string, HttpMethodType> map_data[] = {
		make_pair("GET", GET),
		make_pair("POST", POST),
		make_pair("PUT", PUT),
		make_pair("HEAD", HEAD),
		make_pair("DELETE", DELETE),
		make_pair("OPTIONS", OPTIONS),
		make_pair("PATCH", PATCH),
		make_pair("TRACE", TRACE),
	};
	map<string, HttpMethodType> http_methods(map_data, map_data + sizeof(map_data) / sizeof(map_data[0]));
	
	int now = 0;
	HttpMethodType method_type = METHOD_TYPE_NONE;
	for (map<string, HttpMethodType>::iterator it = http_methods.begin(); it != http_methods.end(); it++) {
		if (str.compare(0,it->first.length(), it->first) == 0) {
			now += it->first.length();
			method_type = it->second;
			break;
		}
	}
	if (now == 0) {
		// cout << "method is invalid." << endl;
		return false;
	}
	if (str[now] == ' ') {
		now++;
	}
	else {
		// cout << "next method, 1 space is needed." << endl;
		return false;
	}
	int http_idx = str.find("HTTP/", now);
	if (http_idx == string::npos) {
		// cout << "HTTP/ is not exist." << endl;
		return false;
	}
	else if (http_idx == now) {
		// cout << "path is not exist." << endl;
		return false;
	}

	if (str[http_idx-1] != ' ') {
		// cout << "next path, 1 space is needed." << endl;
		return false;
	}
	string path = str.substr(now, http_idx-now-1);
	for (int i = 0; i < path.length(); i++) {
		if (path[i] < 33 || path[i] > 126) {
			// cout << "path must printable." << endl;
			return false;
		}
	}

	now = http_idx + 5;  // skip "HTTP/"
	if (now == str.length()) {
		// cout << "HTTP version is not exist." << endl;
		return false;
	}
	string http_version = str.substr(now, str.length()-now);
	if (http_version.length() != 3) {
		// cout << "HTTP version length must be 3." << endl;
		return false;
	}
	else if (!(isdigit(http_version[0]) && isdigit(http_version[2]) && http_version[1] == '.')) {
		// cout << "HTTP version format must be X.X (X is one digit)." << endl;
		return false;
	}

	if (!check_only) {
		this->setMethodType(method_type);
		this->setPath(path);
		this->setHttpVersion(http_version);
		this->setBuildStep(BUILD_HEADER);
	}
	return true;
}


bool HttpRequestBuilder::setHeader(string str, bool check_only)
{
	
	size_t idx = str.find(':');

	// if (!check_only) cout << "SETHEADER@@@@@@@@@@@@" << str << endl;
	// if (idx == -1 || str.find(":", idx+1) != -1) {
	// 	// cout << "delimeter : is invalid." << endl;
	// 	return false;
	// }

	if (idx == string::npos) {
		// cout << "delimeter : is invalid." << endl;
		return false;
	}

	string key = str.substr(0, idx);
	bool is_valid_key = true;
	for (size_t i = 0; i < key.length(); i++) {
		if (!(key[i] >= 33 && key[i] <= 126) || key[i] == ':') {
			is_valid_key = false;
			break;
		}
	}
	if (!key.length() || !is_valid_key) {
		// cout << "key " << key << " is invalid." << endl;
		return false;
	}

	// : 이후 공백 skip
	idx++;
	while (idx < str.length() && isspace(str[idx])) {
		idx++;
	}

	string value = str.substr(idx, str.length());
	if (!value.length()) {
		// cout << "value " << value << " is invalid." << endl;
		return false;
	}
	// first line의 경우 반드시 공백 문자가 1개 이상 필요한데, key의 경우 공백문자 포함될 경우 이미 이전에 필터링됨
	// 때문에 key에 대해서는 first line인지 다시 확인할 필요가 없음
	for (int i = 0; i < value.length(); i++) {
		if (this->buildFirstLine(value.substr(i, value.length()), true)) {
			// cout << "value " << value << " has http first header line." << endl;
			return false;
		}
	}

	if (!check_only) {
		this->headers[key] = value;
		// this->headers[key] = Utils::toLowerCase(value);

		string upper_key = key;
		for (int i = 0; i < upper_key.length(); i++) {
			upper_key[i] = toupper(upper_key[i]);
		}

		if (!upper_key.compare("CONTENT-LENGTH")) { // content_length 갱신
			if (this->is_chunked) {  // is_chunked인 경우 Content-Length header는 없어야 함
				return false;
			}
			bool correct = true;
			for (int i = 0; i < value.length(); i++) {
				if (!isdigit(value[i])) {
					correct = false;
					break;
				}
			}
			if (correct) {
				this->content_length = atoi(value.c_str());
			}
		}
		else if (!upper_key.compare("TRANSFET-ENCODING")) {  // is_chunked 갱신
			if (this->content_length > 0) {  // is_chunked인 경우 Content-Length header는 없어야 함
				return false;
			}
			if (!value.compare("chunked")) {
				this->is_chunked = true;
			}
		}
	}

	return true;
}

string HttpRequestBuilder::getHeader(string key)
{
	return this->headers[key];
}

void HttpRequestBuilder::print(void) {
	cout << "BuildStep: " << this->build_step << endl;
	cout << "Method: " << this->method_type << endl;
	cout << "Path: " << this->path << endl;
	cout << "Version: " << this->http_version << endl;
	cout << "isChunked: " << this->is_chunked << endl;

	cout << endl << "***Headers***" << endl;
	for (map<string,string>::iterator it = this->headers.begin(); it != this->headers.end(); it++) {
		cout << it->first << ": " << it->second << endl;
	}

	cout << endl << "***Body*** (Content-Length: " << this->content_length << ")" << endl;
	cout << this->body << endl;
}

// return value
// -1: garbarage request가 생성되는 경우 (request는 초기화됨)
//  0: 온전한 request가 완성된 경우
//  1: 아직 판단이 불가능한 경우 (다음 호출까지 확인해봐야 하는 경우)
int HttpRequestBuilder::isHttp(string &recv_buf)
{
	vector<string> lines = split(recv_buf, "\r\n");
	if (!lines.size()) {  // \r\n이 recv_buf에 등장하지 않은 경우에 대한 예외처리
		string s(recv_buf);
		lines.push_back(s); // lines가 비어있기 때문에 일단 집어 넣음
	}

	int lines_index = -1;  // first line 직후의 lines index
	if (getBuildStep() == BUILD_FIRST) {  // first line부터 완성해야 하는 경우
		for (int i = 0; i < lines.size()-1; i++) {
			for (int j = 0; j < lines[i].length(); j++) { // 쓰레기 byte가 first line의 앞에 있는 경우를 고려해 이를 skip
				if (buildFirstLine(lines[i].substr(j, lines[i].length()))) {
					break;
				}
			}
			if (getMethodType() != METHOD_TYPE_NONE) {  // first line을 찾은 경우
				lines_index = i + 1;
				break;
			}
		}
		if (lines_index == -1) {  // first line이 없는 경우
			erase();  // 기존에 완성하던 request는 버림
			recv_buf = lines[lines.size()-1]; // \r\n으로 끝나는 모든 line들 (lines[:-1])은 이미 first line 포맷이 아님을 확인했기에 배제, 마지막 line(lines[-1])만 recv_buf에 남김
			cout << "[RETURN -1] first line doesn't exist." << endl;
			return -1;
		}
	}
	else {
		lines_index = 0;
	}
	cout << "START_LINE: " << lines_index << endl;

	HttpMethodType method_type = getMethodType();

	bool body_required = (method_type == POST || method_type == PUT || method_type == PATCH);
	string body = "";
	int chunked_number = -1; // chunked_number == -1 => 이번에 chunked number가 등장할거다
	for (int i = lines_index; i < lines.size()-1; i++) {
		if (lines[i].length() == 0) {  // empty line, header finish
			cout << "EMPTY LINE" << i << endl;
			if (getBuildStep() == BUILD_HEADER && body_required) {  // header를 완성한 뒤 body로 넘어가는 경우
				setBuildStep(BUILD_BODY);
			}
			else if (getBuildStep() == BUILD_BODY && body_required) {  // 이미 body를 완성중이었던 경우
				if (getIsChunked()) {
					return buildChunkedBody(recv_buf, body, lines, i);
				}
				else {
					body += "\r\n";
				}
			}
			else if (!body_required) {  // body가 필요없는 method인 경우
				recv_buf = Utils::str_join(lines, "\r\n", i+1);  // recv_buf에 다음 line부터 추가
				cout << "[RETURN 0] header is finished. (body is not required.)" << endl;
				return 0;  // 온전한 request 완성 (body 없는 method)
			}
			continue;
		}

		if (getBuildStep() == BUILD_BODY) {  // body를 완성중이던 경우
			if (getIsChunked()) {
				if (chunked_number == -1) {  // chunked number가 나올 차례인 경우
					if (Utils::is_digit_string(lines[i])) {
						chunked_number = stoi(lines[i], 0, 16);
						if (chunked_number == 0) {  // chunked number가 0이면 request가 끝이라는 의미
							return buildChunkedBody(recv_buf, body, lines, i+1);  // return 0
						}
					}
					else {  // chunked number가 나올 차례인데 다른 문자열이 나온 경우, 이전까지의 body로 request가 끝났다고 가정
						return buildChunkedBody(recv_buf, body, lines, i);  // return -1
					}
				}
				else {  // chunked string이 나올 차례인 경우
					if (lines[i].length() != chunked_number) {
						erase();
						recv_buf = Utils::str_join(lines, "\r\n", i);  // recv_buf에 현재 line부터 추가
						cout << "[RETURN -1] chunked length mismatch: " << chunked_number << " vs " << lines[i].length() << endl;
						return -1;
					}
					else {
						body += lines[i];
						chunked_number = -1;
					}
				}
			}
			else {
				body += lines[i] + "\r\n";
			}
		}
		else if (getBuildStep() == BUILD_HEADER) {  // header를 완성중이던 경우
			if (!setHeader(lines[i])) {  // header 차례인데 유효하지 않은 header가 나온 경우
				erase();  // 기존에 완성하던 request는 버림
				recv_buf = Utils::str_join(lines, "\r\n", i);  // recv_buf에 현재 line부터 추가
				cout << "[RETURN -1]  invalid header." << endl;
				return -1;
			}
		}
	}

	if (body_required && getBuildStep() == BUILD_BODY) {  // 마지막 line까지 포함해 body 완성 후 content-length와 비교
		if (getIsChunked()) {  // chunked인 경우 마지막 line을 recv_buf에 남긴 뒤에 이후 다시 판단
			appendBody(body);
			recv_buf = "";
			if (chunked_number != -1) {
				recv_buf += to_string(chunked_number) + "\r\n";
			}
			recv_buf += lines[lines.size()-1];
			cout << "[RETURN 1] chunked is not finished." << endl;
			return 1;
		}
		else {
			body += lines[lines.size()-1];

			if (getContentLength() <= 0) {  // 유효하지 않은 content_length인 경우
				erase();  // 기존에 완성하던 request는 버림
				recv_buf = body;  // 그동안 모았던 body를 모두 잔여물로 취급해 recv_buf에 담음
				cout << "[RETURN -1] invalid content_length: " << endl;
				return -1;
			}
			else if (body.length() < getContentLength()) {  // content-length까지 body가 완성되지 않은 경우
				recv_buf = body;  // recv_buf를 body로 덮어씌움
				cout << "[RETURN 1] body is not sufficient." << endl;
				return 1;
			}
			else {  // body가 충분히 모인 경우
				setBody(body.substr(0, getContentLength()));  // 완성된 body를 content-length만큼 slicing해 request_buf에 추가
				recv_buf = body.substr(getContentLength(), body.length()-getContentLength());  // 잔여물 body는 recv_buf에 덮어씌움
				cout << "[RETURN 0] body is sufficient." << endl;
				return 0;
			}
		}
	}
	else {  // 마지막 line (\r\n으로 끝나지 않음)은 잔여물로 취급해 recv_buf에 저장
		recv_buf = lines[lines.size()-1];
		cout << "[RETURN 1] header is not finished." << endl;
		return 1;
	}

	cout << "[RETURN 0] last return." << endl;
	return 0;
}

// chunkedBody를 완성하는 함수
// body의 길이에 따라 실패(body_length=0)/성공(body_length>0) 판단해 return (실패: -1, 성공: 0)
// recv_buf는 "\r\n".join(lines[start_idx:])로 갱신됨
int HttpRequestBuilder::buildChunkedBody(string &recv_buf, string &body, vector<string> &lines, int start_idx)
{
	if (getBody().length() == 0 && body.length() == 0) {
		erase();
		recv_buf = Utils::str_join(lines, "\r\n", start_idx);
		cout << "[RETURN -1] chunked body empty" << endl;
		return -1;
	}
	else {
		appendBody(body);
		recv_buf = Utils::str_join(lines, "\r\n", start_idx);
		cout << "[RETURN 0] chunked body success" << endl;
		return 0;
	}
}

vector<string> HttpRequestBuilder::split(const string& s, const string& delim)
{
	vector<string> result;
    size_t start = 0;
    size_t end = s.find(delim);
    while (end != string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + delim.size();
        end = s.find_first_of(delim, start);
        if (end == string::npos) {
			if (start >= s.length()) {
				result.push_back("");
			}
			else {
				result.push_back(s.substr(start, s.length()));
			}
            break;
        }
    }
    return result;
}

HttpRequestMessage *HttpRequestBuilder::createRequestMessage()
{
	HttpRequestMessage *requestMessage;
	string serverProtocol = "HTTP/" + http_version;
	string method = getMethod(method_type);
	string requestbody = body;
	requestMessage = new HttpRequestMessage(method, path, serverProtocol, headers, body);
	erase();
	return requestMessage;
}
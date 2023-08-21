#include "HttpRequestBuilder.hpp"

using namespace std;

HttpRequestBuilder::HttpRequestBuilder()
{
	this->methodType = METHOD_TYPE_NONE;
	this->buildStep = BUILD_FIRST;
	this->path = "";
	this->httpVersion = "";
	this->contentLength = 0;
	this->isChunked = false;
	this->body = "";
	this->needMoreChunk = false;
}

HttpRequestBuilder::~HttpRequestBuilder()
{
}

void HttpRequestBuilder::setBuildStep(HttpRequestBuilderBuildStep buildStep)
{
	this->buildStep = buildStep;
}

void HttpRequestBuilder::erase(void)
{
	this->methodType = METHOD_TYPE_NONE;
	this->buildStep = BUILD_FIRST;
	this->path = "";
	this->httpVersion = "";
	this->headers.erase(this->headers.begin(), this->headers.end());
	this->contentLength = 0;
	this->isChunked = false;
	this->body = "";
	this->needMoreChunk = false;
}

HttpRequestBuilder::HttpRequestBuilderBuildStep HttpRequestBuilder::getBuildStep(void)
{
	return this->buildStep;
}

void HttpRequestBuilder::setMethodType(HttpMethodType methodType)
{
	this->methodType = methodType;
}

HttpRequestBuilder::HttpMethodType HttpRequestBuilder::getMethodType(void)
{
	return this->methodType;
}

void HttpRequestBuilder::setPath(string path)
{
	this->path = path;
}

string HttpRequestBuilder::getPath(void)
{
	return this->path;
}

void HttpRequestBuilder::setHttpVersion(string httpVersion)
{
	this->httpVersion = httpVersion;
}

string HttpRequestBuilder::getHttpVersion(void)
{
	return this->httpVersion;
}

size_t HttpRequestBuilder::getContentLength(void)
{
	return this->contentLength;
}

bool HttpRequestBuilder::getIsChunked(void)
{
	return this->isChunked;
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

string HttpRequestBuilder::getMethod(const HttpMethodType &methodType) const
{
	switch(methodType)
	{
		case METHOD_TYPE_NONE:
			return "NONE";
		case GET:
			return "GET";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		case HEAD:
			return "HEAD";
		case DELETE:
			return "DELETE";
		case OPTIONS:
			return "OPTIONS";
		case PATCH:
			return "PATCH";
		case TRACE:
			return "TRACE";
		default:
			return "NONE";
	}
}

bool HttpRequestBuilder::buildFirstLine(string str, bool checkOnly)
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
	
	size_t now = 0;
	HttpMethodType methodType = METHOD_TYPE_NONE;
	for (map<string, HttpMethodType>::iterator it = http_methods.begin(); it != http_methods.end(); it++)
	{
		if (str.compare(0,it->first.length(), it->first) == 0)
		{
			now += it->first.length();
			methodType = it->second;
			break;
		}
	}
	if (now == 0)
	{
		// cout << "method is invalid." << endl;
		return false;
	}
	if (str[now] == ' ')
	{
		now++;
	}
	else
	{
		// cout << "next method, 1 space is needed." << endl;
		return false;
	}
	size_t http_idx = str.find("HTTP/", now);
	if (http_idx == string::npos)
	{
		// cout << "HTTP/ is not exist." << endl;
		return false;
	}
	else if (http_idx == now)
	{
		// cout << "path is not exist." << endl;
		return false;
	}

	if (str[http_idx-1] != ' ')
	{
		// cout << "next path, 1 space is needed." << endl;
		return false;
	}
	string path = str.substr(now, http_idx-now-1);
	for(size_t i = 0; i < path.length(); i++)
	{
		if (path[i] < 33 || path[i] > 126)
		{
			// cout << "path must printable." << endl;
			return false;
		}
	}

	now = http_idx + 5;  // skip "HTTP/"
	if (now == str.length())
	{
		// cout << "HTTP version is not exist." << endl;
		return false;
	}
	string httpVersion = str.substr(now, str.length()-now);
	if (httpVersion.length() != 3)
	{
		// cout << "HTTP version length must be 3." << endl;
		return false;
	}
	else if (!(isdigit(httpVersion[0]) && isdigit(httpVersion[2]) && httpVersion[1] == '.'))
	{
		// cout << "HTTP version format must be X.X (X is one digit)." << endl;
		return false;
	}

	if (!checkOnly)
	{
		this->setMethodType(methodType);
		this->setPath(path);
		this->setHttpVersion(httpVersion);
		this->setBuildStep(BUILD_HEADER);
	}
	return true;
}


bool HttpRequestBuilder::setHeader(string str, bool checkOnly)
{
	
	size_t idx = str.find(':');

	// if (!checkOnly) cout << "SETHEADER@@@@@@@@@@@@" << str << endl;
	// if (idx == -1 || str.find(":", idx+1) != -1) {
	// 	// cout << "delimeter : is invalid." << endl;
	// 	return false;
	// }

	if (idx == string::npos)
	{
		// cout << "delimeter : is invalid." << endl;
		return false;
	}

	string key = str.substr(0, idx);
	bool is_valid_key = true;
	for (size_t i = 0; i < key.length(); i++)
	{
		if (!(key[i] >= 33 && key[i] <= 126) || key[i] == ':')
		{
			is_valid_key = false;
			break;
		}
	}
	if (!key.length() || !is_valid_key)
	{
		// cout << "key " << key << " is invalid." << endl;
		return false;
	}

	// : 이후 공백 skip
	idx++;
	while (idx < str.length() && isspace(str[idx]))
	{
		idx++;
	}

	string value = str.substr(idx, str.length());
	if (!value.length())
	{
		// cout << "value " << value << " is invalid." << endl;
		return false;
	}
	// first line의 경우 반드시 공백 문자가 1개 이상 필요한데, key의 경우 공백문자 포함될 경우 이미 이전에 필터링됨
	// 때문에 key에 대해서는 first line인지 다시 확인할 필요가 없음, key에는 공백문자가 있으면 안되기 때문
	for(size_t i = 0; i < value.length(); i++)
	{
		if (this->buildFirstLine(value.substr(i, value.length()), true))
		{
			// cout << "value " << value << " has http first header line." << endl;
			return false;
		}
	}

	if (!checkOnly)
	{
		string lowerKey = Utils::toLowerCase(key);
		this->headers[lowerKey] = value;

		if (!lowerKey.compare("content-length"))
		{ // contentLength 갱신
			if (this->isChunked)
			{  // isChunked인 경우 Content-Length header는 없어야 함
				return false;
			}
			bool correct = true;
			for(size_t i = 0; i < value.length(); i++)
			{
				if (!isdigit(value[i]))
				{
					correct = false;
					break;
				}
			}
			if (correct)
			{
				this->contentLength = atoi(value.c_str());
			}
		}
		else if (!lowerKey.compare("transfer-encoding") && !value.compare("chunked")) //새로고침!
		{  // isChunked 갱신
			if (this->contentLength > 0)
			{  // isChunked인 경우 Content-Length header는 없어야 함
				return false;
			}
			this->isChunked = true;
		}
	}

	return true;
}

string HttpRequestBuilder::getHeader(string key)
{
	return this->headers[key];
}

void HttpRequestBuilder::print(void)
{
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << "BuildStep: " << this->buildStep << endl;
	cout << "Method: " << this->methodType << endl;
	cout << "Path: " << this->path << endl;
	cout << "Version: " << this->httpVersion << endl;
	cout << "isChunked: " << this->isChunked << endl;

	cout << endl << "***Headers***" << endl;
	for (map<string,string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
	{
		cout << it->first << ": " << it->second << endl;
	}

	cout << endl << "***Body*** (Content-Length: " << this->contentLength << ")" << endl;
	cout << this->body << endl;
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl << endl;
}

// return value
// -1: garbarage request가 생성되는 경우 (request는 초기화됨)
//  0: 온전한 request가 완성된 경우
//  1: 아직 판단이 불가능한 경우 (다음 호출까지 확인해봐야 하는 경우)
int HttpRequestBuilder::isHttp(string &recvBuf)
{
	vector<string> lines = split(recvBuf, "\r\n");
	if (!lines.size())
	{  // \r\n이 recvBuf에 등장하지 않은 경우에 대한 예외처리
		string s(recvBuf);
		lines.push_back(s); // lines가 비어있기 때문에 일단 집어 넣음
	}

	int lines_index = -1;  // first line 직후의 lines index
	if (getBuildStep() == BUILD_FIRST)
	{  // first line부터 완성해야 하는 경우
		for(size_t i = 0; i < lines.size()-1; i++)
		{
			for(size_t j = 0; j < lines[i].length(); j++)
			{ // 쓰레기 byte가 first line의 앞에 있는 경우를 고려해 이를 skip
				if (buildFirstLine(lines[i].substr(j, lines[i].length())))
				{
					break;
				}
			}
			if (getMethodType() != METHOD_TYPE_NONE)
			{  // first line을 찾은 경우
				lines_index = i + 1;
				break;
			}
		}
		if (lines_index == -1)
		{  // first line이 없는 경우
			recvBuf = lines[lines.size()-1]; // \r\n으로 끝나는 모든 line들 (lines[:-1])은 이미 first line 포맷이 아님을 확인했기에 배제, 마지막 line(lines[-1])만 recvBuf에 남김
			cout << "[RETURN 1] first line doesn't exist." << endl;
			return 1;
		}
	}
	else
	{
		lines_index = 0;
	}
	cout << "START_LINE: " << lines_index << endl;

	HttpMethodType methodType = getMethodType();

	bool body_required = (methodType == POST || methodType == PUT || methodType == PATCH);
	string body = "";
	int chunked_number = -1; // chunked_number == -1 => 이번에 chunked number가 등장할거다
	for(size_t i = lines_index; i < lines.size()-1; i++)
	{
		if (lines[i].length() == 0)
		{  // empty line //size -1 까지만 보기 때문에 \r\n으로 끝난놈과 \r\n\r\n으로 끝난 놈을 구별할 필요X 무조건 \r\n\r\n으로 끝난 놈임
			cout << "EMPTY LINE" << i << endl;
			if (getBuildStep() == BUILD_HEADER && body_required)
			{  
				// header finish
				// header를 완성한 뒤 body로 넘어가는 경우
				setBuildStep(BUILD_BODY);
			}
			else if (getBuildStep() == BUILD_BODY && body_required)
			{  // 이미 body를 완성중이었던 경우
				if (getIsChunked())
				{  // chunked 1번, 4번
					// 빈문자열이 올경우 이전까지만 정상적인 요청으로 하고 리턴 0
					needMoreChunk = true;
					appendBody(body);
					recvBuf = Utils::stringJoin(lines, "\r\n", i);
					cout << "[RETURN 0] chunked body success" << endl;
					return 0;
				}
				else
				{
					body += "\r\n";
				}
			}
			else if (!body_required)
			{  // body가 필요없는 method인 경우
				recvBuf = Utils::stringJoin(lines, "\r\n", i+1);  // recvBuf에 다음 line부터 추가
				cout << "[RETURN 0] header is finished. (body is not required.)" << endl;
				return 0;  // 온전한 request 완성 (body 없는 method)
			}
		}
		else if (getBuildStep() == BUILD_BODY)
		{  // body를 완성중이던 경우
			if (getIsChunked())
			{
				if (chunked_number == -1)
				{  // chunked number가 나올 차례인 경우
					if (Utils::isDigitString(lines[i]))
					{
						stringstream ss(lines[i]);
						ss >> std::hex >> chunked_number;
						if (chunked_number == 0)
						{  // chunked 2번, chunked number가 0이면 request가 끝이라는 의미
							needMoreChunk = false;
							appendBody(body);
							recvBuf = Utils::stringJoin(lines, "\r\n", i+1);
							cout << "[RETURN 0] chunked body success" << endl;
							return 0;
						}
					}
					else
					{  // chunked 3번, chunked number가 나올 차례인데 다른 문자열이 나온 경우, 이전까지의 body로 request가 끝났다고 가정
						needMoreChunk = true;
						appendBody(body);
						recvBuf = Utils::stringJoin(lines, "\r\n", i);
						cout << "[RETURN 0] chunked body success" << endl;
						return 0;
					}
				}
				else
				{  // chunked string이 나올 차례인 경우
					if (static_cast<int>(lines[i].length()) != chunked_number)
					{  // chunked 5번
						erase();
						recvBuf = Utils::stringJoin(lines, "\r\n", i);  // recvBuf에 현재 line부터 추가
						cout << "[RETURN -1] chunked length mismatch: " << chunked_number << " vs " << lines[i].length() << endl;
						return -1;
					}
					else
					{
						body += lines[i];
						chunked_number = -1;
					}
				}
			}
			else
			{
				body += lines[i] + "\r\n";
			}
		}
		else if (getBuildStep() == BUILD_HEADER)
		{  // header를 완성중이던 경우
			if (!setHeader(lines[i]))
			{  // header 차례인데 유효하지 않은 header가 나온 경우
				erase();  // 기존에 완성하던 request는 버림
				recvBuf = Utils::stringJoin(lines, "\r\n", i);  // recvBuf에 현재 line부터 추가
				cout << "[RETURN -1]  invalid header." << endl;
				return -1;
			}
		}
	}

	// 마지막라인 처리(\r\n으로 끝났을 때 마지막라인이 빈문자열이라는 것이 핵심)
	if (body_required && getBuildStep() == BUILD_BODY)
	{  // 마지막 line까지 포함해 body 완성 후 content-length와 비교
		if (getIsChunked())
		{  // chunked인 경우 마지막 line을 recvBuf에 남긴 뒤에 이후 다시 판단
			appendBody(body);
			recvBuf = "";
			if (chunked_number != -1) // 이제 문자열이 나올차례다
			{
				stringstream ss;
				ss << std::hex << chunked_number;
				recvBuf += ss.str() + "\r\n";
			}
			recvBuf += lines[lines.size()-1];
			cout << "[RETURN 1] chunked is not finished." << endl;
			return 1;
		}
		else
		{
			body += lines[lines.size()-1];

			if (getContentLength() <= 0)
			{  // 유효하지 않은 contentLength인 경우
				erase();  // 기존에 완성하던 request는 버림
				recvBuf = body;  // 그동안 모았던 body를 모두 잔여물로 취급해 recvBuf에 담음
				cout << "[RETURN -1] invalid contentLength: " << endl;
				return -1;
			}
			else if (body.length() < getContentLength())
			{  // content-length까지 body가 완성되지 않은 경우
				recvBuf = body;  // recvBuf를 body로 덮어씌움
				cout << "[RETURN 1] body is not sufficient." << endl;
				return 1;
			}
			else
			{  // body가 충분히 모인 경우
				setBody(body.substr(0, getContentLength()));  // 완성된 body를 content-length만큼 slicing해 body에 추가
				recvBuf = body.substr(getContentLength(), body.length()-getContentLength());  // 잔여물 body는 recvBuf에 덮어씌움
				cout << "[RETURN 0] body is sufficient." << endl;
				return 0;
			}
		}
	}
	else
	{  // 마지막 line (\r\n으로 끝나지 않음)은 잔여물로 취급해 recvBuf에 저장
		recvBuf = lines[lines.size()-1];
		cout << "[RETURN 1] header is not finished." << endl;
		return 1;
	}

	cout << "[RETURN 0] last return." << endl;
	return 0;
}

vector<string> HttpRequestBuilder::split(const string& s, const string& delim)
{
	vector<string> result;
    size_t start = 0;
    size_t end = s.find(delim);
    while (end != string::npos)
	{
        result.push_back(s.substr(start, end - start));
        start = end + delim.size();
        end = s.find(delim, start);
        if (end == string::npos)
		{
			if (start >= s.length())
			{
				result.push_back(""); // \r\n 으로 끝난 문자열이면 result 맨마지막에 "" 요소 추가
			}
			else
			{
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
	string serverProtocol = "HTTP/" + httpVersion;
	string method = getMethod(methodType);
	string requestbody = body;
	requestMessage = new HttpRequestMessage(method, path, serverProtocol, headers, body, needMoreChunk);
	erase();
	return requestMessage;
}

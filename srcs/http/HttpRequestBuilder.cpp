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

	httpMethods.insert(make_pair<string, HttpMethodType>("GET", GET));
	httpMethods.insert(make_pair<string, HttpMethodType>("POST", POST));
	httpMethods.insert(make_pair<string, HttpMethodType>("PUT", PUT));
	httpMethods.insert(make_pair<string, HttpMethodType>("HEAD", HEAD));
	httpMethods.insert(make_pair<string, HttpMethodType>("DELETE", DELETE));
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
	this->body.append(body);
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
		default:
			return "NONE";
	}
}

bool HttpRequestBuilder::buildFirstLine(string str, bool checkOnly, bool formatOnly)
{
	HttpMethodType methodType = METHOD_TYPE_NONE;

	vector<string> firstLine = split(str, " ");
	if (firstLine.size() != 3)
	{
		return false;
	}

	// version check
	string httpVersion = firstLine[2];
	if (httpVersion.length() == 8)
	{
		size_t idx = httpVersion.find("HTTP/");
		if (idx == string::npos)
		{ // cout << "HTTP/ is not exist." << endl;
			return false;
		}
		else if (!(isdigit(httpVersion[5]) && httpVersion[6] == '.' && isdigit(httpVersion[7])))
		{ // cout << "HTTP version format must be X.X (X is one digit)." << endl;
			return false;
		}
	}
	else
	{ // cout << "HTTP version length must be 8." << endl;
		return false;
	}

	// path check
	string path = firstLine[1];
	for (size_t i = 0; i < path.length(); i++)
	{
		if (path[i] < 33 || path[i] > 126)
		{ // cout << "path must printable." << endl;
			return false;
		}
	}

	if (formatOnly == false)
	{
		bool exist = false;
		// method check (none_type은 구현되지 않은 메서드로 간주함)
		string method = firstLine[0];
		for (map<string, HttpMethodType>::iterator it = httpMethods.begin(); it != httpMethods.end(); it++)
		{
			if (method.compare(it->first) == 0)
			{
				methodType = it->second;
				exist = true;
				break;
			}
		}

		if (exist == false)
		{
			return false;
		}
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

	if (idx == string::npos)
	{	// cout << "header field must include colon." << endl;
		return false;
	}

	string key = str.substr(0, idx);
	bool isValidKey = true;
	for (size_t i = 0; i < key.length(); i++)
	{
		if (!(key[i] >= 33 && key[i] <= 126) || key[i] == ':')
		{
			isValidKey = false;
			break;
		}
	}
	if (key.length() == 0 || isValidKey == false)
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
	if (value.length() == 0)
	{
		// cout << value's length is 0 << endl;
		return false;
	}

	// first line의 경우 반드시 공백 문자가 2개 필요한데, key의 경우 공백문자 포함될 경우 이미 이전에 필터링됨
	// 때문에 key에 대해서는 first line인지 다시 확인할 필요가 없음, value에 대해서만 다시 한번 확인
	for (size_t i = 0; i < value.length(); i++)
	{
		if (buildFirstLine(value.substr(i, value.length()-i), true, true))
		{	// cout << "value " << value << " has http first header line." << endl;
			return false;
		}
	}

	if (!checkOnly)
	{
		string lowerKey = Utils::toLowerCase(key);
		
		// content-length와 host는 중복되면 잘못된 요청으로 간주함.
		if (!lowerKey.compare("content-length") && this->headers.find("content-length") != headers.end())
		{
			return false;
		}
		else if (!lowerKey.compare("host") && this->headers.find("host") != headers.end())
		{
			return false;
		}
		
		this->headers[lowerKey] = value;

		if (!lowerKey.compare("content-length"))
		{	// contentLength 갱신
			if (this->isChunked)
			{	// isChunked인 경우 Content-Length header는 없어야 함
				return false;
			}
			bool correct = true;
			for (size_t i = 0; i < value.length(); i++)
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

	int linesIndex = -1;  // first line 직후의 lines index이며, flag 역할도 겸함.
	if (getBuildStep() == BUILD_FIRST)
	{ // first line부터 완성해야 하는 경우
		for (size_t i = 0; i < lines.size()-1; i++)
		{
			for (size_t j = 0; j < lines[i].length(); j++)
			{	// 쓰레기 byte가 first line의 앞에 있는 경우를 고려해 이를 skip
				if (buildFirstLine(lines[i].substr(j, lines[i].length()-j)))
				{
					linesIndex = i + 1;
					break;
				}
			}
		}

		if (linesIndex == -1)
		{	// first line이 없는 경우
			// 구현되지 않은 메서드가 온건 아닌지 체크
			for (size_t i = 0; i < lines.size()-1; i++)
			{
				for (size_t j = 0; j < lines[i].length(); j++)
				{
					if (buildFirstLine(lines[i].substr(j, lines[i].length()-j), false, true))
					{
						// 구현되지 않은 메서드는 바로 return 0
						recvBuf = Utils::stringJoin(lines, "\r\n", linesIndex+1);
						cout << "[RETURN 0] not implemented method" << endl;
						return 0;
					}
				}
				
			}
			recvBuf = lines[lines.size()-1]; // \r\n으로 끝나는 모든 line들 (lines[:-1])은 이미 first line 포맷이 아님을 확인했기에 배제, 마지막 line(lines[-1])만 recvBuf에 남김
			cout << "[RETURN 1] first line doesn't exist." << endl;
			return 1;
		}
	}
	else
	{
		linesIndex = 0;
	}
	cout << "START_LINE: " << linesIndex << endl;

	HttpMethodType methodType = getMethodType();

	bool bodyRequired = (methodType == POST || methodType == PUT);
	string body = "";
	int chunkedNumber = -1; // chunkedNumber == -1 => 이번에 chunked number가 등장할거다
	for (size_t i = linesIndex; i < lines.size()-1; i++)
	{
		if (lines[i].length() == 0)
		{	// empty line //size -1 까지만 보기 때문에 \r\n으로 끝난놈과 \r\n\r\n으로 끝난 놈을 구별할 필요X 무조건 \r\n\r\n으로 끝난 놈임
			cout << "EMPTY LINE" << i << endl;
			if (headers.find("host") == headers.end())
			{
				recvBuf = Utils::stringJoin(lines, "\r\n", i+1);
				cout << "[RETURN -1] request must be include host header." << endl;
				return -1;
			}
			if (getBuildStep() == BUILD_HEADER && bodyRequired)
			{  	// header finish (header를 완성한 뒤 body로 넘어가는 경우)
				setBuildStep(BUILD_BODY);
			}
			else if (getBuildStep() == BUILD_BODY && bodyRequired)
			{	// 이미 body를 완성중이었던 경우
				if (getIsChunked())
				{	// chunked 1번(알맞은 숫자와 스트링이 번갈아 나온 후 0(종료플래그)없이 \r\n\r\n이 나온 경우)
					// checked 2번(내용과 0(종료플래그)없이 \r\n\r\n만 나온 경우 -> body size = 0이라고 치고 return 0)
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
			else if (!bodyRequired)
			{	// body가 필요없는 method인 경우
				recvBuf = Utils::stringJoin(lines, "\r\n", i+1);  // recvBuf에 다음 line부터 추가
				cout << "[RETURN 0] header is finished. (body is not required.)" << endl;
				return 0;  // 온전한 request 완성 (body 없는 method)
			}
		}
		else if (getBuildStep() == BUILD_BODY)
		{	// body를 완성중이던 경우
			if (getIsChunked())
			{
				if (chunkedNumber == -1)
				{	// chunked number가 나올 차례인 경우
					if (Utils::isDigitString(lines[i]))
					{
						stringstream ss(lines[i]);
						ss >> std::hex >> chunkedNumber;
						if (chunkedNumber == 0)
						{	// chunked 2번, chunked number가 0이면 request가 끝이라는 의미
							needMoreChunk = false;
							appendBody(body);
							recvBuf = Utils::stringJoin(lines, "\r\n", i+1);
							cout << "[RETURN 0] chunked body success" << endl;
							return 0;
						}
					}
					else
					{	// chunked 3번(숫자와 스트링이 번갈아 나오다가 숫자가 나올 차례인데 문자열이 나온 경우, 이전까지의 body로 request가 끝났다고 가정함)
						needMoreChunk = true;
						appendBody(body);
						recvBuf = Utils::stringJoin(lines, "\r\n", i);
						cout << "[RETURN 0] chunked body success" << endl;
						return 0;
					}
				}
				else
				{	// chunked string이 나올 차례인 경우
					if (static_cast<int>(lines[i].length()) != chunkedNumber)
					{	// chunked 5번(chucked 숫자에 맞지 않는 길이의 문자열이 올 경우)
						erase(); // 기존에 완성하던 request는 버림
						recvBuf = Utils::stringJoin(lines, "\r\n", i);  // recvBuf에 현재 line부터 추가
						cout << "[RETURN -1] chunked length mismatch: " << chunkedNumber << " vs " << lines[i].length() << endl;
						return -1;
					}
					else
					{
						body += lines[i];
						chunkedNumber = -1;
					}
				}
			}
			else
			{	// chunked가 아닌 바디를 완성하는 중이기 때문에 body에 이어 붙임
				body += lines[i] + "\r\n";
			}
		}
		else if (getBuildStep() == BUILD_HEADER)
		{	// header를 완성중이던 경우
			if (setHeader(lines[i]) == false)
			{	// header 차례인데 유효하지 않은 header가 나온 경우
				erase();  // 기존에 완성하던 request는 버림
				recvBuf = Utils::stringJoin(lines, "\r\n", i);  // recvBuf에 현재 line부터 추가
				cout << "[RETURN -1]  invalid header." << endl;
				return -1;
			}
		}
	}

	// 마지막라인 처리(\r\n으로 끝났을 때 마지막라인이 빈문자열이라는 것이 핵심)
	if (bodyRequired && getBuildStep() == BUILD_BODY)
	{	// 마지막 line까지 포함해 body 완성 후 content-length와 비교
		if (getIsChunked())
		{	// chunked인 경우 마지막 line을 recvBuf에 남긴 뒤에 이후 다시 판단
			appendBody(body);
			recvBuf = "";
			if (chunkedNumber != -1) // 이제 문자열이 나올차례다
			{
				stringstream ss;
				ss << std::hex << chunkedNumber;
				recvBuf += ss.str() + "\r\n";
			}
			recvBuf += lines[lines.size()-1];
			cout << "[RETURN 1] chunked is not finished." << endl;
			return 1;
		}
		else
		{
			body += lines[lines.size()-1];

			if (getContentLength() < 0)
			{	// 유효하지 않은 contentLength인 경우
				erase();  // 기존에 완성하던 request는 버림
				recvBuf = body;  // 그동안 모았던 body를 모두 잔여물로 취급해 recvBuf에 담음
				cout << "[RETURN -1] invalid contentLength: " << endl;
				return -1;
			}
			else if (body.length() < getContentLength())
			{	// content-length까지 body가 완성되지 않은 경우
				recvBuf = body;  // recvBuf를 body로 덮어씌움
				cout << "[RETURN 1] body is not sufficient." << endl;
				return 1;
			}
			else
			{	// body가 충분히 모인 경우
				setBody(body.substr(0, getContentLength()));  // 완성된 body를 content-length만큼 slicing해 body에 추가
				recvBuf = body.substr(getContentLength(), body.length()-getContentLength());  // 잔여물 body는 recvBuf에 덮어씌움
				cout << "[RETURN 0] body is sufficient." << endl;
				return 0;
			}
		}
	}
	else
	{	// 마지막 line (\r\n으로 끝나지 않음)은 잔여물로 취급해 recvBuf에 저장
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

	// delim이 존재하지 않는 경우 result에 집어 넣고 종료
	if (end == string::npos)
	{
		result.push_back(s);
		return result;
	}
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
	string serverProtocol = httpVersion;
	string method = getMethod(methodType);
	string requestbody = body;
	requestMessage = new HttpRequestMessage(method, path, serverProtocol, headers, body, needMoreChunk);
	print();
	erase();
	return requestMessage;
}
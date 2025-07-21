#include "AvalonHttpUtil.h"

#include <sstream>
#include "AvalonStringUtil.h"

namespace Avalon
{
	HttpResponseMaker::HttpResponseMaker(const char* version)
	{
		m_Version = AVALON_DEFAULT_HTTP_VERSION;
		if(version != NULL)
		{
			m_Version = version;
		}
		m_Params["Content-Type"] = "text/html;charset=utf-8";
		m_Params["Connection"] = "Keep-Alive";
	}

	HttpResponseMaker::~HttpResponseMaker()
	{

	}

	void HttpResponseMaker::SetHeadParam(const char* key, const char* value)
	{
		if(key == NULL || value == NULL) return;
		m_Params[std::string(key)] = std::string(value);
	}

	void HttpResponseMaker::Make(std::string& resp, const std::string& content)
	{
		resp.clear();
		resp += m_Version;
		resp += " 200 OK\r\n";

		size_t contentLen = content.length();
		if(contentLen != 0) contentLen += strlen("\r\n\r\n");
		m_Params["Content-Length"] = Avalon::StringUtil::ConvertToString<UInt64>(contentLen);
		
		for(HttpHeadParamMap::iterator iter = m_Params.begin();
			iter != m_Params.end(); ++iter)
		{
			resp += iter->first;
			resp += ": ";
			resp += iter->second;
			resp += "\r\n";
		}
		resp += "\r\n";

		if(contentLen != 0)
		{
			resp += content;
			resp += "\r\n\r\n";
		}
	}

	void HttpResponseMaker::Make404Error(std::string& resp)
	{
		std::string strContent;
		strContent += "<html>\r\n";
		strContent += "<head><title>404 Not Found</title></head>\r\n";
		strContent += "<body bgcolor=\"white\">\r\n";
		strContent += "<center><h1>404 Not Found</h1></center>\r\n";
		strContent += "<hr><center>http_util</center>\r\n";
		strContent += "</body>\r\n";
		strContent += "</html>\r\n";
		strContent += "<!-- The padding to disable MSIE's friendly error page -->";
		strContent += "<!-- The padding to disable MSIE's friendly error page -->";
		strContent += "<!-- The padding to disable MSIE's friendly error page -->";
		strContent += "<!-- The padding to disable MSIE's friendly error page -->";
		strContent += "<!-- The padding to disable MSIE's friendly error page -->";
		strContent += "<!-- The padding to disable MSIE's friendly error page -->";

		char szTemp[100];
		avalon_snprintf(szTemp, 100, "Content-Length: %d\r\n", (int)strContent.size());

		resp = "HTTP/1.1 404 Not Found\r\n";
		resp += "Content-Type: text/html; charset=UTF-8\r\n";
		resp += szTemp;
		resp += "Connection: keep-alive\r\n";
		resp += "\r\n";
		resp += strContent;
	}


	HttpParamStringMaker::HttpParamStringMaker()
	{

	}

	HttpParamStringMaker::~HttpParamStringMaker()
	{

	}

	void HttpParamStringMaker::AddParam(const std::string& key, const std::string& value)
	{
		if(key.empty() || value.empty()) return;

		int keybuflen = 3 * key.length();
		char* keybuf = new char[keybuflen];
		int keylen = UrlEncode(keybuf, keybuflen, key.c_str(), key.length());
		keybuf[keylen] = 0;

		int valbuflen = 3 * value.length();
		char* valbuf = new char[valbuflen];
		int valuelen = UrlEncode(valbuf, valbuflen, value.c_str(), value.length());
		valbuf[valuelen] = 0;

		m_Params.push_back(std::make_pair(keybuf, valbuf));
		delete[] keybuf;
		delete[] valbuf;
	}

	int HttpParamStringMaker::UrlEncode(char* dest, int destsize, const char* src, int size)
	{
		if(dest == NULL || src == NULL) return -1;

		int index = 0;
		for(int i = 0; i < size; ++i)
		{
			int val = src[i];
			
			if(val <= 0 || isalpha(val) || isdigit(val) || val == '-' || val == '.' || val == '~')
			{
				if(index + 1 > destsize) return -1;
				dest[index++] = val;
			}
			else
			{
				if(index + 3 > destsize) return - 1;
				dest[index++] = '%';
				dest[index++] = Avalon::StringUtil::IntToHex(val / 16);
				dest[index++] = Avalon::StringUtil::IntToHex(val % 16);
			}
		}

		return index;
	}

	std::string HttpParamStringMaker::GetParamStr() const
	{
		std::string str;
		for(ParamVec::const_iterator iter = m_Params.begin();
			iter != m_Params.end(); ++iter)
		{
			if(!str.empty()) str += '&';
			str += iter->first;
			str += '=';
			str += iter->second;
		}
		return str;
	}


	HttpRequestMaker::HttpRequestMaker(const char* version)
	{
		m_Version = AVALON_DEFAULT_HTTP_VERSION;
		if(version != NULL)
		{
			m_Version = version;
		}
		m_Params["Connection"] = "Keep-Alive";
	}

	HttpRequestMaker::~HttpRequestMaker()
	{

	}

	void HttpRequestMaker::SetHeadParam(const char* key, const char* value)
	{
		if(key == NULL || value == NULL) return;
		m_Params[std::string(key)] = std::string(value);
	}

	void HttpRequestMaker::MakeGET(std::string& req, const std::string& host, UInt32 port, const std::string& uri)
	{
		req += "GET ";
		req += uri;
		req += "?";
		req += GetParamStr();
		req += " ";
		req += m_Version;
		req += "\r\n";

		std::ostringstream oss;
		oss << host << ":" << port;
		SetHeadParam("Host", oss.str().c_str());

		for(HttpHeadParamMap::iterator iter = m_Params.begin(); iter != m_Params.end(); ++iter)
		{
			req += iter->first;
			req += ": ";
			req += iter->second;
			req += "\r\n";
		}
		req += "\r\n";
	}

	void HttpRequestMaker::MakePOST(std::string& req, const std::string& host, UInt32 port, const std::string& uri)
	{
		req += "POST ";
		req += uri;
		req += " ";
		req += m_Version;
		req += "\r\n";

		std::ostringstream oss;
		oss << host << ":" << port;
		SetHeadParam("Host", oss.str().c_str());

		std::string paramstr = GetParamStr();
		int paramlen = paramstr.length();
		if(paramlen != 0)
		{
			paramlen += strlen("\r\n\r\n");
		}
		SetHeadParam("Content-Length", Avalon::StringUtil::ConvertToString<Int32>(paramlen).c_str());
		
		for(HttpHeadParamMap::iterator iter = m_Params.begin(); iter != m_Params.end(); ++iter)
		{
			req += iter->first;
			req += ": ";
			req += iter->second;
			req += "\r\n";
		}
		req += "\r\n";

		req += paramstr;
		req += "\r\n\r\n";
	}


	HttpGETRequestMaker::HttpGETRequestMaker(const char* version)
		:HttpRequestMaker(version)
	{
		SetHeadParam("Accept", "*/*");
		SetHeadParam("User-Agent", "AvalonHttpUtil");
	}

	HttpGETRequestMaker::~HttpGETRequestMaker()
	{

	}

	void HttpGETRequestMaker::Make(std::string& req, const std::string& host, UInt32 port, const std::string& uri)
	{
		HttpRequestMaker::MakeGET(req, host, port, uri);
	}

	HttpPOSTRequestMaker::HttpPOSTRequestMaker(const char* version)
		:HttpRequestMaker(version)
	{
		SetHeadParam("Content-type", "application/x-www-form-urlencoded; charset=UTF-8");
	}

	HttpPOSTRequestMaker::~HttpPOSTRequestMaker()
	{

	}

	void HttpPOSTRequestMaker::Make(std::string& req, const std::string& host, UInt32 port, const std::string& uri)
	{
		HttpRequestMaker::MakePOST(req, host, port, uri);
	}


	HttpParser::HttpParser()
	{
		m_MethodType = HTTP_METHOD_NODE;
	}

	HttpParser::~HttpParser()
	{

	}

	int HttpParser::Parse(const char* request, int len)
	{
		if(len < 5) return 0;

		if(strncmp(request, "GET ", 4) == 0)
		{
			m_MethodType = HTTP_METHOD_GET;
		}
		else if(strncmp(request, "POST ", 5) == 0)
		{
			m_MethodType = HTTP_METHOD_POST;
		}
		else if(strncmp(request, "HTTP/", 5 == 0))
		{
			m_MethodType = HTTP_METHOD_RESP;
		}
		else return -1;

		const char* pHeadEnd = strstr((char*)request, "\r\n\r\n");
		if(pHeadEnd == NULL) return 0;

		int headParamBegin = ParseHeadParams(request, pHeadEnd - request);
		if(headParamBegin <= 0) return headParamBegin;

		int nContentLen = 0;
		if(m_MethodType == HTTP_METHOD_POST || m_MethodType == HTTP_METHOD_RESP)
		{
			nContentLen = Avalon::StringUtil::ConvertToValue<Int32>(GetHeadParam("Content-Length"));
		}
		int nHeadLen = pHeadEnd - request + 4;
		if(len < nHeadLen + nContentLen) return 0;

		m_Content.clear();
		m_Content.assign(request + nHeadLen, request + len);

		if(m_MethodType == HTTP_METHOD_GET || m_MethodType == HTTP_METHOD_POST)
		{
			const char* uriBegin = request;
			if(m_MethodType == HTTP_METHOD_GET) uriBegin = uriBegin + 4;
			else uriBegin = uriBegin + 5;

			const char* uriEnd = strstr((char*)(uriBegin), " HTTP");
			if(uriEnd == NULL) return -1;

			int ret = ParseUri(uriBegin, uriEnd - uriBegin);
			if(ret <= 0) return ret;
		}

		return 1;
	}

	const std::string& HttpParser::GetHeadParam(const std::string& key) const
	{
		static std::string nullstr;
		for(ParamVec::const_iterator iter = m_HeadParams.begin();
			iter != m_HeadParams.end(); ++iter)
		{
			if(iter->first == key) return iter->second;
		}
		return nullstr;
	}

	const std::string& HttpParser::GetUriParam(const std::string& key) const
	{
		static std::string nullstr;
		for(ParamVec::const_iterator iter = m_UriParams.begin();
			iter != m_UriParams.end(); ++iter)
		{
			if(iter->first == key) return iter->second;
		}
		return nullstr;
 	}

	int HttpParser::ParseHeadParams(const char* head, int len)
	{
		m_Uri.clear();
		m_HeadParams.clear();

		const char* pHeadParam = strstr((char*)head, "\r\n");
		if(pHeadParam == NULL) return -1;

		const char* curParam = pHeadParam;
		while(curParam != NULL && curParam + 2 < head + len)
		{
			const char* nextParam = strstr((char*)curParam + 2, "\r\n");
			
			std::string pair;
			if(nextParam != NULL)
			{
				pair.assign(curParam + 2, nextParam);
				curParam = nextParam;
			}
			else
			{
				pair.assign(curParam + 2, head + len);
				curParam = NULL;
			}

			std::string::size_type index = pair.find_first_of(":");
			if(index == std::string::npos) continue;

			std::string key(pair.begin(), pair.begin() + index);
			if(pair.at(index+1) == ' ') index++;
			index++;
			std::string value(pair.begin() + index, pair.end());
			if(!key.empty() && !value.empty())
			{
				m_HeadParams.push_back(std::make_pair(key, value));
			}
		}

		return pHeadParam - head;
	}

	int HttpParser::ParseUri(const char* uri, int len)
	{
		m_Uri.clear();
		const char* paramBegin = strstr((char*)uri, "?");
		if(paramBegin != NULL)
		{
			m_Uri.assign(uri, paramBegin);
		}
		else
		{
			m_Uri.assign(uri, uri + len);
			return 1;
		}

		std::string paramstr;
		paramstr.assign(paramBegin + 1, uri + len);
		std::vector<std::string> params;
		Avalon::StringUtil::Split(paramstr, params, "&");

		for(std::vector<std::string>::iterator iter = params.begin();
			iter != params.end(); ++iter)
		{
			std::string pair = *iter;

			std::string::size_type splitpos = pair.find_first_of('=');
			if(splitpos == std::string::npos) continue;

			std::string key(pair.begin(), pair.begin()+splitpos);
			std::string value(pair.begin() + splitpos + 1, pair.end());

			char* keybuf = new char[key.length() + 1];
			int keylen = UrlDecode(keybuf, key.length(), key.c_str(), key.length());
			keybuf[keylen] = 0;
			
			char* valuebuf = new char[value.length() + 1];
			int valuelen = UrlDecode(valuebuf, value.length(), value.c_str(), value.length());
			valuebuf[valuelen] = 0;

			m_UriParams.push_back(std::make_pair(keybuf, valuebuf));
			delete[] keybuf;
			delete[] valuebuf;
		}

		return 1;
	}

	int HttpParser::UrlDecode(char* dest, int destsize, const char* src, int size)
	{
		if(dest == NULL || src == NULL) return -1;

		int index = 0;
		for(int i = 0; i < size; ++i)
		{
			if(index >= destsize) return -1;

			if(src[i] == '%')
			{
				if(i + 2 >= size) return -1;

				dest[index++] = Avalon::StringUtil::HexToInt(src[i + 1]) * 16 + Avalon::StringUtil::HexToInt(src[i + 2]);
			}
			else
			{
				dest[index++] = src[i];
			}
		}
		return 0; 
	}
}

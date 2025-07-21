#ifndef _AVALON_HTTP_UTIL_H_
#define _AVALON_HTTP_UTIL_H_

#include <string>
#include <vector>
#include <map>
#include "AvalonDefine.h"
#include "AvalonStringUtil.h"

namespace Avalon
{
	enum HttpMethodType
	{
		HTTP_METHOD_NODE	= 0,	 
		HTTP_METHOD_GET		= 1,	//get请求
		HTTP_METHOD_POST	= 2,	//post请求
		HTTP_METHOD_RESP	= 3,	//回复
	};

#define AVALON_DEFAULT_HTTP_VERSION "HTTP/1.1"

	/**
	 *@brief http回复构造器                                                                     
	 */
	class HttpResponseMaker
	{
		typedef std::map<std::string, std::string>	HttpHeadParamMap;

	public:
		HttpResponseMaker(const char* version = AVALON_DEFAULT_HTTP_VERSION);
		~HttpResponseMaker();

		/**
		 *@brief 设置一个头部参数                                                                     
		 */
		void SetHeadParam(const char* key, const char* value);

		/**
		 *@brief 构造回复                                                                     
		 */
		void Make(std::string& resp, const std::string& content);

		/**
		 *@brief 构造错误回复                                                                     
		 */
		static void Make404Error(std::string& resp);

	public:
		std::string			m_Version;
		HttpHeadParamMap	m_Params;
	};

	/**
	 *@brief http请求参数列表构造                                                                   
	 */
	class HttpParamStringMaker
	{
		typedef std::vector<std::pair<std::string, std::string> >	ParamVec;

	public:
		HttpParamStringMaker();
		virtual ~HttpParamStringMaker();

		void AddParam(const std::string& key, const std::string& value);

		template<typename T>
		void AddParam(const std::string& key, T value)
		{
			int keybuflen = 3 * key.length();
			char* keybuf = new char[keybuflen];
			UrlEncode(keybuf, keybuflen, key.c_str(), key.length());
			m_Params.push_back(std::make_pair(keybuf, Avalon::StringUtil::ConvertToString<T>(value)));
			delete[] keybuf;
		}

		std::string GetParamStr() const;

		static int UrlEncode(char* dest, int destsize, const char* src, int size);
	public:
		ParamVec m_Params;
	};

	/**
	 *@brief http请求构造器                                                                     
	 */
	class HttpRequestMaker : public HttpParamStringMaker
	{
		typedef std::map<std::string, std::string>	HttpHeadParamMap;
	public:
		HttpRequestMaker(const char* version = AVALON_DEFAULT_HTTP_VERSION);
		virtual ~HttpRequestMaker();

	public:
		/**
		 *@brief 设置一个头部参数                                                                     
		 */
		void SetHeadParam(const char* key, const char* value);

		/**
		 *@brief 构造get请求                                                                     
		 */
		void MakeGET(std::string& req, const std::string& host, UInt32 port, const std::string& uri);

		/**
		 *@brief 构造post请求                                                                     
		 */
		void MakePOST(std::string& req, const std::string& host, UInt32 port, const std::string& uri);

	public:
		std::string			m_Version;
		HttpHeadParamMap	m_Params;
	};

	/**
	 *@brief GET请求构造器                                                                     
	 */
	class HttpGETRequestMaker : public HttpRequestMaker
	{
	public:
		HttpGETRequestMaker(const char* version = AVALON_DEFAULT_HTTP_VERSION);
		~HttpGETRequestMaker();

	public:
		void Make(std::string& req, const std::string& host, UInt32 port, const std::string& uri);
	};

	/**
	 *@brief POST请求构造器                                                                     
	 */
	class HttpPOSTRequestMaker : public HttpRequestMaker
	{
	public:
		HttpPOSTRequestMaker(const char* version = AVALON_DEFAULT_HTTP_VERSION);
		~HttpPOSTRequestMaker();

	public:
		void Make(std::string& req, const std::string& host, UInt32 port, const std::string& uri);
	};

	/**
	 *@brief http请求解析                                                                     
	 */
	class HttpParser
	{
		typedef std::vector<std::pair<std::string, std::string> > ParamVec;
	public:
		HttpParser();
		~HttpParser();

	public:
		/**
		 *@brief 解析请求
		 *@return -1:数据错误 0:数据不完整 >0:接收完全
		 */
		int Parse(const char* request, int len);

		/**
		 *@brief 获取头部参数                                                                     
		 */
		const std::string& GetHeadParam(const std::string& key) const;

		/**
		 *@brief 获取uri参数                                                                     
		 */
		const std::string& GetUriParam(const std::string& key) const;

		/**
		 *@brief 获取内容                                                                     
		 */
		const std::string& GetContent() const{ return m_Content; }

		/**
		 *@brief 获取uri                                                                     
		 */
		const std::string& GetUri() const{ return m_Uri; }

		/**
		 *@brief 获取类型                                                                     
		 */
		int GetMethodType() const{ return m_MethodType; }

	private:
		/**
		 *@brief 解析头部信息
		 *@return -1:数据错误 0：数据不完整 >0:参数开始位置
		 */
		int ParseHeadParams(const char* head, int len);

		/**
		 *@brief 解析uri
		 *@return -1:数据错误 0：数据不完整 >0:解析成功
		 */
		int ParseUri(const char* uri, int len);

	public:
		static int UrlDecode(char* dest, int destsize, const char* src, int size);

	private:
		//类型
		int			m_MethodType;
		//头部参数
		ParamVec	m_HeadParams;
		//参数
		ParamVec	m_UriParams;
		//uri
		std::string	m_Uri;
		//内容
		std::string	m_Content;
	};
}

#endif

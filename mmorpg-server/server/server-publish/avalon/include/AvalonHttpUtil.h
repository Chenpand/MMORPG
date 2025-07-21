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
		HTTP_METHOD_GET		= 1,	//get����
		HTTP_METHOD_POST	= 2,	//post����
		HTTP_METHOD_RESP	= 3,	//�ظ�
	};

#define AVALON_DEFAULT_HTTP_VERSION "HTTP/1.1"

	/**
	 *@brief http�ظ�������                                                                     
	 */
	class HttpResponseMaker
	{
		typedef std::map<std::string, std::string>	HttpHeadParamMap;

	public:
		HttpResponseMaker(const char* version = AVALON_DEFAULT_HTTP_VERSION);
		~HttpResponseMaker();

		/**
		 *@brief ����һ��ͷ������                                                                     
		 */
		void SetHeadParam(const char* key, const char* value);

		/**
		 *@brief ����ظ�                                                                     
		 */
		void Make(std::string& resp, const std::string& content);

		/**
		 *@brief �������ظ�                                                                     
		 */
		static void Make404Error(std::string& resp);

	public:
		std::string			m_Version;
		HttpHeadParamMap	m_Params;
	};

	/**
	 *@brief http��������б���                                                                   
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
	 *@brief http��������                                                                     
	 */
	class HttpRequestMaker : public HttpParamStringMaker
	{
		typedef std::map<std::string, std::string>	HttpHeadParamMap;
	public:
		HttpRequestMaker(const char* version = AVALON_DEFAULT_HTTP_VERSION);
		virtual ~HttpRequestMaker();

	public:
		/**
		 *@brief ����һ��ͷ������                                                                     
		 */
		void SetHeadParam(const char* key, const char* value);

		/**
		 *@brief ����get����                                                                     
		 */
		void MakeGET(std::string& req, const std::string& host, UInt32 port, const std::string& uri);

		/**
		 *@brief ����post����                                                                     
		 */
		void MakePOST(std::string& req, const std::string& host, UInt32 port, const std::string& uri);

	public:
		std::string			m_Version;
		HttpHeadParamMap	m_Params;
	};

	/**
	 *@brief GET��������                                                                     
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
	 *@brief POST��������                                                                     
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
	 *@brief http�������                                                                     
	 */
	class HttpParser
	{
		typedef std::vector<std::pair<std::string, std::string> > ParamVec;
	public:
		HttpParser();
		~HttpParser();

	public:
		/**
		 *@brief ��������
		 *@return -1:���ݴ��� 0:���ݲ����� >0:������ȫ
		 */
		int Parse(const char* request, int len);

		/**
		 *@brief ��ȡͷ������                                                                     
		 */
		const std::string& GetHeadParam(const std::string& key) const;

		/**
		 *@brief ��ȡuri����                                                                     
		 */
		const std::string& GetUriParam(const std::string& key) const;

		/**
		 *@brief ��ȡ����                                                                     
		 */
		const std::string& GetContent() const{ return m_Content; }

		/**
		 *@brief ��ȡuri                                                                     
		 */
		const std::string& GetUri() const{ return m_Uri; }

		/**
		 *@brief ��ȡ����                                                                     
		 */
		int GetMethodType() const{ return m_MethodType; }

	private:
		/**
		 *@brief ����ͷ����Ϣ
		 *@return -1:���ݴ��� 0�����ݲ����� >0:������ʼλ��
		 */
		int ParseHeadParams(const char* head, int len);

		/**
		 *@brief ����uri
		 *@return -1:���ݴ��� 0�����ݲ����� >0:�����ɹ�
		 */
		int ParseUri(const char* uri, int len);

	public:
		static int UrlDecode(char* dest, int destsize, const char* src, int size);

	private:
		//����
		int			m_MethodType;
		//ͷ������
		ParamVec	m_HeadParams;
		//����
		ParamVec	m_UriParams;
		//uri
		std::string	m_Uri;
		//����
		std::string	m_Content;
	};
}

#endif

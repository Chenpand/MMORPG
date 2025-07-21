

#ifndef __CCHTTPREQUEST_H__
#define __CCHTTPREQUEST_H__

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpClient.h"



class HttpClient
{
public:
    static HttpClient *getInstance();
    static void destroyInstance();

    void setSSLVerification(const std::string& caFile);
    void send(HttpRequest* request);
    
    inline void setTimeoutForConnect(int value) {_timeoutForConnect = value;};
    inline int getTimeoutForConnect() {return _timeoutForConnect;}
    inline void setTimeoutForRead(int value) {_timeoutForRead = value;};
    inline int getTimeoutForRead() {return _timeoutForRead;};

	//url encode
	static std::string urlEscape(const char* str, int len);
    //url decode
	static std::string urlUnescape(const char* str, int len);

	void updata();			// Ö÷Ïß³Ì
private:
    HttpClient();
    virtual ~HttpClient();
    bool init(void);
    
    bool lazyInitThreadSemphore();
    void networkThread();
    void dispatchResponseCallbacks();
    
private:
    int _timeoutForConnect;
    int _timeoutForRead;
};


#endif //__CCHTTPREQUEST_H__

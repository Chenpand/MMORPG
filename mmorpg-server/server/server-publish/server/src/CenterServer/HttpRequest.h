
#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <string>
#include <vector>
#include <functional>

class HttpClient;
class HttpResponse;

typedef std::function<void(HttpClient* client, HttpResponse* response)> ccHttpRequestCallback;
typedef void (*SEL_HttpResponse)(HttpClient* client, HttpResponse* response);
#define httpresponse_selector(_SELECTOR) (SEL_HttpResponse)(&_SELECTOR)



class HttpRequest
{
public:
    enum class Type
    {
        GET,
        POST,
        PUT,
        DEL,
        UNKNOWN,
    };
    
    HttpRequest()
    {
        _requestType = Type::UNKNOWN;
        _url.clear();
        _requestData.clear();
        _tag.clear();
        //_pTarget = nullptr;
        _pSelector = nullptr;
        _pCallback = nullptr;
        _pUserData = nullptr;
    };
    
    virtual ~HttpRequest()
    {
   //     if (_pTarget)
   //     {
			//delete _pTarget;
   //     }
    };
    
    inline void setRequestType(Type type)
    {
        _requestType = type;
    };
   
    inline Type getRequestType()
    {
        return _requestType;
    };
    
    inline void setUrl(const char* url)
    {
        _url = url;
    };
   
    inline const char* getUrl()
    {
        return _url.c_str();
    };
    
    inline void setRequestData(const char* buffer, size_t len)
    {
        _requestData.assign(buffer, buffer + len);
    };
   
    inline char* getRequestData()
    {
        if(_requestData.size() != 0)
            return &(_requestData.front());

        return nullptr;
    }
   
    inline size_t getRequestDataSize()
    {
        return _requestData.size();
    }
    
    inline void setTag(const char* tag)
    {
        _tag = tag;
    };
   
    inline const char* getTag()
    {
        return _tag.c_str();
    };
    
    inline void setUserData(void* pUserData)
    {
        _pUserData = pUserData;
    };
    
    inline void* getUserData()
    {
        return _pUserData;
    };
    
    inline void setResponseCallback(const ccHttpRequestCallback& callback)
    {
        _pCallback = callback;
    }
    
    //inline void* getTarget()
    //{
    //    return _pTarget;
    //}

    class _prxy
    {
    public:
        _prxy( SEL_HttpResponse cb ) :_cb(cb) {}
        ~_prxy(){};
        operator SEL_HttpResponse() const { return _cb; }
    protected:
        SEL_HttpResponse _cb;
    };
    
    /** Get the selector function pointer, mainly used by HttpClient */
    inline _prxy getSelector()
    {
        return _prxy(_pSelector);
    }
    
    inline const ccHttpRequestCallback& getCallback()
    {
        return _pCallback;
    }
    
    /** Set any custom headers **/
    inline void setHeaders(std::vector<std::string> pHeaders)
   	{
   		_headers=pHeaders;
   	}
   
    /** Get custom headers **/
   	inline std::vector<std::string> getHeaders()
   	{
   		return _headers;
   	}
    
protected:
	
    // properties
    Type                        _requestType;    /// kHttpRequestGet, kHttpRequestPost or other enums
    std::string                 _url;            /// target url that this request is sent to
    std::vector<char>           _requestData;    /// used for POST
    std::string                 _tag;            /// user defined tag, to identify different requests in response callback
    //void*                        _pTarget;        /// callback target of pSelector function
    SEL_HttpResponse            _pSelector;      /// callback function, e.g. MyLayer::onHttpResponse(HttpClient *sender, HttpResponse * response)
    ccHttpRequestCallback       _pCallback;      /// C++11 style callbacks
    void*                       _pUserData;      /// You can add your customed data here 
    std::vector<std::string>    _headers;		      /// custom http headers
};



#endif //__HTTP_REQUEST_H__

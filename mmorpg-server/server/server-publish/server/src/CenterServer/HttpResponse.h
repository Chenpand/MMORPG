
#ifndef __HTTP_RESPONSE__
#define __HTTP_RESPONSE__

#include "HttpRequest.h"


class HttpResponse
{
public:
   
    HttpResponse(HttpRequest* request)
    {
        _pHttpRequest = request;
        _succeed = false;
        _responseData.clear();
        _errorBuffer.clear();
    }
    
    virtual ~HttpResponse()
    {
        if (_pHttpRequest)
        {
            delete _pHttpRequest;
        }
    }
    
    inline HttpRequest* getHttpRequest()
    {
        return _pHttpRequest;
    }
        
    inline bool isSucceed()
    {
        return _succeed;
    };
    
    inline std::vector<char>* getResponseData()
    {
        return &_responseData;
    }
    
    inline std::vector<char>* getResponseHeader()
    {
        return &_responseHeader;
    }

    inline long getResponseCode()
    {
        return _responseCode;
    }

    inline const char* getErrorBuffer()
    {
        return _errorBuffer.c_str();
    }
	   
    inline void setSucceed(bool value)
    {
        _succeed = value;
    };
    
    inline void setResponseData(std::vector<char>* data)
    {
        _responseData = *data;
    }
    
    inline void setResponseHeader(std::vector<char>* data)
    {
        _responseHeader = *data;
    }
    
    inline void setResponseCode(long value)
    {
        _responseCode = value;
    }
    
    inline void setErrorBuffer(const char* value)
    {
        _errorBuffer.clear();
        _errorBuffer.assign(value);
    };

	inline void setHttpRequest(HttpRequest* p)
	{
		_pHttpRequest = p;
	}
    
protected:
    bool initWithRequest(HttpRequest* request);
    
    // properties
    HttpRequest*        _pHttpRequest;  /// the corresponding HttpRequest pointer who leads to this response 
    bool                _succeed;       /// to indecate if the http reqeust is successful simply
    std::vector<char>   _responseData;  /// the returned raw data. You can also dump it as a string
    std::vector<char>   _responseHeader;  /// the returned raw header data. You can also dump it as a string
    long                _responseCode;    /// the status code returned from libcurl, e.g. 200, 404
    std::string         _errorBuffer;   /// if _responseCode != 200, please read _errorBuffer to find the reason 
    
};




#endif //__HTTP_RESPONSE_H__

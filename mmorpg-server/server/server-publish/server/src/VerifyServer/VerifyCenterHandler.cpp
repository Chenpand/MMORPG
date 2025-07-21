#include "VerifyCenterHandler.h"

void HandleProtocol(CLProtocol::VerifyCenterRegisterRes& res)
{
	if (res.result != 0)
	{
		ErrorLogStream << "register failed, reason:" << res.result << LogStream::eos;
		return;
	}

	ErrorLogStream << "register success." << LogStream::eos;
}
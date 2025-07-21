DEL /Q /S bin\win32\*
DEL /Q /S avalon.sdf
DEL /Q /S src\AvalonCore\.*.d src\AvalonCore\.*.o
DEL /Q /S src\AvalonDatabase\.*.d src\AvalonDatabase\.*.o
DEL /Q /S src\AvalonDataTable\.*.d src\AvalonDataTable\.*.o
DEL /Q /S src\AvalonFramework\.*.d src\AvalonFramework\.*.o
DEL /Q /S src\AvalonGame\.*.d src\AvalonGame\.*.o
DEL /Q /S src\AvalonLogger\.*.d src\AvalonLogger\.*.o
DEL /Q /S src\AvalonNetwork\.*.d src\AvalonNetwork\.*.o
DEL /Q /S src\AvalonScript\.*.d src\AvalonScript\.*.o
DEL /Q /S src\AvalonThread\.*.d src\AvalonThread\.*.o
DEL /Q /S src\AvalonFramework\.*.d src\AvalonFramework\.*.o
DEL /Q /S src\AvalonXml\.*.d src\AvalonXml\.*.o
DEL /Q /S src\testclient\.*.d src\testclient\.*.o
DEL /Q /S src\testcore\.*.d src\testcore\.*.o
DEL /Q /S src\testnetwork\.*.d src\testnetwork\.*.o

RD /Q /S ipch
RD /Q /S src\AvalonCore\Debug src\AvalonCore\Release
RD /Q /S src\AvalonDatabase\Debug src\AvalonDatabase\Release
RD /Q /S src\AvalonDataTable\Debug src\AvalonDataTable\Release
RD /Q /S src\AvalonFramework\Debug src\AvalonFramework\Release
RD /Q /S src\AvalonGame\Debug src\AvalonGame\Release
RD /Q /S src\AvalonLogger\Debug src\AvalonLogger\Release
RD /Q /S src\AvalonNetwork\Debug src\AvalonNetwork\Release
RD /Q /S src\AvalonScript\Debug src\AvalonScript\Release
RD /Q /S src\AvalonThread\Debug src\AvalonThread\Release
RD /Q /S src\AvalonXml\Debug src\AvalonXml\Release
RD /Q /S src\testclient\Debug src\testclient\Release
RD /Q /S src\testcore\Debug src\testcore\Release
RD /Q /S src\testnetwork\Debug src\testnetwork\Release

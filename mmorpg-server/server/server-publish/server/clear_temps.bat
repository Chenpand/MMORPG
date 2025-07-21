DEL /Q /S bin\win32\Debug\*
DEL /Q /S bin\win32\Release\*
DEL /Q /S xgame.sdf
DEL /Q /S src\CommonLib\.*.d src\CommonLib\.*.o
DEL /Q /S src\LoginServer\.*.d src\LoginServer\.*.o
DEL /Q /S src\AdminServer\.*.d src\AdminServer\.*.o
DEL /Q /S src\RecordServer\.*.d src\RecordServer\.*.o
DEL /Q /S src\GateServer\.*.d src\GateServer\.*.o
DEL /Q /S src\WorldServer\.*.d src\WorldServer\.*.o
DEL /Q /S src\SceneServer\.*.d src\SceneServer\.*.o
DEL /Q /S src\CenterServer\.*.d src\CenterServer\.*.o
DEL /Q /S src\SocialServer\.*.d src\SocialServer\.*.o

RD /Q /S ipch
RD /Q /S src\CommonLib\Debug src\CommonLib\Release
RD /Q /S src\LoginServer\Debug src\LoginServer\Release
RD /Q /S src\AdminServer\Debug src\AdminServer\Release
RD /Q /S src\RecordServer\Debug src\RecordServer\Release
RD /Q /S src\GateServer\Debug src\GateServer\Release
RD /Q /S src\WorldServer\Debug src\WorldServer\Release
RD /Q /S src\SceneServer\Debug src\SceneServer\Release
RD /Q /S src\CenterServer\Debug src\CenterServer\Release
RD /Q /S src\SocialServer\Debug src\SocialServer\Release

# Project directory
SUB_DIRS = src/CommonLib src/AdminServer src/RecordServer src/GateServer src/WorldServer src/SceneServer src/CenterServer src/CrossServer tools/TableDumper src/MatchServer src/RelayServer src/UdpConnServer src/CenterServer src/ReplayServer src/VerifyCenterServer src/VerifyServer src/GlobalActivityServer src/BattleServer src/BattleScene src/TeamCopyServer src/DungeonServer src/UnionServer src/ObserveServer src/TransferServer

.PHONY: all objs clean help

# Delete the default suffixes
.SUFFIXES:

all:
	@$(foreach N, $(SUB_DIRS), make -C $(N);)

objs:
	@$(foreach N, $(SUB_DIRS), make -C $(N) objs;)

clean:
	@$(foreach N, $(SUB_DIRS), make -C $(N) clean;)

install:
	cp bin/linux/AdminServer* ../server_res/AdminServer
	cp bin/linux/RecordServer* ../server_res/RecordServer
	cp bin/linux/GlobalRecordServer* ../server_res/GlobalRecordServer
	cp bin/linux/MatchServer* ../server_res/MatchServer
	cp bin/linux/GateServer* ../server_res/GateServer1
	cp bin/linux/GateServer* ../server_res/GateServer2
	cp bin/linux/WorldServer* ../server_res/WorldServer
	cp bin/linux/SceneServer* ../server_res/SceneServer
	cp bin/linux/UdpConnServer* ../server_res/UdpConnServer
	cp bin/linux/RelayServer* ../server_res/RelayServer
	cp bin/linux/CenterServer* ../server_res/CenterServer
	cp bin/linux/ReplayServer* ../server_res/ReplayServer
	cp bin/linux/CrossServer* ../server_res/CrossServer
	cp bin/linux/GlobalActivityServer* ../server_res/GlobalActivityServer
	cp bin/linux/TeamCopyServer*  ../server_res/TeamCopyServer
	cp bin/linux/DungeonServer*  ../server_res/DungeonServer
	cp bin/linux/UnionServer*  ../server_res/UnionServer
	cp bin/linux/ObserveServer*  ../server_res/ObserveServer
	cp bin/linux/TransferServer*  ../server_res/TransferServer

# Show help.
help:
	@echo 'Generic Makefile for C/C++ Programs (gcmakefile) version 0.5'
	@echo
	@echo 'Usage: make [TARGET]'
	@echo 'TARGETS:'
	@echo '  all       (=make) compile and link.'
	@echo '  objs      compile only (no linking).'
	@echo '  clean     clean objects and the executable file.'
	@echo '  help      print this message.'
	@echo
## End of the Makefile ##  Suggestions are welcome  ## All rights reserved ##
#############################################################################

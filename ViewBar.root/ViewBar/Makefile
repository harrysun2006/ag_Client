RM=del
RMDIR=rmdir /S /Q
CP=xcopy
REL_DIR=$(VIEWBAR_HOME)\release
REL_FILE=$(VIEWBAR_HOME)\Misc\*.*
NSIS="$(NSIS_HOME)\makensis.exe"
VS="$(VS80_HOME)\Common7\IDE\devenv.exe"

all:
	@echo Now building

	$(RMDIR) $(REL_DIR)
	mkdir $(REL_DIR)
	
	$(CP) /S /Y /C $(REL_FILE) $(REL_DIR)\

	$(VS) ViewBar.sln /build Release
	
	@echo Now removing Debug info
	$(RMDIR) $(REL_DIR)\update

	$(RM) $(REL_DIR)\log.txt
	$(RM) $(REL_DIR)\*.exp
	$(RM) $(REL_DIR)\*.lib
	$(RM) /s $(REL_DIR)\*.pdb
	$(RM) /s /ah $(REL_DIR)\skins\*.db
	
	@echo upx
	upx.bat

	@echo nsis
	$(NSIS) XPack\XPack.nsi
	
	@echo mount network drive
#	drive.bat
	
	@echo create dir
#	upload.exe "V:/AGloCo/Viewbar"
	




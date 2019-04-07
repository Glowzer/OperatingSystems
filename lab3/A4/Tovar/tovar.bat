@echo off

set A_COMP_PATH=\\fpmi-serv604\Common_stud\Timoshenko\A4

call :check H: %A_COMP_PATH%\Bank\e
goto :eof

:check
	set disk_name=%1
	set path_=%2

	if not exist %disk_name% (
		echo "%disk_name% doesn't exist."
		net use %disk_name% %path_% >nul  
	) else (
		net use %disk_name% /delete >nul
		net use %disk_name% %path_% >nul
	)
	if not errorlevel 1 (
		echo "tovar.bat completed successfully." 
	) else (
		echo "unsuccessfull creation of %disk_name% disk."
	)

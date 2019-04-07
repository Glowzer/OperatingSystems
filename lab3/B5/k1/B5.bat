@echo off

set B_COMP_PATH=\\fpmi-serv604\Common_stud\Timoshenko\B5

if exist %B_COMP_PATH%\k1\k3_old rd %B_COMP_PATH%\k1\k3_old /S /Q
if exist %B_COMP_PATH%\k2\k1_old rd %B_COMP_PATH%\k2\k1_old /S /Q
if exist %B_COMP_PATH%\k3\k2_old rd %B_COMP_PATH%\k3\k2_old /S /Q

md %B_COMP_PATH%\k1\k3_old
md %B_COMP_PATH%\k2\k1_old
md %B_COMP_PATH%\k3\k2_old


if exist %B_COMP_PATH%\k1\k2_new (
    xcopy %B_COMP_PATH%\k1\k2_new %B_COMP_PATH%\k3\k2_old /E /Q
    rd %B_COMP_PATH%\k1\k2_new /s /q
    )
if exist %B_COMP_PATH%\k2\k3_new (
    xcopy %B_COMP_PATH%\k2\k3_new %B_COMP_PATH%\k1\k3_old /E /Q
    rd %B_COMP_PATH%\k2\k3_new /s /q
    )
if exist %B_COMP_PATH%\k3\k1_new (
    xcopy %B_COMP_PATH%\k3\k1_new %B_COMP_PATH%\k2\k1_old /E /Q
    rd %B_COMP_PATH%\k3\k1_new /s /q
    )


md %B_COMP_PATH%\k3\k1_new\d\lectures
xcopy %B_COMP_PATH%\k1\d\lectures %B_COMP_PATH%\k3\k1_new\d\lectures /Q
md "%B_COMP_PATH%\k3\k1_new\d\Заказчики"
xcopy "B_COMP_PATH%\k1\d\Заказчики" "B_COMP_PATH%\k3\k1_new\d\Заказчики" /Q
md "%B_COMP_PATH%\k3\k1_new\d\Заказчики\БНТУ"
xcopy "B_COMP_PATH%\k1\d\Заказчики\БНТУ" "B_COMP_PATH%\k3\k1_new\d\Заказчики\БНТУ" /Q
md "%B_COMP_PATH%\k3\k1_new\d\Заказчики\БГУ"
xcopy "B_COMP_PATH%\k1\d\Заказчики\БГУ" "B_COMP_PATH%\k3\k1_new\d\Заказчики\БГУ" /E /Q

md %B_COMP_PATH%\k1\k2_new\e
xcopy %B_COMP_PATH%\k2\e %B_COMP_PATH%\k1\k2_new\e /Q
md %B_COMP_PATH%\k1\k2_new\d\texts
xcopy %B_COMP_PATH%\k2\d\texts %B_COMP_PATH%\k1\k2_new\d\texts /E /Q

md "%B_COMP_PATH%\K2\k3_new\data\Документация"
xcopy "%B_COMP_PATH%\k3\data\Документация" "%B_COMP_PATH%\K2\k3_new\data\Документация" /Q

@ECHO OFF

CD /D %~dp0

@REM Install folder
SET relative_install_path=%windir%\..\SOARE
FOR %%i IN ("%relative_install_path%") DO SET "install_path=%%~fi"

@REM SOARE
ECHO.
ECHO "  _____  _____  ___  ______ _____  "
ECHO " /  ___||  _  |/ _ \ | ___ \  ___| "
ECHO " \ `--. | | | / /_\ \| |_/ / |__   "
ECHO "  `--. \| | | |  _  ||    /|  __|  "
ECHO " /\__/ /\ \_/ / | | || |\ \| |___  "
ECHO " \____/  \___/\_| |_/\_| \_\____/  "
ECHO.
ECHO SOARE Installer
ECHO Antoine LANDRIEUX (MIT License) install.bat
ECHO https://github.com/AntoineLandrieux/SOARE/
ECHO.

@REM Show LICENSE file
IF EXIST "LICENSE" (
    TYPE LICENSE
    ECHO.
)

@REM Ask for install
:ask_install
SET ask=""
SET /P ask="Install SOARE? (yes/no): "
IF "%ask%" EQU "yes" GOTO :main
IF "%ask%" EQU "y" GOTO :main
IF "%ask%" EQU "no" GOTO :cancel
IF "%ask%" EQU "n" GOTO :cancel
GOTO :ask_install

:main
@REM If an installation is already present
IF EXIST %install_path% (
    ECHO Warning: Existing installation
:input
    SET ask_rem=""
    SET /P ask_rem="Delete the old installation? (yes/no): "
    IF "%ask_rem%" EQU "yes" GOTO :delete_install
    IF "%ask_rem%" EQU "y" GOTO :delete_install
    IF "%ask_rem%" EQU "no" GOTO :cancel
    IF "%ask_rem%" EQU "n" GOTO :cancel
    GOTO :input
)

@REM Enable colored output
:register
ECHO Updating REGISTER...
REG add HKEY_CURRENT_USER\Console /v VirtualTerminalLevel /t REG_DWORD /d 0x00000001 /f
IF NOT errorlevel 0 ECHO Error when update REG (try to run as an Administrator)

@REM Install...
:install
ECHO Creating file...
MKDIR %install_path%

FOR %%i IN ("%CD%\bin\soare.exe" "%CD%\LICENSE" "%CD%\README.md") DO (
    IF NOT EXIST "%%i" (
        ECHO "File %%i" is missing...
        GOTO :end
    )
)

FOR %%i IN ("%CD%\lib\" "%CD%\include\" "%CD%\doc\" "%CD%\resources\" "%CD%\script\" "%CD%\windows\") DO (
    IF NOT EXIST "%%i" (
        ECHO "Folder %%i" is missing...
        GOTO :end
    )
)

COPY "%CD%\bin\soare.exe" "%install_path%\soare.exe"
COPY "%CD%\LICENSE" "%install_path%\LICENSE"
COPY "%CD%\README.md" "%install_path%\README.md"

ECHO R | XCOPY "%CD%\include\" "%install_path%\include" /S /E
ECHO R | XCOPY "%CD%\lib\" "%install_path%\lib" /S /E
ECHO R | XCOPY "%CD%\doc\" "%install_path%\doc" /S /E
ECHO R | XCOPY "%CD%\script\" "%install_path%\script\" /S /E
ECHO R | XCOPY "%CD%\resources\" "%install_path%\resources" /S /E
ECHO R | XCOPY "%CD%\windows\" "%install_path%\windows" /S /E

SETX SOARE "%install_path%"
ECHO Success
GOTO :end

@REM Remove install
:delete_install
ECHO Remove installation..
RMDIR /S /Q %install_path%
GOTO :main

@REM Cancel
:cancel
ECHO Operation Canceled

@REM Quit
:end
ECHO.
PAUSE
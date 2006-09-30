@echo off

rem This is just a little madness added to prevent pauses during full-auto builds
IF "%1" == "SKIP" GOTO SKIP
echo PLEASE MAKE SURE YOU ARE NOT CURRENTLY RUNNING VISUAL STUDIO!
pause
GOTO SKIP
:SKIP

set VCVARS="C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
set P_out=win32rel\sdk_vc71
set P_="winSDK_vc71"
set P_OG="winSDK_vc71\OpenGUI"
set P_OGinc="winSDK_vc71\OpenGUI\include"
set P_OGlib="winSDK_vc71\OpenGUI\lib"
set P_OGLinc="winSDK_vc71\OGLRenderer\include"
set P_OGLlib="winSDK_vc71\OGLRenderer\lib"
set P_TACHinc="winSDK_vc71\Tachometer\include"
set P_TACHlib="winSDK_vc71\Tachometer\lib"
set P_demo="winSDK_vc71\Demos"
set P_demoOgre="winSDK_vc71\OgreDemos"
set P_OF="winSDK_vc71\OgreFusion"
set P_OFinc="winSDK_vc71\OgreFusion\include"
set P_OFlib="winSDK_vc71\OgreFusion\lib"
set LOG="sdkBuildLog_vc71.txt"

rem Reset the Build Log
echo Begin Build > %LOG% 2>&1

echo Loading Visual Studio Environment...
call %VCVARS% >> %LOG% 2>&1

rem Delete any previous SDK data
rmdir /s /q %P_% >> %LOG% 2>&1

echo Cleaning Solution...
echo ################ >> %LOG% 2>&1
echo ### CLEANING ### >> %LOG% 2>&1
echo ################ >> %LOG% 2>&1
echo ############################################### >> %LOG% 2>&1
echo ...Debug
devenv OpenGUI.sln /clean "Debug - All" >> %LOG%
echo ...Release
devenv OpenGUI.sln /clean "Release - All" >> %LOG%
call CleanAll.bat


echo Building Solution...
echo ################ >> %LOG% 2>&1
echo ### BUILDING ### >> %LOG% 2>&1
echo ################ >> %LOG% 2>&1
echo ############################################### >> %LOG% 2>&1
echo ...Debug
devenv OpenGUI.sln /build "Debug - All" >> %LOG%
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >> %LOG% 2>&1
echo !!!!!!!!!!!!!!!!!!! >> %LOG% 2>&1
echo !!!  END DEBUG  !!! >> %LOG% 2>&1
echo !!!!!!!!!!!!!!!!!!! >> %LOG% 2>&1

echo ...Release
devenv OpenGUI.sln /build "Release - All" >> %LOG%
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >> %LOG% 2>&1
echo !!!!!!!!!!!!!!!!!!! >> %LOG% 2>&1
echo !!! END RELEASE !!! >> %LOG% 2>&1
echo !!!!!!!!!!!!!!!!!!! >> %LOG% 2>&1


IF "%1" == "SKIP" GOTO SKIP2
echo PLEASE CHECK THE SDK BUILD LOG AND ENSURE THAT EVERYTHING BUILT CORRECTLY BEFORE CONTINUING
pause
GOTO SKIP2
:SKIP2

echo Build directory tree...
mkdir %P_OGinc%
mkdir %P_OGlib%
mkdir %P_OGLinc%
mkdir %P_OGLlib%
mkdir %P_TACHinc%
mkdir %P_TACHlib%
mkdir %P_demo%
mkdir %P_demoOgre%
mkdir %P_OFinc%
mkdir %P_OFlib%

echo Copying OpenGUI files...
copy OpenGUI\LICENSE.TXT %P_OG% >> %LOG% 2>&1
copy OpenGUI\*.h %P_OGinc% >> %LOG% 2>&1
copy Debug\OpenGUI_d.lib %P_OGlib% >> %LOG% 2>&1
copy Debug\OpenGUI_d.dll %P_OGlib% >> %LOG% 2>&1
copy Release\OpenGUI.lib %P_OGlib% >> %LOG% 2>&1
copy Release\OpenGUI.dll %P_OGlib% >> %LOG% 2>&1


echo Copying OpenGL Renderer files...
copy OpenGUI_OGLRenderer\*.h %P_OGLinc% >> %LOG% 2>&1
copy Release\OpenGUI_OGLRenderer.lib %P_OGLlib% >> %LOG% 2>&1
copy Debug\OpenGUI_OGLRenderer_d.lib %P_OGLlib% >> %LOG% 2>&1

echo Copying Tachometer files
copy examples\TachometerWidget\*.h %P_TACHinc% >> %LOG% 2>&1
copy examples\bin\TachometerWidget.dll %P_TACHlib% >> %LOG% 2>&1
copy examples\bin\TachometerWidget.lib %P_TACHlib% >> %LOG% 2>&1
copy examples\bin\TachometerWidget_d.dll %P_TACHlib% >> %LOG% 2>&1
copy examples\bin\TachometerWidget_d.lib %P_TACHlib% >> %LOG% 2>&1

echo Copying Demos...
echo ...Demo1
copy examples\bin\licenses.txt %P_demo% >> %LOG% 2>&1
copy examples\bin\pecot.ttf %P_demo% >> %LOG% 2>&1
copy examples\bin\kongtext.ttf %P_demo% >> %LOG% 2>&1
copy examples\bin\TachometerWidget.dll %P_demo% >> %LOG% 2>&1
copy examples\bin\tachometer.png %P_demo% >> %LOG% 2>&1
copy examples\bin\tachometer.xml %P_demo% >> %LOG% 2>&1
copy examples\bin\metal.png %P_demo% >> %LOG% 2>&1
copy examples\bin\metal.xml %P_demo% >> %LOG% 2>&1
copy examples\bin\OpenGUI.dll %P_demo% >> %LOG% 2>&1
copy examples\bin\demo1.xml %P_demo% >> %LOG% 2>&1
copy examples\bin\Demo1.exe %P_demo% >> %LOG% 2>&1
echo ...Demo O
copy examples\DemoO\Release\DemoO.exe %P_demoOgre% >> %LOG% 2>&1
copy examples\DemoO\Release\*.dll %P_demoOgre% >> %LOG% 2>&1
copy examples\DemoO\Release\*.zip %P_demoOgre% >> %LOG% 2>&1
copy examples\DemoO\Release\*.cfg %P_demoOgre% >> %LOG% 2>&1

echo Copying OgreFusion...
copy OgreFusion\LICENSE.TXT %P_OF% >> %LOG% 2>&1
copy OgreFusion\*.h %P_OFinc% >> %LOG% 2>&1
copy OgreFusion\Debug\OgreFusion_d.lib %P_OFlib% >> %LOG% 2>&1
copy OgreFusion\Debug\OgreFusion_d.dll %P_OFlib% >> %LOG% 2>&1
copy OgreFusion\Release\OgreFusion.lib %P_OFlib% >> %LOG% 2>&1
copy OgreFusion\Release\OgreFusion.dll %P_OFlib% >> %LOG% 2>&1


move %P_% %P_out%


IF "%1" == "SKIP" (
echo Cleaning Solution...
echo ################ >> %LOG% 2>&1
echo ### CLEANING ### >> %LOG% 2>&1
echo ################ >> %LOG% 2>&1
echo ############################################### >> %LOG% 2>&1
echo ...Debug
devenv OpenGUI.sln /clean "Debug - All" >> %LOG%
echo ...Release
devenv OpenGUI.sln /clean "Release - All" >> %LOG%
call CleanAll.bat
)

GOTO END
:END
IF "%1" == "SKIP" exit
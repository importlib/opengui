@echo Building HTML Docs...
@"C:\Program Files\doxygen\bin\doxygen.exe" DoxyfileInternal > buildLog-internal.txt 2>&1
@echo Compiling CHM...
@"c:\Program Files\HTML Help Workshop\hhc.exe" html-internal\index.hhp >> buildLog-internal.txt 2>&1
@echo DONE!
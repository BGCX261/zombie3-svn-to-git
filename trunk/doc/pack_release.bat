del *.tmp;
del *.exp;
del *.bak;
del *.bak;
del *.lib;
del release.zip;
del axchatter.cab;
del "C:\\Documents and Settings\\Administrator\\Local Settings\\Temporary Internet Files\\axchatter.cab"
copy ..\..\lib\release\*.dll .\
copy .\*.dll c:\windows\system32\
CABARC -s 6144 N axchatter.cab deploy_release.bat Microsoft.VC80.ATL.manifest Microsoft.VC80.CRT.manifest base64.dll mythread.dll myfile.dll mynet.dll myudpnet.dll message.dll bridgenet.dll axaudio.dll chatter.dll axchatter.dll cab_release.inf
"C:\\Program Files\\7-Zip\\7z.exe" a release.zip *.dll *.exe
regsvr32 axchatter.dll
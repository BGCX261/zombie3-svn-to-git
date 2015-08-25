del *.tmp;
del *.exp;
del *.bak;
#del *.lib;
del "C:\\Documents and Settings\\Administrator\\Local Settings\\Temporary Internet Files\\axchatter.cab"
copy .\*.dll c:\windows\system32\
regsvr32 axchatter.dll
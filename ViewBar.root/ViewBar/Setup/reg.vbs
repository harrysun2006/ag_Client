set WS = CreateObject("WScript.Shell")
WS.Run "regsvr32.exe /u mssoap30.dll"
WS.Run "regsvr32.exe /u mssoapR3.dll"
WS.Run "regsvr32.exe /u wisc30.dll"
WS.Run "regsvr32.exe /u msxml4r.dll"
WS.Run "regsvr32.exe /u msxml4.dll"

set ws = nothing
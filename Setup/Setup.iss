;Inno Setup script
;http://www.jrsoftware.org/isinfo.php

[Setup]
AppName=Cursorial CD
AppVerName=Cursorial CD
Compression=lzma
SolidCompression=yes
CreateAppDir=no
DisableProgramGroupPage=yes

[Files]
Source: "..\Release\CCD.exe"; DestDir: "{win}"

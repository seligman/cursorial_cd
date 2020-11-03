$func = "DWORD WINAPI GetModuleFileNameExW(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);";
$retdef = "0";
$lib = "Psapi";

$temp = <<'EOT';
RET Local_NAME(ALL)
{
    if (Local_LIB())
    {
        static FARPROC ret = NULL;

        if (!ret)
        {
            ret = GetProcAddress(Local_LIB(), "NAME");
        }

        if (ret)
        {
            typedef RET (WINAPI * PFN_Func)(PARAMS);
            return ((PFN_Func) ret)(NAMES);
        }
    }

    return DEF;
}
EOT

$func =~ s/ WINAPI / /;

$func =~ /([^ ]+) ([^\(]+)\(([^\)]*)/;
($ret, $name, $params) = ($1, $2, $3);
$names = "";
$types = "";
while ($params =~ /([^, ][^,]*[^, ]+) ([^, ]+)/g)
{
    ($a, $b) = ($1, $2);
    if ($names ne "")
    {
        $names .= ", ";
        $types .= ", ";
    }
    $names .= $b;
    $types .= $a;
}
# print "[$ret][$name] [$params] [$names] [$types]";

$temp =~ s/RET/$ret/gs;
$temp =~ s/ALL/$params/gs;
$temp =~ s/LIB/$lib/gs;
$temp =~ s/NAMES/$names/gs;
$temp =~ s/NAME/$name/gs;
$temp =~ s/PARAMS/$types/gs;
$temp =~ s/DEF/$retdef/gs;

print $temp;
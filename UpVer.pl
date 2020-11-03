chdir "Properties";

&cmd("p4 edit CCD.rc");
&cmd("p4 edit Hidden.cpp");
&cmd("p4 edit Hidden.h");
&cmd("p4 edit \"Make Hidden\\HiddenData.txt\"");

open RC, "CCD.rc";

$final = "";
$set = "0";

while ($l = <RC>)
{
    if ($l =~ /^(.*?)([0-9]+)(.*INTVERSION.*)$/si)
    {
        ($pre, $ver, $post) = ($1, $2, $3);
        
        if ($set eq "0")
        {
            $set = sprintf("%d", ($ver + 1));
            print "Moving from ", $ver, " to ", $set, "\n";
        }
        
        $final .= $pre . $set . $post;
    }
    elsif ($l =~ /^(.*)([0-9],[0-9],[0-9],[0-9])(.*COMMAVERSION.*)$/si)
    {
        ($pre, $ver, $post) = ($1, $2, $3);
        
        $temp = sprintf("%d,%d,%d,0", int($set / 100), int($set / 10) % 10, int($set) % 10);
        
        $final .= $pre . $temp . $post;
    }
    else
    {
        $final .= $l;
    }
}

close RC;

open RC, ">CCD.rc";
print RC $final;
close RC;

chdir("Make Hidden\\bin\\Debug");
&cmd("start /w \"\" MakeHidden.exe \"Build " . $set . "\"");
chdir("..\\..\\..");

&cmd("p4 revert -a Hidden.h");

print "Done.\n";

sub cmd
{
    my $c = shift;
    
    print "] ", $c, "\n";
    system $c;
}

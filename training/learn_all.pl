# two arguments: path to normalized logs and FRADE.conf
$usage="$0 normalized-logs frade-conf type\n";
if ($#ARGV < 2)
{
    print $usage;
    exit;
}
$type = $ARGV[2];
open(my $fh, '<', $ARGV[1])
    or die "Could not open file '$ARGV[1]' $!";
@windows=();
@regex=();
while(<$fh>)
{
    if ($_ =~ /WINDOWS/)
    {
	@items = split /\=/, $_;
	@windows = split /\s+/, $items[1];
    }
    if ($_ =~ /^MAIN/)
    {
	@items = split /\=/, $_;
	@regex = split /\s+/, $items[1];
    }
}
%requests=();
open(my $fh, '<', $ARGV[0])
    or die "Could not open file '$ARGV[1]' $!";
while(<$fh>)
{
#23.112.114.15 1492471058459 GET /mediawiki/ HTTP/1.1 301 193626
#23.112.114.15 1492471058801 GET /mediawiki/index.php/Main_Page HTTP/1.1 200 86414
    @items = split /\s/, $_;
    $ip = $items[0];
    $time = $items[1];
    $url = $items[3];
    $code = int($items[5]);
    if ($code == 301 && $type ne "all")
    {
	next;
    }
    if ($type eq "main" || $type eq "embed")
    {
	$found = 0;
	for $r (@regex)
	{
	    if ($url =~ /$r/)
	    {
		$found = 1;
	    }
	}
	if ($found == 0 && $type eq "main")
	{
	    next;
	}
	if ($found == 1 && $type eq "embed")
	{
	    next;
	}
    }
    if ($ip !~ /\d+\.\d+\.\d+\.\d+/)
    {
	next;
    }
    push(@{$requests{$ip}}, $time);
}
%global=();
for $w (@windows)
{
    $global{$w} = 0;
}
for $ip (sort {$a <=> $b} keys %requests)
{
    $lasttime = 0;
    @sorted = sort {$a <=> $b} @{$requests{$ip}};
    my %max=();
    for $w (@windows)
    {
	$max{$w} = 0;
    }
    for ($i=0; $i<scalar(@sorted)-1;$i++)
    {
	$a = $sorted[$i];
	for ($j=$i+1; $j<scalar(@sorted);$j++)
	{
	    $b = $sorted[$j];
	    for $w (@windows)
	    {
		if ($b - $a < $w)
		{
		    $diff = $j - $i + 1;
		    if ($diff > $max{$w})
		    {
			$max{$w} = $diff;
			#print "$ip max $max{$w} for $w\n";
		    }
		}
	    }
	}
	for $w (@windows)
	{
	    if ($max{$w} > $global{$w})
	    {
		$global{$w} = $max{$w};
	    }
	}
    }
}
for $w (@windows)
{
    #print "$w $global{$w}\n";
    print "$global{$w} ";
}

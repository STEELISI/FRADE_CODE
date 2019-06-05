# two arguments: path to normalized logs and FRADE.conf
$usage="$0 normalized-logs frade-conf type dyn3-file\n";
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
%ur=();
%urt=();


# Read from dyn3-file#
my %dyn3;
open(my $fh, '<', $ARGV[3])
    or die "Could not open file '$ARGV[1]' $!";

while(<$fh>)
{
        @vals = split /\s+/, $_;
        $dyn3{$vals[0]} = $vals[1];
        #print "\n $dyn3{$vals[0]} ";
}

open(my $fh, '<', $ARGV[0])
    or die "Could not open file '$ARGV[1]' $!";

$count=0;
$cost=50;
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
		if (exists($dyn3{$url}))
		{
			#$count = $count + $dyn3{$url}
                        $cost = $dyn3{$url}

		}
                else
                {
                       $cost = "50";
                }
		$found = 1;
                print "$ip $url $time $cost \n";
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
    push(@{$ur{$ip}}, $cost);

}


%global=();
for $w (@windows)
{
    $global{$w} = 0;
}
for $ip (sort {$a <=> $b} keys %requests)
{
    $lasttime = 0;
    #@sorted = sort {$a <=> $b} @{$requests{$ip}};
    @sorted = @{$requests{$ip}};
    @sorted_urls = @{$ur{$ip}};
     
    
	for($a = 0; $a < scalar(@sorted); $a++) {
    	for($b = 0; $b < scalar(@sorted)-1; $b ++){
        	if($sorted[$b] > $sorted[$b+1]) {
            	$temp = $sorted[$b+1];
            	$sorted[$b+1]=$sorted[$b];
            	$sorted[$b]=$temp;
                $temp = $sorted_urls[$b+1];
                $sorted_urls[$b+1]=$sorted_urls[$b];
                $sorted_urls[$b]=$temp;
        	}       
    	}
	}
        print " Sorting Over1 \n";
        for($a = 0; $a < scalar(@sorted); $a++) { 
              print " Sorting Over2 \n";
              print "Here $sorted[$a] $sorted_urls[$a] \n"; 

        }
        print " Sorting Over3 \n";

    my %max=();
    for $w (@windows)
    {
	$max{$w} = 0;
    }
    for ($i=0; $i<scalar(@sorted)-1;$i++)
    {
	$a = $sorted[$i];
        $c = $sorted_urls[$i];
        print "$a $c \n";
	for ($j=$i+1; $j<scalar(@sorted);$j++)
	{
	    $b = $sorted[$j];
            $d = $sorted_urls[$j];
            print "$b $d \n";
	    for $w (@windows)
	    {
		if ($b - $a < $w)
		{
                    print "$ip $a $b $i $j $sorted[$i] $sorted[$j]  $sorted_urls[$i]  $sorted_urls[$j]\n";
                    $diff = 0;

                   for($x=$i; $x<=$j;$x++)
                   {
                      $diff = $diff + $sorted_urls[$x];
                   }



                    print "$diff $i $j \n";

		    #$diff = $j - $i + 1;
		    if ($diff > $max{$w})
		    {
			$max{$w} = $diff;
			print "$ip max $max{$w} for $w\n";
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
    print "$w $global{$w}\n";
}

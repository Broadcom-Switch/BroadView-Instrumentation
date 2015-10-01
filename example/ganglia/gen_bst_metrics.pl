#!/usr/bin/perl

use JSON -support_by_pp;
my $json = JSON->new->pretty;

my $gangliaDir = "ganglia-3.7.1";
my $gangliaWebDir = "ganglia-web-3.7.0";
#my $inFile = "user_input.json";
my $inFile = "metric_input.json";
my $outDefJsonRep = "$gangliaWebDir/conf/default.json";
my $genDir = "$gangliaWebDir/graph.d";
my $bstReportFile = "bst_report";
my $bstReportFileName = "$genDir/bst_report.json";
my $gmondSrcConfFile = "gmond_src.conf";
my $gmondDestConfFile = "./gmond_dest.conf";
my $poll_interval = 2;
my $threshold_time = 5;
my $mygraph_title = "Buffers Consumed";


my $myjson;
{
  local $/; #Enable 'slurp' mode
  open my $fh, "<", $inFile;
  $myjson = <$fh>;
  close $fh;
}

my $json_object = $json->decode($myjson);
 
 
sub genDefaultJsonRep
{
    my $jSONStrStart = '{"included_reports": [';
    my $jSONStrEnd = ']}';
    my $myContent = "";

    open(OUT, ">$outDefJsonRep") || die "Couldn't open $outDefJsonRep: $!";
    $myContent = "\"$bstReportFile\"";
    my $JSONContent =  $jSONStrStart . $myContent . $jSONStrEnd . "\n";
    print OUT $JSONContent;
    close(OUT);
}

sub genBstJsonRep
{
    my $jSONStrStart = "{\"report_name\" : \"$bstReportFile\", \"report_type\" : \"standard\", \"title\" : \"BST STATS\", \"vertical_label\" : \"$mygraph_title\", \"series\" : [";
    my $jSONStrEnd = ']}';
    my $myContent = "";

    
    open(OUT, ">$bstReportFileName") || die "Couldn't open $bstReportFileName: $!";

    print OUT $jSONStrStart;

    for my $item( @{$json_object->{metricinfo}} ){
        my $metricName = $item->{metric};
        my $colorVal   = $item->{color};
        my $shortName   = $item->{shortname};

        $myContent .= "{ \"metric\": \"$metricName\", \"color\": \"$colorVal\", \"label\": \"$shortName\", \"line_width\": \"2\", \"type\": \"line\" },";
    }
    chop($myContent);
    print OUT $myContent; 
    print OUT $jSONStrEnd; 
    close(OUT);
}


sub genGmonDestConfFile
{
    my $confStrGroupStart = "collection_group {\n   collect_every = $poll_interval \n   time_threshold = $threshold_time \n\n";
    my $confStrGroupEnd = "\n}\n";
    my $confStrGroupMetricContent = "";
    my $gmondSrcContent;

    
    local $/; #Enable 'slurp' mode
    open my $fh, "<", $gmondSrcConfFile;
    $gmondSrcContent = <$fh>;
    close $fh;

    open(OUT, ">$gmondDestConfFile") || die "Couldn't open $gmondDestConfFile: $!";

    print OUT $gmondSrcContent;
    print OUT "\n\n /* BST module group */ \n\n";
    print OUT $confStrGroupStart; 

    for my $item( @{$json_object->{metricinfo}} ){
        my $metricName = $item->{metric};
        $confStrGroupMetricContent  .= "\n   metric {\n      name_match = \"$metricName\" \n     value_threshold = 1.0\n      title = \"BST stats\" \n   }";

    } 

    print OUT $confStrGroupMetricContent;
    print OUT $confStrGroupEnd;

    close(OUT);
}


genDefaultJsonRep();
genBstJsonRep();
genGmonDestConfFile();



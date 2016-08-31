#!/usr/bin/perl

use JSON -support_by_pp;
my $json = JSON->new->pretty;

my $inFile = $ARGV[0];
my $outBstAppFilePrefix = "bstapp_config_";
my $gmondSrcConfFile = "gmond_src_secondary.conf";
my $gmondDestConfFilePrefix = "gmond_dest_";
my $inMetricFile =  "metric_input.json";
my $poll_interval = 2;
my $threshold_time = 5;
my $metric_type_absolute = "absolute";
my $metric_type_percentage = "percentage";
my $stat_type_cfg_name = "stat_type";
my $stat_type_cfg_val_absolute = "1";
my $stat_type_cfg_val_percentage = "2";



my $myjson;
{
  local $/; #Enable 'slurp' mode
  open my $fh, "<", $inFile;
  $myjson = <$fh>;
  close $fh;
}

my $mymetricjson;
{
  local $/; #Enable 'slurp' mode
  open my $fh, "<", $inMetricFile;
  $mymetricjson = <$fh>;
  close $fh;
}


my $json_object = $json->decode($myjson);
my $sw_name = $json_object->{name};


my $json_metric_object = $json->decode($mymetricjson);

my $outBstAppFile = $outBstAppFilePrefix . $sw_name . ".cfg";
my $gmondDestConfFile = $gmondDestConfFilePrefix.$sw_name.".conf";

sub genConfigFile
{

    open(OUT, ">$outBstAppFile") || die "Couldn't open $outBstAppFile: $!";
    for my $item( @{$json_object->{configparams}} ){
        my $configStr = "";

        $configStr = $item->{param};

        if ($configStr eq "polling_interval"){
            $poll_interval = $item->{value};   
        }
        $configStr .= "="; 
        $configStr .= $item->{value};
        $configStr .= "\n";

        print OUT $configStr; 
    }
    my $configStr = "";
    
    $configStr = $stat_type_cfg_name;
    $configStr .= "=";
    $configStr .= ($json_metric_object->{metric_type} eq $metric_type_absolute)?$stat_type_cfg_val_absolute:$stat_type_cfg_val_percentage;
    
    print OUT $configStr; 
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

    my $gmondFormattedContent = sprintf($gmondSrcContent, $sw_name);

    open(OUT, ">$gmondDestConfFile") || die "Couldn't open $gmondDestConfFile: $!";

    print OUT $gmondFormattedContent;
    print OUT "\n\n /* BST module group */ \n\n";
    print OUT $confStrGroupStart; 

    for my $item( @{$json_metric_object->{metricinfo}} ){
        my $metricName = $item->{metric};
        $confStrGroupMetricContent  .= "\n   metric {\n      name_match = \"$metricName\" \n     value_threshold = 1.0\n      title = \"BST stats\" \n   }";

    } 

    print OUT $confStrGroupMetricContent;
    print OUT $confStrGroupEnd;

    close(OUT);
}

sub printCmdToRun
{
  print "Run gmond with the below command\n";
  print "gmond -c $gmondDestConfFile -i $outBstAppFile -f\n";

}



genConfigFile();
genGmonDestConfFile();
printCmdToRun();




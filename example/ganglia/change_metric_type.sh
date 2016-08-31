#This script kills already runnging gmetad, gmond processes
# and deletes existing rrds, and creates empy rrds  dir 
#This is used when metric type is changed between absolute and percentage

killall gmond
killall gmetad
rm -rf /var/lib/ganglia
mkdir /var/lib/ganglia
mkdir /var/lib/ganglia/rrds

chown nobody:nogroup  /var/lib/ganglia/rrds
if [ $? != 0 ]; then
	echo "Error: Failed to set owner for /var/lib/ganglia/rrds"
	exit 1
fi


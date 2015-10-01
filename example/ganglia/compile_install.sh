
jsonlint -v metric_input.json

if [ $? != 0 ]; then
	echo "Error: Invalid metric_input.json"
	exit 1
fi

jsonlint -v switch_conf.json
if [ $? != 0 ]; then
	echo "Error: Invalid switch_conf.json"
	exit 1
fi

perl gen_bst_metrics.pl
rm -rf /var/lib/ganglia
rm -rf /var/www/html/ganglia
mkdir /var/lib/ganglia
mkdir /var/lib/ganglia/rrds
mkdir /var/www/html/ganglia
mkdir /etc/ganglia-web

chown nobody:nogroup  /var/lib/ganglia/rrds
if [ $? != 0 ]; then
	echo "Error: Failed to set owner for /var/lib/ganglia/rrds"
	exit 1
fi

chown www-data:www-data  /var/www/html/ganglia
if [ $? != 0 ]; then
	echo "Error: Failed to set owner for /var/www/html/ganglia"
	exit 1
fi

cd ganglia-3.7.1
make clean
sleep 3

cd gmond
gengetopt --c-extension=c.in --input ./cmdline.sh
if [ $? != 0 ]; then
	echo "Error: Failed to generate cmdline.c.in"
	exit 1
fi

cd ../

./configure --with-gmetad
if [ $? != 0 ]; then
	echo "Error: Failed to configure Ganglia"
	exit 1
fi
sleep 3
make 
if [ $? != 0 ]; then
	echo "Error: Failed to compile Ganglia"
	exit 1
fi
sleep 3
make install
if [ $? != 0 ]; then
	echo "Error: Failed to install Ganglia"
	exit 1
fi

export LD_LIBRARY_PATH=/usr/local/lib/
cd ../ganglia-web-3.7.0/
make uninstall
make clean
make install
make uninstall
make clean
make install





jsonlint -v metric_input.json

if [ $? != 0 ]; then
	echo "Error: Invalid metric_input.json"
	exit 1
fi

perl gen_bst_metrics.pl

cd ./ganglia-web-3.7.0/
make uninstall
make clean
make install
make uninstall
make clean
make install


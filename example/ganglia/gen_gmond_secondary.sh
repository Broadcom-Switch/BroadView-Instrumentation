
if [ $# != 1 ]; then
	echo "Error: Invalid number of parameters!"
        echo "Usage: sh gen_gmond_secondary.sh <switch-config-jsonfile>"
	exit 1
fi

jsonlint -v metric_input.json

if [ $? != 0 ]; then
	echo "Error: Invalid metric_input.json"
	exit 1
fi

jsonlint -v $1
if [ $? != 0 ]; then
	echo "Error: Invalid $1"
	exit 1
fi

perl gen_gmond_secondary.pl $1


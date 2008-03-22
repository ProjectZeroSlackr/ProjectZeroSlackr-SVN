if [ -d logs ]; then
	rm -rf logs
fi
mkdir logs
sh build-hotdog.sh
sh build-ttk.sh

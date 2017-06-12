#!/bin/bash

if [ -e "count.dat" ]; then
	echo "[+] Plotting histogram data ..."
	python histPlot.py count.dat
else
	echo "[-] File 'count.dat' not found !"
fi

exit 0

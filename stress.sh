#!/bin/bash
for i in `seq 1 10`
do
	./clipshare-linux-binary &
done

wait

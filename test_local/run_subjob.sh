#!/bin/bash
/Users/mikacashman/Documents/KBaseSDK/fba_tools_clone/test_local/run_docker.sh run --rm --user $(id -u) -v /Users/mikacashman/Documents/KBaseSDK/fba_tools_clone/test_local/subjobs/$1/workdir:/kb/module/work -v /Users/mikacashman/Documents/KBaseSDK/fba_tools_clone/test_local/workdir/tmp:/kb/module/work/tmp $4 -e "SDK_CALLBACK_URL=$3" $2 async

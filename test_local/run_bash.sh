#!/bin/bash
script_dir="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)"
$script_dir/run_docker.sh run --user $(id -u) -i -t -v $script_dir/workdir:/kb/module/work test/fba_tools:latest bash

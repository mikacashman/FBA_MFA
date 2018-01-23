# test_local
In order to run the GA scripts you must first go into the Docker container by running bash run_bash.sh from this directory.  Then cd to /work/fbafiles/ then the directory of your choice.

#### From sdk module readme:
This directory contains temporary scripts and files needed to run tests 
locally in Docker installed on developers' machine. These files are not
supposed to be commited into git repo or to be copied inside Docker
image.

To run tests:
- change test_user and test_password in test.cfg file
- run "kb-sdk test"

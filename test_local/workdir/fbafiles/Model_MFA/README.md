Notable files/directories are:

* GAFiles - this is where the output from our custom scripts will go (as well as the specificaion of specific tests)
	* ID.txt - output of the IDs of a script
	* OV.txt - output of the objective value of a script
	* time.txt - output of the time it took to run each test
	* combined.txt - a combination of the above (in that order)
	* ...tests.txt - to specific specific IDs of tests to run using the GA_tests.cpp script

* tmp - contains all MFAToolkit files (input and output)
* driver_MFAmodel.cpp - runs random tests (compile with g++ -std=c++11 -pthread)
	* e.g. g++ -std=c++11 -pthread driver_MFAModel.cpp -o driver.out

##### Code Specifics
###### Input files:  /kb/module/work/fbafiles/EC_Model_MFA/tmp/MFAOutput/
###### Output files: /kb/module/work/fbafiles/EC_Model_MFA/GAFiles/

* GA_driver.cpp
	* Loops over multiple runs of FBA directly through the MFAToolkit
* runMFAToolkitGA.sh
	* Runs MFAToolkit from inside docker container (run_bash.sh) through work/fbafiles/EC_Model_MFA

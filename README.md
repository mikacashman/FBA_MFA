# Scrapped FBA (KBase) for running MFAToolkit
-----------------------------------------

This repo includes a scraped version of the fba_tools sdk module in KBase that is able to run the MFAToolkit with minimal extra code for clarity.

Files of importance are located in the **test_local/workdir/fbafiles** directory.  

The contents of each subdirectory may vary, but important files are:
* GAFiles - this is where the output from our custom scripts will go (as well as the specificaion of specific tests)
	* ID.txt - output of the IDs of a script
	* OV.txt - output of the objective value of a script
	* time.txt - output of the time it took to run each test
	* combined.txt - a combination of the above (in that order)
	* ModelGUITests.txt - to specific specific IDs of tests to run with verified OVs
* tmp - contains all MFAToolkit files (input and output)

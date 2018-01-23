////////////////////////////////////////////////////////////////////////////////
//	Driver to loop the MFAtoolkit script
//	For running software testing experiments on the MFAToolkit Model
//	Authors: Mikaela Cashman & Myra Cohen
//	Date: Summer 2017 -> this version is Spring 2018
////////////////////////////////////////////////////////////////////////////////

///////
// Input files:  /kb/module/work/fbafiles/Model_MFA/tmp/
// Output files: /kb/module/work/fbafiles/Model_MFA/GAFiles/
//////

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <iostream>
#include <sys/time.h>
#include <string.h>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <pthread.h>//for timeout on system call
#include <unistd.h>
#include <signal.h>
using namespace std;

//Functions
float getOV();
void deleteFiles();
void buildSpecParamFile();
void printHeader();
void printID();
void *runToolkit(void *);
char* getCurTime();

//Output files for recording data
string baseOutputName = "/kb/module/work/fbafiles/Model_MFA/GAFiles/Output_";
string oname;
//string oname = "/kb/module/work/fbafiles/Model_MFA/GAFiles/OV.txt";
ofstream ofOVfile;
ifstream ifOVfile;
string idname;
//string idname = "/kb/module/work/fbafiles/Model_MFA/GAFiles/ID.txt";
ofstream IDfile;
string cdname;
//string cdname = "/kb/module/work/fbafiles/Model_MFA/GAFiles/combined.txt";
ofstream CDfile;
string tname;
//string tname = "/kb/module/work/fbafiles/Model_MFA/GAFiles/time.txt";
ofstream Tfile;

//File where OV is read
string iname = "/kb/module/work/fbafiles/Model_MFA/tmp/MFAOutput/RawData/RawSolutions.txt";
ifstream inFile;
//Specialized Param File
string spname = "/kb/module/work/fbafiles/Model_MFA/tmp/SpecializedParameters.txt"; 
fstream SPfile;
string paramAddition;
//Model files
string modelname = "/kb/module/work/fbafiles/Model_MFA/tmp/";

//Testing Functionality
//File where input tests are found
//Default is a fixed testing file to verify the program
string iTname = "/kb/module/work/fbafiles/Model_MFA/GAFiles/";
ifstream iTFile;
string ITname;
char itname[100];
void initTests();
void getNextTest(int t);
int pos;
vector < vector <string>> TESTS;
vector <string> HEADER;
vector <int> ID;
vector <float> ORACLE;
int numAttributes;

//Globals
string max_uptake = "";
string replacement;
string replace_call;      
string call;
clock_t clockTicksTaken;
int CurID;
float run_time;
string temp;
float ov;
bool TESTING=false;
bool RUNNING=false;
int NUM_TESTS=0;
pthread_t p;
bool loopCond = true;
long RunID;

//Extra Parms
//bools
int fluxMin     =1;
int KO          =0;
int minMedia    =0;
int rxnRev      =0;
int useVarDrainFlux=0;
int rxnKOSen    =0;
int decompDrain =0;
int decompRxn   =0;
int tightBounds =1;
int phenoFit    =0;
int varForce    =1;
int maxActiveRxn=0;
int newPipeline =1;
int prefMFA     =1;
int rxnUse      =0;
int recMILP     =1;
int useDataFields=1;
int simpVar     =1;
int LPFile      =0;
int varKey      =1;
int optimMetabo =0;
//ints
int maxC;
int maxN;
int maxO;
int maxP;
int maxS;
int maxDrain    =0;
int minDrain    =-1000;
int deltaGSlack =10;
int maxDeltaG   =10000;
int maxFlux     =1000;
int minFluxMult =1;
int minFlux     =-1000;
//floats
float conObj    =.1;
float minTargetFlux=.01;

//=================================================================
//============== Change Range of Values here ======================
//=================================================================
//RANGES
//bools
int fluxMin_range[2]    ={0,1};
int KO_range[2]         ={0,1};
int minMedia_range[2]   ={0,1};
int rxnRev_range[2]     ={0,1};
int useVarDrainFlux_range[2]={0,1};
int rxnKOSen_range[2]   ={0,1};
int decompDrain_range[2]={0,1};
int decompRxn_range[2]  ={0,1};
int tightBounds_range[2]={0,1};
int phenoFit_range[2]   ={0,1};
int varForce_range[2]   ={0,1};
int maxActiveRxn_range[2]={0,1};
int newPipeline_range[2]={0,1};
int prefMFA_range[2]    ={0,1};
int rxnUse_range[2]     ={0,1};
int recMILP_range[2]    ={0,1};
int useDataFields_range[2]={0,1};
int simpVar_range[2]    ={0,1};
int LPFile_range[2]     ={0,1};
int varKey_range[2]     ={0,1};
int optimMetabo_range[2]={0,1};
//ints
int maxC_range[5] = {0,25,50,75,100};
int maxN_range[5] = {0,25,50,75,100};
int maxO_range[5] = {0,25,50,75,100};
int maxP_range[5] = {0,25,50,75,100};
int maxS_range[5] = {0,25,50,75,100};
int maxDrain_range[5]   ={0,250,500,750,1000};
int minDrain_range[5]   ={0,-250,-500,-750,-1000};//default = -1000
int deltaGSlack_range[5]={0,5,10,15,20};//default = 10
int maxDeltaG_range[5]  ={0,2500,5000,7500,10000};//default = 10000
int maxFlux_range[5]    ={0,250,500,750,1000};//default = 1000
int minFluxMult_range[5]={0,1,2,3,4};//default = 1
int minFlux_range[5]    ={0,-250,-500,-750,-1000};//default = -1000
//floats
float conObj_range[5]           ={0,.05,.1,.15,.2};//default = .1
float minTargetFlux_range[5]    ={0,.005,.01,.015,.02};//default = .01


int main ( int argc, char **argv) {
    auto TotalStartTime =std::chrono::high_resolution_clock::now();
    //Check for arguments
    for (int i=1; i<argc; i++)
    {   
        if (strcmp(argv[1], "-T")==0 || strcmp(argv[1], "--test")==0)
        {   
            i++;
            TESTING = true;
            cout << "-----TESTING MODE ON" << endl;
            strcpy(itname, argv[2]);
            initTests();
        }
        if (strcmp(argv[1], "-R")==0 || strcmp(argv[1], "--running")==0)
        {   
            i++;
            RUNNING = true;
            cout << "-----RUNNING MODE ON" << endl;
            strcpy(itname, argv[2]);
            initTests();
        }
    } 

    //set up time counters
	struct timeval time;
   	srand(std::time(NULL));

    //create unique output directory
    //auto IDTime=std::chrono::high_resolution_clock::now();
    auto timeInMillis = std::time(nullptr); //returns long type
    RunID = timeInMillis;
    cout << "Run ID: " << RunID << endl;
    call = "mkdir " + baseOutputName + to_string(RunID);
    system(call.c_str());

    //set up files
    oname=baseOutputName;
    oname+=to_string(RunID);
    oname+="/OV.txt";
    ofOVfile.open(oname);
    cdname=baseOutputName;
    cdname+=to_string(RunID);
    cdname+="/combined.txt";
	CDfile.open(cdname);
    idname=baseOutputName;
    idname+=to_string(RunID);
    idname+="/ID.txt";
	IDfile.open(idname);
    tname=baseOutputName;
    tname+=to_string(RunID);
    tname+="/time.txt";
	Tfile.open(tname);
	
	printHeader();
	
    //================================
	if (!TESTING && !RUNNING)
        NUM_TESTS = 2;
    //===============================
	int count=0;

    cout << "number of runs: " << NUM_TESTS << endl;	
	for (int t=0; t<NUM_TESTS; t++)
	{
		count++;
		cout << count << endl;
        auto startTime =std::chrono::high_resolution_clock::now();
        deleteFiles();
		
		if (!TESTING && !RUNNING){
            //=================
            //Set the params
            //=================
            CurID=t+1;
            fluxMin=fluxMin_range[rand()%2];
            KO=KO_range[rand()%2]; 
            minMedia=minMedia_range[rand()%2];
            rxnRev=rxnRev_range[rand()%2];
            useVarDrainFlux=useVarDrainFlux_range[rand()%2];
            rxnKOSen=rxnKOSen_range[rand()%2];
            decompDrain=decompDrain_range[rand()%2];
            decompRxn=decompRxn_range[rand()%2];
            tightBounds=tightBounds_range[rand()%2];
            phenoFit=phenoFit_range[rand()%2];
            varForce=varForce_range[rand()%2];
            maxActiveRxn=maxActiveRxn_range[rand()%2];
            newPipeline=newPipeline_range[rand()%2];
            prefMFA=prefMFA_range[rand()%2];
            rxnUse=rxnUse_range[rand()%2];
            recMILP=recMILP_range[rand()%2];
            useDataFields=useDataFields_range[rand()%2];
            simpVar=simpVar_range[rand()%2];
            LPFile=LPFile_range[rand()%2];
            varKey=varKey_range[rand()%2];
            optimMetabo=optimMetabo_range[rand()%2];
           
            maxC=maxC_range[rand()%(sizeof(maxC_range)/sizeof(*maxC_range))];
            maxN=maxN_range[rand()%(sizeof(maxN_range)/sizeof(*maxN_range))];
            maxP=maxP_range[rand()%(sizeof(maxP_range)/sizeof(*maxP_range))];
            maxS=maxS_range[rand()%(sizeof(maxS_range)/sizeof(*maxS_range))];
            maxO=maxO_range[rand()%(sizeof(maxO_range)/sizeof(*maxS_range))];
	        max_uptake= "C:" + to_string(maxC) + ";N:" + to_string(maxN) + 
                ";P:" + to_string(maxP) + ";S:" + to_string(maxS) + ";O:" + to_string(maxO);
            maxDrain=maxDrain_range[rand()%(sizeof(maxDrain_range)/sizeof(*maxDrain_range))];
            minDrain=minDrain_range[rand()%(sizeof(minDrain_range)/sizeof(*minDrain_range))];
            deltaGSlack=deltaGSlack_range[rand()%(sizeof(deltaGSlack_range)/sizeof(*deltaGSlack_range))];
            maxDeltaG=maxDeltaG_range[rand()%(sizeof(maxDeltaG_range)/sizeof(*maxDeltaG_range))];
            maxFlux=maxFlux_range[rand()%(sizeof(maxFlux_range)/sizeof(*maxFlux_range))];
            minFluxMult=minFluxMult_range[rand()%(sizeof(minFluxMult_range)/sizeof(*minFluxMult_range))];
            minFlux=minFlux_range[rand()%(sizeof(minFlux_range)/sizeof(*minFlux_range))];
            conObj=conObj_range[rand()%(sizeof(conObj_range)/sizeof(*conObj_range))];
            minTargetFlux=minTargetFlux_range[rand()%(sizeof(minTargetFlux_range)/sizeof(*minTargetFlux_range))];            
        }
        else{
            getNextTest(t);
        }
        //edit the param file
        buildSpecParamFile();
        	
        printID();
         
        auto midTime=std::chrono::high_resolution_clock::now();
	    auto secsBeforeFBA = std::chrono::duration_cast<std::chrono::milliseconds>(midTime- startTime);

		//==================================
		//Call the MFAToolkit (aka FBA)
            //call = "bash /kb/module/work/fbafiles/Model_MFA/runMFAToolkit.sh";
		    //system(call.c_str());
		//Setting up a thread to run the job
        int rc;
        rc = pthread_create(&p,NULL,runToolkit,(void *)NULL);
        if (rc) {
            cout << "Error: unable to create thread" << endl;
            exit(-1);}
        auto toolStartTime =std::chrono::high_resolution_clock::now();
        sleep(1);
        while (loopCond)
        {
            auto toolEndTime =std::chrono::high_resolution_clock::now();
            auto toolElapsed = std::chrono::duration_cast<std::chrono::seconds>(toolEndTime- toolStartTime);
            if (!(pthread_kill(p, 0) == 0))
            {//thread is done
                cout << "Thread is done" << endl;
                loopCond = false;
            }
            //=================================================================Set thread timer here
            else if (toolElapsed > std::chrono::duration<long int>(1000))
            {//thread is over time
                pthread_cancel(p);
                cout << "Thread timeout" << endl;
                loopCond = false;
            } 
            else
            {//thread still has time, wait 5 seconds
                sleep(5);
            }
        }
        loopCond = true;//reset the loop condition
        auto endTime=std::chrono::high_resolution_clock::now();
	    //=================================
	    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime- midTime); //just count call to fbamodule
       	auto curTime=std::chrono::high_resolution_clock::now();
        //Save results to output files
        Tfile << totalSeconds.count() << endl;
		ov=getOV();
	    ofOVfile << ov << endl;
        CDfile << ov << "," << totalSeconds.count() << endl; 	
	    
        //package up the model files (zip)
        //move to the output directory with a runNumber tag on it
        //call = "zip -rq " + baseOutputName + to_string(RunID) + "/" + to_string(CurID) + ".zip "+ modelname;
        cout << "call: " << call << endl;
        system(call.c_str());
        //rm the non-necesary model files
        call = "bash rmfiles.sh";
        system(call.c_str());
    }		
	ofOVfile.close();
	IDfile.close();
	Tfile.close();
	CDfile.close();

    //evaluate tests	
    if(TESTING){
        cout << endl << "-----TESTING RESULTS" << endl;
        bool FAIL=false;
        //Verify tests gave correct OV
        //by compareing to Oracle
        ifOVfile.open(oname);
        for(int i=0;i<ORACLE.size();i++){
            ifOVfile >> ov;
            cout << "ov: " << ov << endl;
            if(ov != ORACLE[i]){
                FAIL=true;
                cout << "FAILED TEST --- #" << i << endl;
                cout << "     ov: " << ov << "   ORACLE: " << ORACLE[i] << endl;
            }            
        }
        if (FAIL)
            return 1;
        else
            cout << " -----All tests passed" << endl;
    	ifOVfile.close();
    }

    //Final time printouts
    cout << "Cur Time: " << getCurTime() << endl;
    auto TotalEndTime =std::chrono::high_resolution_clock::now();
    auto TotalElapsedsec =std::chrono::duration_cast<std::chrono::seconds>(TotalEndTime- TotalStartTime);
    auto TotalElapsedmin =std::chrono::duration_cast<std::chrono::minutes>(TotalEndTime- TotalStartTime);
    auto TotalElapsedhr  =std::chrono::duration_cast<std::chrono::hours>(TotalEndTime- TotalStartTime);
    cout << "Total seconds: " << TotalElapsedsec.count() << endl;
    cout << "Total minutes: " << TotalElapsedmin.count() << endl;
    cout << "Total hours:   " << TotalElapsedhr.count()  << endl;
    return 0;
}


void *runToolkit(void *params)
{
    call = "bash /kb/module/work/fbafiles/Model_MFA/runMFAToolkit.sh";
    system(call.c_str());
    pthread_exit(NULL);
}

float getOV()
{//Obtain the objective value from the rawSolutions files 
	ov=0.0;
	inFile.open(iname);
	if (!inFile)
		cout << "Output File not found" << endl;
	else
	{
		getline(inFile,temp);
		getline(inFile,temp);
		pos = temp.rfind(";");
		ov = stof(temp.substr(pos+1,temp.length()));
      	cout << "  OV=" << ov << endl;
    }
  	inFile.close();
  
 	return ov;
}

void deleteFiles()
{//Delete MFA input files to ensure correctness
	remove("/kb/module/work/fbafiles/Model_MFA/tmp/MFAOutput/CombinationKO.txt");
	remove("/kb/module/work/fbafiles/Model_MFA/tmp/MFAOutput/SolutionCompoundData.txt");
	remove("/kb/module/work/fbafiles/Model_MFA/tmp/MFAOutput/TightBoundsCompoundData.txt");
	remove("/kb/module/work/fbafiles/Model_MFA/tmp/MFAOutput/TightBoundsReactionData.txt");
	remove("/kb/module/work/fbafiles/Model_MFA/tmp/MFAOutput/RawData/RawSolutions.txt");
	remove("/kb/module/work/fbafiles/Model_MFA/tmp/MFAOutput/RawData/RawTightBounds0.txt");
}

void buildSpecParamFile()
{//Replace the Parm files with the new values
    //Begin with the template file
    system("cp /kb/module/work/fbafiles/Model_MFA/tmp/SpecializedParameters.templateModel.txt /kb/module/work/fbafiles/Model_MFA/tmp/SpecializedParameters.txt");

    //paramAddition  = "\n";
    //31 parameter here
    paramAddition += "flux minimization|"+to_string(fluxMin)+"|Specialized parameters\n";
    paramAddition += "perform single KO experiments|"+to_string(KO)+"|Specialized parameters\n";
    paramAddition += "uptake limits|" + max_uptake + "|Specialized parameters\n";
    paramAddition += "determine minimal required media|"+to_string(minMedia)+"|Specialized parameters\n";
    paramAddition += "Make all reactions reversible in MFA|"+to_string(rxnRev)+"|Specialized parameters\n";
    paramAddition += "Add use variables for any drain fluxes|"+to_string(useVarDrainFlux)+"|Specialized parameters\n";
    paramAddition += "calculate reaction knockout sensitivity|"+to_string(rxnKOSen)+"|Specialized parameters\n";
    paramAddition += "Decompose reversible drain fluxes|"+to_string(decompDrain)+"|Specialized parameters\n";
    paramAddition += "Decompose reversible reactions|"+to_string(decompRxn)+"|Specialized parameters\n";
    paramAddition += "find tight bounds|"+to_string(tightBounds)+"|Specialized parameters\n";
    paramAddition += "fit phenotype data|"+to_string(phenoFit)+"|Specialized parameters\n";
    paramAddition += "Force use variables for all reactions|"+to_string(varForce)+"|Specialized parameters\n";
    paramAddition += "maximize active reactions|"+to_string(maxActiveRxn)+"|Specialized parameters\n";
    paramAddition += "new fba pipeline|"+to_string(newPipeline)+"|Specialized parameters\n";
    paramAddition += "perform MFA|"+to_string(prefMFA)+"|Specialized parameters\n";
    paramAddition += "Reactions use variables|"+to_string(rxnUse)+"|Specialized parameters\n";
    paramAddition += "Recursive MILP solution limit|"+to_string(recMILP)+"|Specialized parameters\n";
    paramAddition += "use database fields|"+to_string(useDataFields)+"|Specialized parameters\n";
    paramAddition += "use simple variable and constraint names|"+to_string(simpVar)+"|Specialized parameters\n";
    paramAddition += "write LP file|"+to_string(LPFile)+"|Specialized parameters\n";
    paramAddition += "write variable key|"+to_string(varKey)+"|Specialized parameters\n";
    paramAddition += "Default max drain flux|"+to_string(maxDrain)+"|Specialized parameters\n";
    paramAddition += "Default min drain flux|"+to_string(minDrain)+"|Specialized parameters\n";
    paramAddition += "deltagslack|"+to_string(deltaGSlack)+"|Specialized parameters\n";
    paramAddition += "Max deltaG|"+to_string(maxDeltaG)+"|Specialized parameters\n";
    paramAddition += "Max flux|"+to_string(maxFlux)+"|Specialized parameters\n";
    paramAddition += "Min flux multiplier|"+to_string(minFluxMult)+"|Specialized parameters\n";
    paramAddition += "Min flux|"+to_string(minFlux)+"|Specialized parameters\n";
    paramAddition += "Constrain objective to this fraction of the optimal value|"+to_string(conObj)+"|Specialized parameters\n";
    paramAddition += "minimum_target_flux|"+to_string(minTargetFlux)+"|Specialized parameters\n";
    paramAddition += "optimize metabolite production if objective is zero|"+to_string(optimMetabo)+"|Specialized parameters\n";

    //Write new text to file
    //cout << paramAddition;
    SPfile.open(spname,std::fstream::out | std::fstream::app);
    SPfile << paramAddition;  
    SPfile.close();
}

void getNextTest(int t)
{//Set values of params when in TESTING mode
    CurID=ID[t];
    max_uptake = "";
    for (int i=0; i<numAttributes; i++){
        if (HEADER[i]=="maxC"){
            maxC=stoi(TESTS[t][i]);
            if (max_uptake == "") max_uptake += "C:" + TESTS[t][i];
            else max_uptake += ";C:" + TESTS[t][i];}
        else if (HEADER[i]=="maxN"){
            maxN=stoi(TESTS[t][i]);
            if (max_uptake == "") max_uptake += "N:" + TESTS[t][i];
            else max_uptake += ";N:" + TESTS[t][i];}
        else if (HEADER[i]=="maxO"){
            maxO=stoi(TESTS[t][i]);
            if (max_uptake == "") max_uptake += "O:" + TESTS[t][i];
            else max_uptake += ";O:" + TESTS[t][i];}
        else if (HEADER[i]=="maxP"){
            maxP=stoi(TESTS[t][i]);
            if (max_uptake == "") max_uptake += "P:" + TESTS[t][i];
            else max_uptake += ";P:" + TESTS[t][i];}
        else if (HEADER[i]=="maxS"){
            maxS=stoi(TESTS[t][i]);
            if (max_uptake == "") max_uptake += "S:" + TESTS[t][i];
            else max_uptake += ";S:" + TESTS[t][i];}
        else if (HEADER[i]=="minFlux"){
            minFlux = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="KO"){
            KO = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="tightBounds"){
            tightBounds = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="varForce"){
            varForce = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="maxDrain"){
            maxDrain = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="fluxMin"){
            fluxMin = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="minMedia"){
            minMedia = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="rxnKOSen"){
            rxnKOSen = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="recMILP"){
            recMILP = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="varKey"){
            varKey = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="phenoFit"){
            phenoFit = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="useVarDrainFlux"){
            useVarDrainFlux = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="simpVar"){
            simpVar = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="useDataFields"){
            useDataFields = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="rxnRev"){
            rxnRev = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="decompDrain"){
            decompDrain = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="decompRxn"){
            decompRxn = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="newPipeline"){
            newPipeline = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="maxActiveRxn"){
            maxActiveRxn = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="LPFile"){
            LPFile = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="prefMFA"){
            prefMFA = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="optimMetabo"){
            optimMetabo = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="rxnUse"){
            rxnUse = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="minDrain"){
            minDrain = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="deltaGSlack"){
            deltaGSlack = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="maxDeltaG"){
            maxDeltaG = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="maxFlux"){
            maxFlux = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="minFluxMult"){
            minFluxMult = stoi(TESTS[t][i]);}
        else if (HEADER[i]=="conObj"){
            conObj = stof(TESTS[t][i]);}
        else if (HEADER[i]=="minTargetFlux"){
            minTargetFlux = stof(TESTS[t][i]);}
        else
            cout << "WARNING----  Param in Test NOT in code: " << HEADER[i] << endl;
    }
}

void initTests()
{//initalize TESTING mode
 //read in the input tests into vectors
    int tempI;
    ITname = iTname;
    for (int i=0; i<strlen(itname); i++)
        ITname+=itname[i];
    iTFile.open(ITname);
    if (!iTFile){
        cout << "Input test file " + ITname + " not found" << endl;
        exit(1);}
    float tempf;
    //set up header
    string head;
    iTFile >> numAttributes;
    iTFile >> head;
    //HEADER.push_back(head);//to catch the ID label
    for(int i=0; i<numAttributes; i++){
        iTFile >> head;
        HEADER.push_back(head);}  
    iTFile >> head;//to catch the OV label 
    //fill in the tests
    vector <string> tempTests;
    while(iTFile >> tempI){
        NUM_TESTS++;
        ID.push_back(tempI);//get ID tag
        //tempTests.push_back(temp);
        for(int i=0; i<numAttributes; i++){
            iTFile >> temp;
            tempTests.push_back(temp);}
        //Add oracle OV value to vector
        iTFile >> tempf;
        ORACLE.push_back(tempf);
        TESTS.push_back(tempTests);
        tempTests.clear();
    }
    iTFile.close();
}

void printHeader()
{
    CDfile << "ID,fluxMin,KO,minMedia,rxnRev,useVarDrainFlux,rxnKOSen,decompDrain,decompRxn,tightBounds,";
    CDfile << "phenoFit,varForce,maxActiveRxn,newPipeline,prefMFA,rxnUse,recMILP,useDataFields,simpVar,";
    CDfile << "LPFile,varKey,optimMetabo,";
    CDfile << "maxC,maxN,maxO,maxP,maxS,maxDrain,minDrain,deltaGSlack,";
    CDfile << "maxDeltaG,maxFlux,minFluxMult,minFlux,conObj,minTargetFlux,";
    CDfile << "OV,time" << endl;
}

void printID()
{//print to IDfile and CDfile
    CDfile << CurID<<","<<fluxMin<<","<<KO<<","<<minMedia<<","<<rxnRev<<","<<useVarDrainFlux<<",";
    CDfile << rxnKOSen<<","<<decompDrain<<","<<decompRxn<<","<<tightBounds<<",";
    CDfile << phenoFit<<","<<varForce<<","<<maxActiveRxn<<","<<newPipeline<<",";
    CDfile << prefMFA<<","<<rxnUse<<","<<recMILP<<","<<useDataFields<<","<<simpVar<<",";
    CDfile << LPFile<<","<<varKey<<","<<optimMetabo<<",";
    CDfile << maxC<<","<<maxN<<","<<maxO<<","<<maxP<<","<<maxS<<","<<maxDrain<<","<<minDrain<<","<<deltaGSlack<<",";
    CDfile << maxDeltaG<<","<<maxFlux<<","<<minFluxMult<<","<<minFlux<<","<<conObj<<","<<minTargetFlux<<",";
 
    IDfile << CurID<<","<<fluxMin<<","<<KO<<","<<minMedia<<","<<rxnRev<<","<<useVarDrainFlux<<",";
    IDfile << rxnKOSen<<","<<decompDrain<<","<<decompRxn<<","<<tightBounds<<",";
    IDfile << phenoFit<<","<<varForce<<","<<maxActiveRxn<<","<<newPipeline<<",";
    IDfile << prefMFA<<","<<rxnUse<<","<<recMILP<<","<<useDataFields<<","<<simpVar<<",";
    IDfile << LPFile<<","<<varKey<<","<<optimMetabo<<",";
    IDfile << maxC<<","<<maxN<<","<<maxO<<","<<maxP<<","<<maxS<<","<<maxDrain<<","<<minDrain<<","<<deltaGSlack<<",";
    IDfile << maxDeltaG<<","<<maxFlux<<","<<minFluxMult<<","<<minFlux<<","<<conObj<<","<<minTargetFlux<<endl;

    cout << CurID<<","<<fluxMin<<","<<KO<<","<<minMedia<<","<<rxnRev<<","<<useVarDrainFlux<<",";
    cout << rxnKOSen<<","<<decompDrain<<","<<decompRxn<<","<<tightBounds<<",";
    cout << phenoFit<<","<<varForce<<","<<maxActiveRxn<<","<<newPipeline<<",";
    cout << prefMFA<<","<<rxnUse<<","<<recMILP<<","<<useDataFields<<","<<simpVar<<",";
    cout << LPFile<<","<<varKey<<","<<optimMetabo<<",";
    cout << maxC<<","<<maxN<<","<<maxO<<","<<maxP<<","<<maxS<<","<<maxDrain<<","<<minDrain<<","<<deltaGSlack<<",";
    cout << maxDeltaG<<","<<maxFlux<<","<<minFluxMult<<","<<minFlux<<","<<conObj<<","<<minTargetFlux<<endl;
}

char* getCurTime()
{
   time_t now = time(0);
   tm *gmtm = gmtime(&now);
   return asctime(gmtm);
}

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include <errno.h>

#include "My_Constants.h"
#include "Parameters.h"
#include "Pattern.h"
#include "Algorithm.h"
#include "Deserializer.h"
#include "Automatic_Run.h"

#include "CooccurranceAnomaliesDetector.h"


using namespace std;

int main() {
    
    Parameters parameters; 
    Automatic_Run runner;
    string inputname;
    string modeSelection;
    
    CooccurranveAnomaliesDetector score2Detector;
    
    //--------------------Check Results Directory-------------------------------
    DIR* dir = opendir("Results");
    if (ENOENT == errno) {
        CreateDirectory( "Results", NULL );
        cout<<"create a directory for results \n";
    }
    //--------------Delete Old Files if Exist-----------------------------------
    auto res = DeleteFile("CodeTable.json");
    if(res==1)
        cout<<"old CodeTable.json was successfully deleted\n";
    res = DeleteFile("doubleCodeTable.json");
    if(res==1)
        cout<<"old doubleCodeTable.json was successfully deleted\n";
    //-----------------------User mode Input------------------------------------
    cout << "choose the algorithm, enter 1 for SQS and 2 for SQUISH \n";
    bool repeat = true;
    while (repeat){
        getline(cin, modeSelection);
        if (modeSelection.compare("1") == 0){
            parameters.mode = SQS;
            repeat = false;
        }
        else if (modeSelection.compare("2")== 0){
            parameters.mode = SQUISH;
            repeat = false;
        }
        else
            cout<<"incorrect input, please repeat \n";
    }
    //---------------------------User Input-------------------------------------
    cout << "enter input file name \n";
    getline(cin, inputname);
    parameters.inputFileName.assign(inputname);
    parameters.outputFileName.assign("SQS_Code_Table.txt");  // only for SQS
    cout<<"=================================================================\n";
    
    //---------------------Running Algorithm/first call-------------------------
    char command[1000]="";
    if (parameters.mode == SQS){
        cout<<"Running SQS - first call\n";
        string passedParameters ="-o " + parameters.outputFileName + " -i "+parameters.inputFileName +" -f "+ "t";
        strcat(command, "/cygdrive/c/Users/Administrator/Documents/NetBeansProjects/SQS/dist/Debug/Cygwin_1-Windows/sqs.exe ");
        strcat(command,passedParameters.c_str());
    }
    else{
        cout<<"Running SQUISH - first call\n";
        string passedParameters = "-i "+parameters.inputFileName +" -f "+ "t";
        strcat(command, "/cygdrive/c/Users/Administrator/Documents/NetBeansProjects/SQUISH/dist/Debug/Cygwin_1-Windows/squish.exe ");
        strcat(command,passedParameters.c_str());
    }
        
    int result = system(command);

    cout<<"=================================================================\n";
    
    //-------------------------Parsing Input File-------------------------------
    cout<<"Parsing input file\n";
    FILE *file;
    file = fopen(parameters.inputFileName.c_str(), "r");
    if(!file){
        cout<<"ERROR opening data file: "<< parameters.inputFileName <<"\n";
        return 1;
    }
    else{
        parameters.algorithmItem.readInputFile(file);
        fclose(file);
        cout<<"attributes = "<<parameters.algorithmItem.attributesNumber<<", events = "<<parameters.algorithmItem.eventsNumber<<", sequences = "<<parameters.algorithmItem.sequencesNumber<<"\n";
    }
    cout<<"=================================================================\n";
    
    //--------------------------------Deserializing------------------------------------

    Deserializer deserializer;
    ifstream jsonFile("CodeTable.json");

    if(!jsonFile){
        cout<<"Couldn't open JSON file\n";
        return 1;
    }
    else{
        if(parameters.mode == SQS)
            deserializer.deserializeSQSCodeTable(jsonFile, parameters.algorithmItem);
        else if(parameters.mode ==SQUISH)
            deserializer.deserializeSquishCodeTable(jsonFile, parameters.algorithmItem);
    }
    
    
    jsonFile.close();
    
    if(parameters.mode == SQS){
        ifstream jsonFile2("orderedMinWindows.json");
        if(!jsonFile2){
            cout<<"Couldn't open JSON file\n";
            return 1;
        }   
        else{
            deserializer.deserializeSQSOrderedMinWindows(jsonFile2, parameters.algorithmItem);
        }
        jsonFile2.close();
        cout<<"number of ordered min windows = "<<parameters.algorithmItem.orderedMinWindows.size()<<"\n";
    }


    cout<<"=================================================================\n";
    
    //--------------------------------Building ST------------------------------------
    cout<<"building ST Patterns \n";
    FILE *sFile;
    sFile = fopen(parameters.inputFileName.c_str(), "r");
    if(!file){
        cout<<"ERROR opening data file: "<< parameters.inputFileName <<"\n";
        return 1;
    }
    else{
        parameters.algorithmItem.buildSTPatterns(sFile);
        parameters.algorithmItem.writeSTPatternstoFile();
        fclose(sFile);
    }
    parameters.algorithmItem.calculateCTCodeLength();
    cout<<"==================================================================\n";
    
    //--------------------------------User Input-------------------------------------
    string mainChoice;
    bool WindowApproachSelected;
    repeat = true;
    cout<<"Select the main choice, enter 1 for Score1 or 2 for Score2\n";
    while (repeat){
        getline(cin, mainChoice);
        if (mainChoice.compare("1") == 0){
            WindowApproachSelected = true;
            repeat = false;
        }
        else if (mainChoice.compare("2")==0){
            WindowApproachSelected = false;
            repeat = false;
        }
        else
            cout<<"incorrect input, please repeat \n";
    }
    cout<<"=================================================================\n";
    
     if (!WindowApproachSelected){
         score2Detector.runScore2(parameters);
    }
    else{
        bool splittedWindowsSelected;
        bool repeat = true;
        cout<<"Enter 1 for Split Windows Approach or 2 for Sliding Windows Approach\n";
        while (repeat){
            getline(cin, mainChoice);
            if (mainChoice.compare("1") == 0){
                splittedWindowsSelected = true;
                repeat = false;
            }
            else if (mainChoice.compare("2")==0){
                splittedWindowsSelected = false;
                repeat = false;
            }
            else
                cout<<"incorrect input, please repeat \n";
        }
        int k;
        cout<<"enter window size, enter 0 to run for all possible sizes automatically\n";
        cin>>k;
        if(k>0){
            if(!splittedWindowsSelected)
                runner.RunScore1ForOneWindowSize(parameters, k, false);
            else
                runner.RunScore1ForOneWindowSize(parameters, k, true);
            char plotCommand1[1000]="";
            char plotCommand2[1000]="";
            char plotCommand3[1000]="";
            strcat(plotCommand1,"gnuplot -e \"set terminal png size 2000,1200; set key right bottom; set output 'Results\\windowContent.png'; set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5; set pointintervalbox 3; plot 'Results\\windowContent.txt' title 'Window CL - Content' with linespoints ls 1 \"");
            int plotResult1 = system(plotCommand1);
            strcat(plotCommand2,"gnuplot -e \"set terminal png size 2000,1200; set key right bottom; set output 'Results\\futureProbabilities.png'; set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5; set pointintervalbox 3; plot 'Results\\futureProbabilities.txt' title 'Window CL - Future Probabilities' with linespoints ls 1 \"");
            int plotResult2 = system(plotCommand2);
            strcat(plotCommand3,"gnuplot -e \"set terminal png size 2000,1200; set key right bottom; set output 'Results\\pastAndFutureProbabilities.png'; set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5; set pointintervalbox 3; plot 'Results\\pastAndFutureProbabilities.txt' title 'Window CL - Past and Future Probabilities' with linespoints ls 1 \"");
            int plotResult3 = system(plotCommand3);
        } 
        else{
            if(!splittedWindowsSelected)
                runner.RunScore1(parameters, false);
            else
                runner.RunScore1(parameters, true);
        }  
        cout<<"results are printed into text files\n";
    }

    return 0;
}

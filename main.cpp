/* 
 * File:   main.cpp
 * Author: Sandra
 *
 * Created on December 22, 2019, 7:46 PM
 */

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


using namespace std;

/*
 * 
 */
int main() {
    
    Parameters parameters; 
    Automatic_Run runner;
    string inputname;
    string modeSelection;
    
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
    cout << "choose the algorithm, enter 1 or 2 \n";
    bool repeat = true;
    while (repeat){
        getline(cin, modeSelection);
        if (modeSelection.compare("1") == 0){
            parameters.mode = algorithm1;
            repeat = false;
        }
        else if (modeSelection.compare("2")== 0){
            parameters.mode = algorithm2;
            repeat = false;
        }
        else
            cout<<"incorrect input, please repeat \n";
    }
    //---------------------------User Input-------------------------------------
    cout << "enter input file name \n";
    getline(cin, inputname);
    parameters.inputFileName.assign(inputname);
    parameters.outputFileName.assign("Code_Table.txt");
    cout<<"=================================================================\n";
    
    //---------------------Running Algorithm/first call-------------------------
    char command[1000]="";
    if (parameters.mode == algorithm1){
        cout<<"Running algorithm1 - first call\n";
        string passedParameters =" -o" + parameters.outputFileName + " -i "+parameters.inputFileName +" -f "+ "t";
        strcat(command, "/cygdrive/c/Users/Administrator/Documents/CodeBlocksProjects/algorithm1/bin/Debug/algorithm1.exe ");
        strcat(command,passedParameters.c_str());
    }
    else{
        cout<<"Running algorithm2 - first call\n";
        string passedParameters = " -i "+parameters.inputFileName +" -f "+ "t";
        strcat(command, "/cygdrive/c/Users/Administrator/Documents/CodeBlocksProjects/algorithm2/bin/Debug/algorithm2.exe ");
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

    ifstream jsonFile("CodeTable.json");
    Deserializer deserializer;

    if(!jsonFile){
        cout<<"Couldn't open JSON file\n";
        return 1;
    }
    else{
        deserializer.deserializeSQSCodeTable(jsonFile, parameters.algorithmItem);
    };
    jsonFile.close();

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
            //--------------Running Algorithm/second call-----------------------
            char command[1000]="";
            if (parameters.mode == algorithm1){
                cout<<"Running algorithm1 - second call\n";
                string passedParameters =" -o" + parameters.outputFileName + " -i "+parameters.inputFileName +" -f "+ "f";
                strcat(command, "/cygdrive/c/Users/Administrator/Documents/CodeBlocksProjects/algorithm1/bin/Debug/algorithm1.exe ");
                strcat(command,passedParameters.c_str());
            }
            else{
                cout<<"Running algorithm2 - second call\n";
                string passedParameters = " -i "+parameters.inputFileName +" -f "+ "f";
                strcat(command, "/cygdrive/c/Users/Administrator/Documents/CodeBlocksProjects/algorithm2/bin/Debug/algorithm2.exe ");
                strcat(command,passedParameters.c_str());
            }
            int result = system(command);

            cout<<"=========================================================\n";

            //----------------------Deserializing-------------------------------
            ifstream jsonFile2("doubleCodeTable.json");
            Deserializer deserializer2;
            if(!jsonFile2){
                    cout<<"Couldn't open JSON file \n";
                    return 1;
            }
            else{
                deserializer.deserializeSQSDoubleCT(jsonFile2, parameters.algorithmItem);
                cout<<"number of CT*CT deserialized patterns = "<<parameters.algorithmItem.CTxCT.size()<<"\n";
            }
            cout<<"------------------------------------------\n";

            //---------------------Filtering CT*CT------------------------------
            cout<<"filtering CTxCT \n";
            parameters.algorithmItem.filterDoubleCT();
            //--------------------Anomaly Detection-----------------------------
            runner.RunScore2(parameters);
            cout<<"\n";
            cout<<"results are printed into text files\n";            
    }
    else{
        int k;
        cout<<"enter window size, enter 0 to run for all possible sizes automatically\n";
        cin>>k;
        if(k>0){
            runner.RunScore1SlidingWindowsForOneWindowSize(parameters, k);
            char plotCommand1[1000]="";
            char plotCommand2[1000]="";
            char plotCommand3[1000]="";
            strcat(plotCommand1,"gnuplot -e \"set terminal png size 800,600; set output 'windowContent.png'; plot 'windowContent.txt'\"");
            int plotResult1 = system(plotCommand1);
            strcat(plotCommand2,"gnuplot -e \"set terminal png size 800,600; set output 'futureProbabilities.png'; plot 'futureProbabilities.txt'\"");
            int plotResult2 = system(plotCommand2);
            strcat(plotCommand3,"gnuplot -e \"set terminal png size 800,600; set output 'pastAndFutureProbabilities.png'; plot 'pastAndFutureProbabilities.txt'\"");
            int plotResult3 = system(plotCommand3);
        }
           
        else
            runner.RunScore1SlidingWindows(parameters);
        cout<<"results are printed into text files\n";

    }

    return 0;
}
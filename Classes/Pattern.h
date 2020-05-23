/* 
 * File:   Pattern.h
 * Author: Sandra
 *
 * Created on December 23, 2019, 8:00 PM
 */

#ifndef PATTERN_H
#define PATTERN_H


#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <set>
#include <list>
#include <string>
#include <iostream>
#include <string.h>

#include "Window.h"
#include "STItem.h"


using namespace std;

class Pattern
{
    public:

        Pattern (int patternSize, int patternSupport, string symbols); //use for the first and second patterns for Ct*CT
        Pattern (int patternSize, int patternSupport, string symbols, int patternUsage, list <Window*> windows);
        Pattern (int patternSize, int patternSupport, string symbols, int patternUsage, list <Window*> windows, Pattern* firstP, Pattern* secondP);

        void printPattern();
        void printPatternMinWindows();
        bool checkSubset (Pattern *otherPattern);
        bool isUsageLess (Pattern *otherPattern);
        bool checkPatternAttributeOverlap(Pattern *otherPattern, int offset);
        void calculateFirstandSecondProbabilities(vector<Pattern*> CTPatterns, vector<STItem> STPatterns);
        bool smallerThan (Pattern *otherPattern);
        bool biggerThan (Pattern *otherPattern);
        bool smallerOrEqual (Pattern *otherPattern);
        bool biggerOrEqual (Pattern *otherPattern);
        bool equals (Pattern *otherPattern);
        bool notEqual (Pattern *otherPattern);
        void printPatternEvents();
        bool checkForRedundancy(Pattern *otherPattern);
        bool checkSubsetinPosition(Window *window1, Window *window2);


        string patternSymbols;

        int patternLength, //the number of multi-events it contains (equals to size for the univariate case)
            patternSupport,
            patternSize, //number of events it contains
            patternUsage,
            patternNewUsage;

        double patternProbability,
               patternCodeLength,
               patternAnomalyScore,
               patternSTSize;


        list <Window*> minWindows;

        Pattern *firstPattern, *secondPattern;

        //EventSet patternEventSet;
};



#endif /* PATTERN_H */


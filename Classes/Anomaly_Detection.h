#ifndef ANOMALY_DETECTION_H
#define ANOMALY_DETECTION_H

#include <fstream>
#include<iostream>
#include <math.h> 

#include "Algorithm.h"
#include "Parameters.h"
#include "SlidingWindow.h"

using namespace std;

class Anomaly_Detection
{
    public:

        void computeAnomalyScores (Parameters parameters, string fileName, int windowLength, int totalWindows, int *totalWindowsPerSequence);
        void computeCTProbabilities (Parameters parameters, int totalWindows , int windowLength , int *totalWindowsPerSequence);
        void computeSTProbabilities (Parameters &parameters, int totalWindows , int windowLength , int *totalWindowsPerSequence);
        int computeNrWindows(Parameters parameters , Pattern *p, int windowLength, int *totalWindowsPerSequence);
        int computeNrWindowsForST (Parameters parameters , Pattern *p, int windowLength, int *totalWindowsPerSequence);
        int countAlignments(Pattern *x, Pattern *y);
        void insertOrdered(list<Pattern*>* lst, Pattern *cand);
        void removeRedundancy(list<Pattern*> *ranking);
        int findRedundantPattern(list<Pattern*> *ranking, int currentPatternPosition);
        void writeAnomaliesToTextFile(list<Pattern*> *ranking, string fileName);
        
        vector<coveringWindowEvent> findCommonSuspiciousEvents(vector<SlidingWindow> sortedCoveringWindows);
        void findOverlappedWindows(vector<SlidingWindow> analyzedWindows, vector<vector<SlidingWindow>> &overlappedWindowsSet);
        bool isOverlapped(SlidingWindow w1, SlidingWindow w2);
};

#endif /* ANOMALY_DETECTION_H */


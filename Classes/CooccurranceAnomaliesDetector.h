#ifndef COOCCURANCEANOMALIESDETECTOR_H
#define COOCCURANCEANOMALIESDETECTOR_H

#include<iostream>
#include<list>
#include<utility> // for pair objects
#include<vector>
#include <fstream>


#include "Parameters.h" 

using namespace std;

class CooccurranveAnomaliesDetector{
public:
    void runScore2(Parameters parameters);
    void findOccurrencesForCTPatterns(Parameters &parameters);
    void findOccurrencesForSTPatterns(Parameters &parameters);
    vector<int> findEventsNotCoveredWithCT(Parameters parameters);
    vector<int> getInteractionsIds(vector<SlidingWindow> firstPatternWindows, vector<SlidingWindow> secondPatternWindows);
};

#endif /* COOCCURANCEANOMALIESDETECTOR_H */


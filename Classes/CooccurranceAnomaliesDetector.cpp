#include "CooccurranceAnomaliesDetector.h"


void CooccurranveAnomaliesDetector::runScore2(Parameters parameters){
    
    int patternsCount = parameters.algorithmItem.CTPatterns.size()+parameters.algorithmItem.STPatterns.size();
    
    int** coOccurrances = new int*[patternsCount];  // to save the number of windows that the pair of patterns appears in
    for(int i=0; i<patternsCount;i++){
        coOccurrances[i] = new int[patternsCount - i - 1];
        for (int j=i+1; j<patternsCount; j++)
            coOccurrances[i][j - i - 1] = 0;      
    }
   
    
    double** combiCodeLength = new double*[patternsCount]; // to save the probabilities of the patterns combinations
    double* ctCodeLength = new double[patternsCount]; // to save the probabilities of the patterns
    
    // find the longest pattern in CT
    int maxLength=0;
    for (int i=0;i<parameters.algorithmItem.CTPatterns.size();i++){
        if (parameters.algorithmItem.CTPatterns[i]->patternSize > maxLength)
            maxLength= parameters.algorithmItem.CTPatterns[i]->patternSize;
    }
    int windowLength = 4* maxLength -1;
    
    FILE *sFile;
    sFile = fopen(parameters.inputFileName.c_str(), "r");
    if(!sFile)
        cout<<"ERROR opening data file: "<< parameters.inputFileName <<"\n";
    else
        parameters.algorithmItem.buildSlidingWindows(sFile, windowLength);

    findOccurrencesForCTPatterns(parameters);  
    findOccurrencesForSTPatterns(parameters);
    
    int windowsCount = parameters.algorithmItem.dataCoveringWindows.size();
    
    vector<vector<pair<int,int>>> coOccurrancesPerWindow(windowsCount);  // to save all pairs of patterns occurring in each window
    
    
    // gather all patterns in one container to create combinations
    vector<Pattern *> listOfPatterns;
    for(Pattern *p: parameters.algorithmItem.CTPatterns)
        listOfPatterns.push_back(p);
    for (Pattern *s: parameters.algorithmItem.STPatterns)
        listOfPatterns.push_back(s);
    
    
    // find each pair per window
    for(int i=0; i<patternsCount; i++){
        Pattern *a = listOfPatterns.at(i);
        for (int j=i+1; j< patternsCount; j++){
            Pattern* b = listOfPatterns.at(j);
            vector<int> result = getInteractionsIds(a->occurrencesInWindows, b->occurrencesInWindows);
            for (int wid: result)
                coOccurrancesPerWindow[wid].push_back(make_pair(i, j));    // save the pair of patterns for this window
            coOccurrances[i][j-i-1] += result.size();  // save the count of co-occurrence windows for this pair of patterns
        }
    }
    
    // calculate the probability of each pair of patterns
    double** pairsProbabilities = new double*[patternsCount];
    for(int i = 0; i < patternsCount; ++i){
        pairsProbabilities[i] = new double[patternsCount-i-1];	
        for(int j = i+1; j < patternsCount; ++j){	//NOTE: only upper diagonal
            pairsProbabilities[i][j-i-1] = -log2((double)coOccurrances[i][j-i-1]/(double)parameters.algorithmItem.dataCoveringWindows.size());	
        }
    }
    
    // calculate the probability of each pattern
    double* patternsProbabilities = new double[patternsCount];
    for(int i = 0; i < patternsCount; ++i){
        patternsProbabilities[i] = -log2((double)listOfPatterns[i]->occurrencesInWindows.size()/(double)parameters.algorithmItem.dataCoveringWindows.size());
    }
    
    struct suspisioucPair{
        double score = 0;
        pair<int, int> patterns;
    };
    
    vector<vector<pair<int,int>>> mostUnlikelyPairPerWindow(windowsCount);  // to save all pairs of patterns occurring in each window
    
    //compute score per window		
    double *maxScore = new double[parameters.algorithmItem.dataCoveringWindows.size()];	//per transactions
    double score;
    for(int wid = 0; wid < parameters.algorithmItem.dataCoveringWindows.size(); wid++){
        maxScore[wid] = 0;
        for(auto patternPair: coOccurrancesPerWindow[wid]){
            score = -pairsProbabilities[patternPair.first][patternPair.second- patternPair.first - 1] + patternsProbabilities[patternPair.first] + patternsProbabilities[patternPair.second];
            if (score > maxScore[wid]){
                maxScore[wid] = score;	
                mostUnlikelyPairPerWindow[wid].push_back(make_pair(patternPair.first, patternPair.second));  //save the pattern pair causing the highest score 
            }
        }
        
    }
    
    for(int i = 0;i<mostUnlikelyPairPerWindow.size();i++){

        if (mostUnlikelyPairPerWindow.at(i).size() != 0){
            int lastIndex = mostUnlikelyPairPerWindow.at(i).size()-1;
            int firsPattern = mostUnlikelyPairPerWindow.at(i).at(lastIndex).first;
            int secondPattern = mostUnlikelyPairPerWindow.at(i).at(lastIndex).second;
            cout<<i<<": "<<firsPattern<<", "<<secondPattern<<" - ";
            cout<<"patterns: "<<listOfPatterns.at(firsPattern)->patternSymbols<<", "<<listOfPatterns.at(secondPattern)->patternSymbols<<" - ";
            cout<<"score = "<<maxScore[i]<<"\n";
        }
    }
    
    ofstream myfile;
    myfile.open("Results\\windowsScore2.txt", fstream::trunc);
    for (int i=0;i<mostUnlikelyPairPerWindow.size();i++){
        myfile<<i<<" "<<maxScore[i]<<"\n";
    }
    myfile.close();
    
     char plotCommand[1000]="";
    strcat(plotCommand,"gnuplot -e \"set terminal png size 2000,1200; set key right bottom; set output 'Results\\windowsScore2.png'; set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 pi -1 ps 1.5; set pointintervalbox 3; plot 'Results\\windowsScore2.txt' title 'Window Score 2' with linespoints ls 1 \"");
    int plotResult = system(plotCommand);
       
    
}

// get the occurrences (sliding windows) for each of the CT patterns
void CooccurranveAnomaliesDetector::findOccurrencesForCTPatterns(Parameters &parameters){
    
    for(SlidingWindow sw:parameters.algorithmItem.dataCoveringWindows)
        for(Pattern *p: parameters.algorithmItem.CTPatterns){
            list<Window*>::iterator itBegin = p->minWindows.begin();
            list<Window*>::iterator itEnd = p->minWindows.end();
            while(itBegin != itEnd){
                if(((*itBegin)->first->eventId >= sw.events.at(0).eventId) && ((*itBegin)->last->eventId <= sw.events.at(sw.events.size()-1).eventId) ){
                    p->occurrencesInWindows.push_back(sw);
                    break;
                }
                itBegin++;
            }
        }
}

// get the occurrences (sliding windows) for each of the ST patterns by finding events not covered with CT patterns
void CooccurranveAnomaliesDetector::findOccurrencesForSTPatterns(Parameters &parameters){
    
    vector<int> eventsNotInCT = findEventsNotCoveredWithCT(parameters);
    
    for(SlidingWindow sw:parameters.algorithmItem.dataCoveringWindows)
        for (int e: eventsNotInCT)
            if((e >= sw.events.at(0).eventId)&&(e <= sw.events.at(sw.events.size()-1).eventId)){
                int symbol = sw.events.at(e - sw.events.at(0).eventId).eventContent;
                for(int i = 0;i < parameters.algorithmItem.STPatterns.size();i++)
                    if(stoi(parameters.algorithmItem.STPatterns.at(i)->patternSymbols) == symbol){
                        parameters.algorithmItem.STPatterns.at(i)->occurrencesInWindows.push_back(sw);
                    }         
            }
}

vector<int> CooccurranveAnomaliesDetector::findEventsNotCoveredWithCT(Parameters parameters){
    
    vector<int> eventsNotInCT;
    vector<int> eventsInCT(parameters.algorithmItem.eventsNumber, 0);  // initialize by 0 for all events and then if an event is covered with CT patterns then it will be set to 1
    for(Pattern *p: parameters.algorithmItem.CTPatterns){
        list<Window*>::iterator itBegin = p->minWindows.begin();
        list<Window*>::iterator itEnd = p->minWindows.end();
        while(itBegin != itEnd){
            int start = (*itBegin)->first->eventId;
            int end = (*itBegin)->last->eventId;
            int relatedEventsCount = end - start;
            if (relatedEventsCount+1 == p->patternSize)  // only consider valid windows that are used in the cover!
                for(int i=start;i<=start+relatedEventsCount;i++)
                    eventsInCT.at(i) = 1;
            itBegin++;
        }
    }
    
    for(int i = 0;i<eventsInCT.size();i++)
        if (eventsInCT.at(i) == 0)
            eventsNotInCT.push_back(i);
    
    return eventsNotInCT;
    
}

// get the common occurrences of sliding windows between two patterns
vector<int> CooccurranveAnomaliesDetector::getInteractionsIds(vector<SlidingWindow> firstPatternWindows, vector<SlidingWindow> secondPatternWindows){
    
    vector<int> vector1, vector2;
    vector<int> result(vector1.size() + vector2.size());
    for (SlidingWindow w : firstPatternWindows)
        vector1.push_back(w.windowId);
    for (SlidingWindow w : secondPatternWindows)
        vector2.push_back(w.windowId);
    
//    for(int i: vector1)
//        cout<<i<<", ";
//    cout<<'\n';
//    
//    for(int i: vector2)
//        cout<<i<<", ";
//    cout<<'\n';
    
    sort(vector1.begin(), vector1.end()); 
    sort(vector2.begin(), vector2.end()); 

    
    for (vector<int>::iterator i = vector1.begin(); i != vector1.end(); ++i)
        if (std::find(vector2.begin(), vector2.end(), *i) != vector2.end())
            result.push_back(*i);
    
//    for(int i: result)
//        cout<<i<<", ";
//    cout<<'\n';
//    cout<<"--------------------------------------------------\n";
//    cout<<'\n';

    return result;
    

 
}
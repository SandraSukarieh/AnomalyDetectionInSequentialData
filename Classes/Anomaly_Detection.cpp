#include "Anomaly_Detection.h"

using namespace std;

void Anomaly_Detection::computeAnomalyScores(Parameters parameters, string fileName, int windowLength, int totalWindows, int *totalWindowsPerSequence){

    computeCTProbabilities(parameters,totalWindows , windowLength , totalWindowsPerSequence);

    computeDoubleCTProbabilities(parameters,totalWindows , windowLength , totalWindowsPerSequence);

    computeSTProbabilities(parameters, totalWindows, windowLength, totalWindowsPerSequence);

    list<Pattern*> *ranking = new list<Pattern*>;
    
    for (int i=0;i<parameters.algorithmItem.CTxCT.size();i++){
        double nrAlignments = countAlignments(parameters.algorithmItem.CTxCT[i]->firstPattern, parameters.algorithmItem.CTxCT[i]->secondPattern);
        parameters.algorithmItem.CTxCT[i]->calculateFirstandSecondProbabilities(parameters.algorithmItem.CTPatterns, parameters.algorithmItem.STPatterns);
        if(!isinf(-lg2(parameters.algorithmItem.CTxCT[i]->patternProbability)) and (!isinf(lg2((parameters.algorithmItem.CTxCT[i]->firstPattern->patternProbability * parameters.algorithmItem.CTxCT[i]->secondPattern->patternProbability )/(double)nrAlignments ))))
            parameters.algorithmItem.CTxCT[i]->patternAnomalyScore =  -lg2(parameters.algorithmItem.CTxCT[i]->patternProbability) + lg2((parameters.algorithmItem.CTxCT[i]->firstPattern->patternProbability * parameters.algorithmItem.CTxCT[i]->secondPattern->patternProbability )/(double)nrAlignments );
        cout<<parameters.algorithmItem.CTxCT[i]->patternSymbols<<" , "<<  parameters.algorithmItem.CTxCT[i]->firstPattern->patternSupport<<", "<<parameters.algorithmItem.CTxCT[i]->firstPattern->patternProbability<<" , "<<  parameters.algorithmItem.CTxCT[i]->secondPattern->patternSupport<<", "<<parameters.algorithmItem.CTxCT[i]->secondPattern->patternProbability<<"\n";
        if(parameters.algorithmItem.CTxCT[i]->patternAnomalyScore > 0)	//only surprising combi's
            if (parameters.algorithmItem.CTxCT[i]->patternProbability != -1){ //not skipped
                insertOrdered(ranking, parameters.algorithmItem.CTxCT[i]);
            }
    }

    cout<<"------------------------------------------\n";
    if(parameters.approach == SUPPORT)
        cout<<"Anomaly result - Support Approach \n";
    else if(parameters.approach == WINDOW)
        cout<<"Anomaly result - Window Approach \n";
    else
        cout<<"Anomaly result - CodeTable Approach \n";
    if (ranking->size()==0)
        cout<<"no anomalies were detected \n";
    else{
        removeRedundancy(ranking);
        writeAnomaliesToTextFile(ranking, fileName);
    }
}

void Anomaly_Detection::computeCTProbabilities (Parameters parameters, int totalWindows , int windowLength , int *totalWindowsPerSequence){

    for(Pattern *p : parameters.algorithmItem.CTPatterns){
        if (parameters.approach== SUPPORT){
            p->patternProbability = (p->patternSupport*p->patternLength)/(double)parameters.algorithmItem.eventsNumber;
        }
        else if (parameters.approach ==WINDOW){
            double numberOfWindows = computeNrWindows(parameters, p, windowLength, totalWindowsPerSequence);
            p->patternProbability = numberOfWindows/(double)totalWindows;
        }

        else if(parameters.approach == CODETABLE){
            double sum = 0;
            for(Pattern *p2 : parameters.algorithmItem.CTPatterns)
                if(p->checkSubset(p2))
                    sum += pow(2, -p2->patternCodeLength);
            p->patternProbability = sum;
        }
    }
}

void Anomaly_Detection::computeSTProbabilities (Parameters &parameters, int totalWindows , int windowLength , int *totalWindowsPerSequence){

    for(int i=0;i<parameters.algorithmItem.STPatterns.size();i++){
        if (parameters.approach== SUPPORT){
            parameters.algorithmItem.STPatterns.at(i)->patternProbability = (parameters.algorithmItem.STPatterns.at(i)->patternSupport)/(double)parameters.algorithmItem.eventsNumber;
        }
        else if (parameters.approach ==WINDOW){
            double numberOfWindows = computeNrWindowsForST(parameters, parameters.algorithmItem.STPatterns.at(i), windowLength, totalWindowsPerSequence);
            parameters.algorithmItem.STPatterns.at(i)->patternProbability = numberOfWindows/(double)totalWindows;
        }

        else if(parameters.approach == CODETABLE){
            double sum = 0;
            for(Pattern *p2 : parameters.algorithmItem.CTPatterns)
                for (int e : p2->parsedEvents)
                    if(stoi(parameters.algorithmItem.STPatterns.at(i)->patternSymbols) == e){
                        sum += pow(2, -p2->patternCodeLength);
                        break;
                    }
            parameters.algorithmItem.STPatterns.at(i)->patternProbability = sum;
        }
    }

}

int Anomaly_Detection::computeNrWindows(Parameters parameters , Pattern *p, int windowLength, int *totalWindowsPerSequence){

    int nrWin = 0;
    if(p->minWindows.size()==1)
        nrWin=1;
    else{
        list <Window*> windows= p->minWindows;
        list<Window*>::iterator it = windows.begin();
        list<Window*>::iterator itEnd = windows.end();
        Window *cur_win = (*it);
        for(int seqID = 0; seqID < parameters.algorithmItem.sequencesNumber; seqID++){
            for(int i = parameters.algorithmItem.sequences[seqID]->start; i < parameters.algorithmItem.sequences[seqID]->start+totalWindowsPerSequence[seqID]; i++){
                while(cur_win->first->eventId < i){
                    ++it;
                    if(it == itEnd){
                        break;
                    }
                    cur_win = *it;
                }
                if(it == itEnd){
                    break;
                }
                if(cur_win->first->eventId >= i && cur_win->last->eventId < i + windowLength){
                    nrWin++;
                }
            }
        }
    }
    return nrWin;
}

int Anomaly_Detection::computeNrWindowsForST (Parameters parameters , Pattern *p, int windowLength, int *totalWindowsPerSequence){

    int nrWin = 0;
    for(int seqID = 0; seqID < parameters.algorithmItem.sequencesNumber; seqID++){
        for(int i = parameters.algorithmItem.sequences[seqID]->start; i < parameters.algorithmItem.sequences[seqID]->start+totalWindowsPerSequence[seqID]; i++){
            for (int j = 0;j<p->occurances.size();j++){
                    int cur_win = p->occurances.at(j);
                    if(cur_win >= i && cur_win < i + windowLength){
                        nrWin++;
                        break;
                    }
            }
        }
    }
    return nrWin;
}

int Anomaly_Detection::countAlignments(Pattern *x, Pattern *y){

    int cnt = 0;
    //try all different offsets
    for(int offset = 0; offset < x->patternLength + y->patternLength + 1; ++offset)
        if(!x->checkPatternAttributeOverlap(y, offset))
            ++cnt;
    return cnt;
}

//Highest score at top (prefer larger patterns)
void Anomaly_Detection::insertOrdered(list<Pattern*>* lst, Pattern *cand){

    bool inserted = false;
	list<Pattern*>::iterator it = lst->begin(), end = lst->end();
	while(it != end)
	{
		if( cand->patternAnomalyScore > (*it)->patternAnomalyScore || (cand->patternAnomalyScore == (*it)->patternAnomalyScore && cand->patternSize >= (*it)->patternSize) )
		{
			lst->insert(it, cand);
			inserted = true;
			break;
		}
		++it;
	}
	if(!inserted)
		lst->push_back(cand);
}

void Anomaly_Detection::removeRedundancy(list<Pattern*>* ranking){
    
    list<Pattern*>::iterator beginIter = ranking->begin(), endIter = ranking->end();
    int currentPatternPosition = 0;
    while(beginIter != endIter){
        int result = -1;
        while(true){
            result = findRedundantPattern(ranking, currentPatternPosition);
            if(result != -1){
                list<Pattern*>::iterator eraseIterator = ranking->begin();
                for (int j=0;j< result;j++)
                    eraseIterator++;
                ranking->erase(eraseIterator);
            }
            else
                break;
        }
        beginIter++;
        currentPatternPosition ++;
    }
}


int Anomaly_Detection::findRedundantPattern(list<Pattern*>* ranking, int currentPatternPosition) {
    
    list<Pattern*>::iterator beginIter = ranking->begin(), endIter = ranking->end(), iter = ranking->begin();
    for (int j=0;j< currentPatternPosition;j++){
        beginIter++;
        iter++;
    }
    Pattern *currentPattern = *beginIter;
    iter++; // we want to compare the current pattern with all patterns after it but not with itself!
    int patternToRemove = currentPatternPosition + 1;
    while(iter != endIter){
        if(currentPattern->checkForRedundancy((*iter))){
            return patternToRemove;
        }
        else{
            iter++;
            patternToRemove++;
        }
    }
    return -1;
}


void Anomaly_Detection::writeAnomaliesToTextFile(list<Pattern*> *ranking, string fileName){

    fstream fs;
    fs.open (fileName, fstream::in | fstream::out | fstream::trunc);
    list<Pattern*>::iterator it2 = ranking->begin(), endit2 = ranking->end();
    while(it2 != endit2){
        Pattern *p = *it2;
        fs << p->patternSymbols<<", score = "<<p->patternAnomalyScore<<",";
        list<Window*>::iterator iter = p->minWindows.begin(), endIter = p->minWindows.end();
        fs<<" min windows: {";
        while(iter != endIter){
            if((*iter)->last->eventId - (*iter)->first->eventId + 1== p->patternSize)
                fs<<"("<<(*iter)->first->eventId<<","<<(*iter)->last->eventId<<")";
            iter++;
        }
    fs<<"}\n";
    it2++;
    }
    fs.close();
}


vector<coveringWindowEvent> Anomaly_Detection::findCommonSuspiciousEvents(vector<SlidingWindow> sortedCoveringWindows){
    
    double maxWindowCodeLength = DBL_MIN;
    double minWindowCodeLength = DBL_MAX;
    
    for(int i=0;i<sortedCoveringWindows.size();i++){
        if (sortedCoveringWindows.at(i).windowCodeLength < minWindowCodeLength)
            minWindowCodeLength = sortedCoveringWindows.at(i).windowCodeLength;
        if (sortedCoveringWindows.at(i).windowCodeLength > maxWindowCodeLength)
            maxWindowCodeLength = sortedCoveringWindows.at(i).windowCodeLength;
    }
    
    bool repeat = true;
    double mainChoice = 0;
    double threshold = 0;
    cout<<"Enter a window's code length threshold between "<<minWindowCodeLength<<" and "<<maxWindowCodeLength<<"\n";
    while (repeat){
        cin>>mainChoice;
        if ((mainChoice <= maxWindowCodeLength) and (mainChoice >= minWindowCodeLength)){
            threshold = mainChoice;
            repeat = false;
        }
        else
            cout<<"incorrect input, please repeat \n";
    }
    
    vector<SlidingWindow> analyzedWindows;
    for(int i=0;i<sortedCoveringWindows.size();i++)
        if (sortedCoveringWindows.at(i).windowCodeLength >= threshold)
            analyzedWindows.push_back(sortedCoveringWindows.at(i));
    
    vector<vector<SlidingWindow>> overlappedWindowsSet;
    findOverlappedWindows(analyzedWindows, overlappedWindowsSet);
    
    vector<coveringWindowEvent> commonSuspiciousEvents;
    set<int> alreadyAddedEvents;
    
    struct myclass {
        bool operator() (SlidingWindow w1, SlidingWindow w2) { return (w1.windowId < w2.windowId);}
    } myobject;
    
    for(int j = 0;j< overlappedWindowsSet.size();j++) { 
        sort(overlappedWindowsSet.at(j).begin(), overlappedWindowsSet.at(j).end(), myobject);
        int temp = abs(overlappedWindowsSet.at(j).at(overlappedWindowsSet.at(j).size()-1).windowId - overlappedWindowsSet.at(j).at(0).windowId);
        int windowLength = overlappedWindowsSet.at(j).at(0).windowContent.size();
        int commonEventsCount = abs(windowLength - temp);
        for(int i = windowLength - commonEventsCount ; i<windowLength ; i++)
            if(alreadyAddedEvents.find(overlappedWindowsSet.at(j).at(0).events.at(i).eventId) == alreadyAddedEvents.end()){
                alreadyAddedEvents.insert(overlappedWindowsSet.at(j).at(0).events.at(i).eventId);
                commonSuspiciousEvents.push_back(overlappedWindowsSet.at(j).at(0).events.at(i));
            }
    }
    
    return commonSuspiciousEvents;
}

void Anomaly_Detection::findOverlappedWindows(vector<SlidingWindow> analyzedWindows, vector<vector<SlidingWindow>> &overlappedWindowsSet){
    
    for(int i=0;i<analyzedWindows.size();i++){
        vector<SlidingWindow> currectVector;
        currectVector.push_back(analyzedWindows.at(i));
        for(int j=i+1;j<analyzedWindows.size();j++)
            if(isOverlapped(analyzedWindows.at(i), analyzedWindows.at(j)))
                currectVector.push_back(analyzedWindows.at(j));
        if(currectVector.size()>1)
            overlappedWindowsSet.push_back(currectVector);
    }        
}

bool Anomaly_Detection::isOverlapped(SlidingWindow w1, SlidingWindow w2){
    
    if (w1.sequenceId != w2.sequenceId)
        return false;
    else{
        int windowLength = w1.windowContent.size();
        if (abs(w1.windowId-w2.windowId) <= windowLength-1)
            return true;
        else
            return false;
    }
}

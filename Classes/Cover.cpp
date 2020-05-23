/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Cover.h"

//order the CT patterns based on which should be used first for covering (using the simple bubble sort)
void Cover::orderPatternsforCovering(vector<Pattern*> CTPatterns){

   orderedPatternsforCovering = CTPatterns;
   bool swapped;
   for (int i = 0; i < orderedPatternsforCovering.size()-1; i++)
   {
     swapped = false;
     for (int j = 0; j <orderedPatternsforCovering.size()-i-1; j++)
     {
        if (orderedPatternsforCovering.at(i+1)->smallerThan(orderedPatternsforCovering.at(i)))
        {
            Pattern temp = *(orderedPatternsforCovering.at(i));
            *(orderedPatternsforCovering.at(i)) = *(orderedPatternsforCovering.at(i+1));
            *(orderedPatternsforCovering.at(i+1))= temp;
            swapped = true;
        }
     }

     // IF no two elements were swapped by inner loop, then break
     if (swapped == false)
        break;
   }
}

void Cover::coverSlidingWindows(vector<Sliding_Window> &dataSlidingWindows, vector<Pattern*> orderedPatternsforCovering, vector<STItem> STPatterns, int answer){

    //use patterns one by one to cover all possibilities within the set of sliding windows
    for (int i=0;i<orderedPatternsforCovering.size();i++){
            Pattern *p = (orderedPatternsforCovering.at(i));
            for (int j=0;j<dataSlidingWindows.size();j++){
                vector<int> positions;
                findAllOccurrences(dataSlidingWindows.at(j), orderedPatternsforCovering.at(i)->patternSymbols, positions);
                if (positions.size() > 0)
                    coverOneWindowbyOneCTPattern(dataSlidingWindows.at(j), orderedPatternsforCovering.at(i), positions);
            }
    }
    if (answer == 1)
        coverRemainingSingletons(dataSlidingWindows, STPatterns);
    else if (answer == 2)
        coverRemainingSingletonsWithFuture(dataSlidingWindows, STPatterns);
    else
        coverRemainingSingletonsWithHistoryandFuture(dataSlidingWindows, STPatterns);
}

void Cover::findAllOccurrences(Sliding_Window window, string pattern, vector<int> &positions){

    vector<int> stringPositions;
    // Get the first occurrence
    int pos = window.windowContent.find(pattern);

    // Repeat till end is reached
    while( pos != std::string::npos)
    {
            // Add position to the vector
            stringPositions.push_back(pos);

            // Get the next occurrence from the current position
            pos =window.windowContent.find(pattern, pos + pattern.size());
    }
    // link each found position in the string to its event to return the position of the first event
    for(int i=0;i<stringPositions.size();i++){
        for(int j=0;j<window.events.size();j++){
            string x = to_string(window.events.at(j).eventContent);
            int eventEndPosiotion = x.length()-1;
            if(stringPositions.at(i) < eventEndPosiotion){
                positions.push_back(j);
                break;
            }
        }
    }
}


//cover one window with one pattern from CT based on the locations of the pattern's occurrences in the window
void Cover::coverOneWindowbyOneCTPattern(Sliding_Window &window,  Pattern *pattern, vector<int> positions){

    for (int pos : positions){
        if (window.events.at(pos).covered == true)
            continue;
        for (int j=0;j<pattern->patternSize;j++){
            window.events.at(pos+j).covered = true;
        }
        window.windowCodeLength += pattern->patternCodeLength;
    }
}


//3 different methods to cover the uncovered singleton events within the window with ST patterns as CT patterns were not able to cover them

void Cover::coverRemainingSingletons(vector<Sliding_Window> &dataSlidingWindows, vector<STItem> STPatterns){

    for (int i=0;i<dataSlidingWindows.size();i++){
        for (int j=0;j<dataSlidingWindows.at(i).events.size();j++){
            if (dataSlidingWindows.at(i).events.at(j).covered == false){
                int k=0;
                while (k != STPatterns.size()){
                    if (dataSlidingWindows.at(i).events.at(j).eventContent == STPatterns.at(k).STPattern)
                        break;
                    else
                        k++;
                }
                dataSlidingWindows.at(i).windowCodeLength += STPatterns.at(k).STPatternCTLength;
                dataSlidingWindows.at(i).events.at(j).covered = true;
            }
        }
    }
}

void Cover::coverRemainingSingletonsWithFuture(vector<Sliding_Window> &dataSlidingWindows, vector<STItem> STPatterns){

    for (int i=0;i<dataSlidingWindows.size();i++){
          for (int j=0;j<dataSlidingWindows.at(i).events.size();j++){
            if (dataSlidingWindows.at(i).events.at(j).covered == false){
                if ((j == dataSlidingWindows.at(i).events.size()-1) && (i!=dataSlidingWindows.size()-1)) //for the very last window, we should not check the possibilities of the last singleton because nothing after it
                    coverLastSigleton(dataSlidingWindows, i, j, STPatterns);
                else{
                    int k=0;
                    while (k != STPatterns.size()){
                        if (dataSlidingWindows.at(i).events.at(j).eventContent == STPatterns.at(k).STPattern)
                            break;
                        else
                            k++;
                    }
                    dataSlidingWindows.at(i).windowCodeLength += STPatterns.at(k).STPatternCTLength;
                }
                dataSlidingWindows.at(i).events.at(j).covered = true;
            }
        }
    }
}

void Cover::coverRemainingSingletonsWithHistoryandFuture(vector<Sliding_Window> &dataSlidingWindows, vector<STItem> STPatterns){

    for (int i=0;i<dataSlidingWindows.size();i++){
        for (int j=0;j<dataSlidingWindows.at(i).events.size();j++){
            if (dataSlidingWindows.at(i).events.at(j).covered == false){
                if ((j == 0) && (i !=0)) //for the very first window, we should not check the possibilities of the first singleton because nothing before it
                    coverFirstSigleton(dataSlidingWindows,i,j,STPatterns);
                else if ((j == dataSlidingWindows.at(i).events.size()-1) && (i!=dataSlidingWindows.size()-1)) //for the very last window, we should not check the possibilities of the last singleton because nothing after it
                    coverLastSigleton(dataSlidingWindows, i, j, STPatterns);
                else{
                    int k=0;
                    while (k != STPatterns.size()){
                        if (dataSlidingWindows.at(i).events.at(j).eventContent == STPatterns.at(k).STPattern)
                            break;
                        else
                            k++;
                    }
                    dataSlidingWindows.at(i).windowCodeLength += STPatterns.at(k).STPatternCTLength;
                }
                dataSlidingWindows.at(i).events.at(j).covered = true;
            }
        }
    }
}


//cover the first singleton with taking into account the previous events
void Cover::coverFirstSigleton(vector<Sliding_Window> &dataSlidingWindows, int i, int j, vector<STItem> STPatterns){
    vector<Pattern*> possibilities; //only contains CT patterns and not singletons
    int firstSingleton = dataSlidingWindows.at(i).events.at(j).eventContent;
    for (int k = 0;k < orderedPatternsforCovering.size();k++){
        if (orderedPatternsforCovering.at(k)->patternSymbols.find(to_string(firstSingleton)) != std::string::npos)
            possibilities.push_back(orderedPatternsforCovering.at(k));
    }
    int m = 0;
    while (m != STPatterns.size()){
        if(firstSingleton == STPatterns.at(m).STPattern)
            break;
        else
            m++;
    }
    double calculatedCodeLength = 0;
    double weight = 0;
    if (possibilities.size()==0){
        calculatedCodeLength = 1* STPatterns.at(m).STPatternCTLength; // weight = 1
    }
    else{
        for (int k = 0;k < possibilities.size();k++){
            weight =  calculatPossibilityWeight(possibilities, k, STPatterns.at(m).STPatternCTLength, false);
            calculatedCodeLength += weight* possibilities.at(k)->patternCodeLength * ( STPatterns.at(m).STPatternCodeLength/(double)possibilities.at(k)->patternSTSize);
        }
        weight = calculatPossibilityWeight(possibilities, -1, STPatterns.at(m).STPatternCTLength, true);
        calculatedCodeLength += weight* STPatterns.at(m).STPatternCTLength;
    }
    dataSlidingWindows.at(i).windowCodeLength += calculatedCodeLength;
}

//cover the last singleton with taking into account the future events
void Cover::coverLastSigleton(vector<Sliding_Window> &dataSlidingWindows, int i, int j, vector<STItem> STPatterns){
    vector<Pattern*> possibilities; //only contains CT patterns and not singletons
    int lastSingleton = dataSlidingWindows.at(i).events.at(j).eventContent;
    for (int k = 0;k < orderedPatternsforCovering.size();k++){
        if (orderedPatternsforCovering.at(k)->patternSymbols.find(to_string(lastSingleton)) != std::string::npos)
            possibilities.push_back(orderedPatternsforCovering.at(k));
    }
    int m = 0;
    while (m != STPatterns.size()){
        if(lastSingleton == STPatterns.at(m).STPattern)
            break;
        else
            m++;
    }
    double calculatedCodeLength = 0;
    double weight = 0;
    if (possibilities.size()==0){
        calculatedCodeLength = 1* STPatterns.at(m).STPatternCTLength; // weight = 1
    }
    else{
        for (int k = 0;k < possibilities.size();k++){
            weight =  calculatPossibilityWeight(possibilities, k, STPatterns.at(m).STPatternCTLength, false);
            calculatedCodeLength += weight* possibilities.at(k)->patternCodeLength * ( STPatterns.at(m).STPatternCodeLength/(double)possibilities.at(k)->patternSTSize);
        }
        weight = calculatPossibilityWeight(possibilities, -1, STPatterns.at(m).STPatternCTLength, true);
        calculatedCodeLength += weight* STPatterns.at(m).STPatternCTLength;
    }
    dataSlidingWindows.at(i).windowCodeLength += calculatedCodeLength;
}


double Cover::calculatPossibilityWeight(vector<Pattern*> possibilities, int k, double STPatternCTLength, bool isSingleton){

    double dom=0;
    for (int i=0;i<possibilities.size();i++){
        dom += pow(2,-possibilities.at(i)->patternCodeLength);
    }
    dom += pow(2,-STPatternCTLength); // add the amount related to the singleton itself
    if (! isSingleton)
        return (pow(2,-possibilities.at(k)->patternCodeLength)/(double)dom);
    else
        return (pow(2,-STPatternCTLength)/(double)dom);

}

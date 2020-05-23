/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include"Pattern.h"

using namespace std;

Pattern::Pattern(int patternSize, int patternSupport, string symbols): patternSize(patternSize), patternSupport(patternSupport){
    patternSymbols.assign(symbols);
    patternLength = patternSize;
    patternProbability = -1;
    firstPattern = NULL;
    secondPattern = NULL;
    patternSTSize = 0;
    patternNewUsage = 0;
}

Pattern::Pattern(int patternSize, int patternSupport, string symbols, int patternUsage, list <Window*> windows): patternSize(patternSize), patternSupport(patternSupport),patternUsage(patternUsage){
    patternSymbols.assign(symbols);
    patternLength = patternSize;
    patternProbability = -1;
    this->minWindows = windows;
    firstPattern = NULL;
    secondPattern = NULL;
    patternSTSize = 0;
    patternNewUsage = 0;
}

Pattern::Pattern(int patternSize, int patternSupport,string symbols, int patternUsage, list <Window*> windows, Pattern* firstP, Pattern* secondP): patternSize(patternSize), patternSupport(patternSupport),patternUsage(patternUsage){
    patternSymbols.assign(symbols);
    patternLength = patternSize;
    patternProbability = -1;
    this->minWindows = windows;
    firstPattern = firstP;
    secondPattern = secondP;
    patternSTSize = 0;
    patternNewUsage = 0;
}

void Pattern::printPattern(){

     cout<<"pattern "<<patternSymbols<<", usage "<<patternUsage<<", support "<<patternSupport<<", size "<<patternSize<<" and new usage "<<patternNewUsage<<"\n";

    //----Printing minimal windows for the pattern
    //this.printPatternMinWindows();

}

void Pattern::printPatternMinWindows(){

    for (auto w: minWindows)
    cout<<"first event("<<w->first->eventId<<","<<w->first->eventIndex<<","<<w->first->eventSeqid<<","<<w->first->eventSimid<<","<<w->first->eventSymbol<<") , last event("<<w->last->eventId<<","<<w->last->eventIndex<<","<<w->last->eventSeqid<<","<<w->last->eventSimid<<","<<w->last->eventSymbol<<")\n";

}

//returns true if this is a subset of otherPattern
bool Pattern::checkSubset(Pattern *otherPattern){

    if (otherPattern->patternSymbols.find(patternSymbols) != std::string::npos)
        return true;
    else
        return false;
}

bool Pattern::isUsageLess(Pattern *otherPattern){
    return (patternUsage<otherPattern->patternUsage)||((patternUsage==otherPattern->patternUsage)&&(patternSize<otherPattern->patternSize));
}

//RETURN true when Patterns a and b overlap when combined with the given offset
bool Pattern::checkPatternAttributeOverlap(Pattern *otherPattern, int offset){

    if(offset == 0 || offset == patternLength + otherPattern->patternLength){   //one pattern completely before the other
        //cout<<" checkPatternAttributeOverlap method, one pattern completely before the other \n";
        return false;
    }


    int startPosA = max(0, otherPattern->patternLength - offset), startPosB = max(0, offset - otherPattern->patternLength);
	int newLength;

	if(startPosB == 0)
		newLength = max(startPosA + patternLength, otherPattern->patternLength);
    else
		newLength = max(startPosB + otherPattern->patternLength, patternLength);

    for(int pos = 0; pos < newLength; ++pos)
	{
		if(pos >= startPosA && pos >= startPosB && pos-startPosA < patternLength && pos-startPosB < otherPattern->patternLength)
			//if(checkEventsetAttributeOverlap(a->getSymbols(pos-startPosA),  b->getSymbols(pos-startPosB))) RETURN true when eventsets a and b contain a similar attribute... we don't need that in the univariate case.
				return true;
	}
	return false;

}

void Pattern::calculateFirstandSecondProbabilities(vector<Pattern*> CTPatterns, vector<STItem> STPatterns){

    if (firstPattern !=NULL){
        if(firstPattern->patternSize > 1){
            for (int i=0;i<CTPatterns.size();i++){
                if (strcmp(firstPattern->patternSymbols.c_str(), CTPatterns[i]->patternSymbols.c_str())==0){
                    firstPattern->patternProbability = CTPatterns[i]->patternProbability;
                    break;
                }
            }
        }
        else{
          for (int i=0;i<STPatterns.size();i++){
            if (strcmp(firstPattern->patternSymbols.c_str(), to_string(STPatterns[i].STPattern).c_str())==0){
                firstPattern->patternProbability = STPatterns[i].STProbability;
                break;
            }
          }

        }
    }
    if (secondPattern !=NULL){
            if(secondPattern->patternSize > 1){
                for (int i=0;i<CTPatterns.size();i++){
                    if (strcmp(secondPattern->patternSymbols.c_str(), CTPatterns[i]->patternSymbols.c_str())==0){
                        secondPattern->patternProbability = CTPatterns[i]->patternProbability;
                        break;
                    }
                }
            }
            else{
                for (int i=0;i<STPatterns.size();i++){
                    if (strcmp(secondPattern->patternSymbols.c_str(), to_string(STPatterns[i].STPattern).c_str())==0){
                        secondPattern->patternProbability = STPatterns[i].STProbability;
                        break;
                    }
                }

            }

    }

}

//this function is used for the covering order, and smaller means here "should come first", so if x is smaller than y then it should be selected first for the covering
bool Pattern::smallerThan(Pattern *otherPattern){

    //descending on cardinality ||X|| -- the return result is swapped cause it's a descending order
    if (patternSize < otherPattern->patternSize)
        return false;
    if (patternSize > otherPattern->patternSize)
        return true;

    //descending on support
    if (patternSupport < otherPattern->patternSupport)
        return false;
    if (patternSupport > otherPattern->patternSupport)
        return true;

    //descending on L(X|ST)
    if (patternSTSize < otherPattern->patternSTSize)
        return false;
    if (patternSTSize > otherPattern->patternSTSize)
        return true;

    //ascending lexicographically
    auto ita = patternSymbols.begin(), enda = patternSymbols.end(), itb = otherPattern->patternSymbols.begin(), endb = otherPattern->patternSymbols.end();
    while(ita != enda && itb != endb){
        if ((*ita) < (*itb))
            return true;
        if ((*itb) < (*ita))
            return false;
        ++ita;
        ++itb;
    }
    return false;
}

bool Pattern::biggerThan(Pattern *otherPattern){
    return otherPattern->smallerThan(this);
}

bool Pattern::smallerOrEqual(Pattern *otherPattern){
    return !(biggerThan(otherPattern));
}

bool Pattern::biggerOrEqual(Pattern *otherPattern){
    return !(smallerThan(otherPattern));
}

bool Pattern::equals(Pattern *otherPattern){
    return (!(smallerThan(otherPattern)) && !(otherPattern->smallerThan(this)));
}

bool Pattern::notEqual(Pattern *otherPattern){
    return !(equals(otherPattern));
}

void Pattern::printPatternEvents(){

    list<Window*>::iterator iter = this->minWindows.begin(), endIter = this->minWindows.end();
    cout<<" min windows: {";
    while(iter != endIter){
        if((*iter)->last->eventId - (*iter)->first->eventId + 1== this->patternSize)
            cout<<"("<<(*iter)->first->eventId<<","<<(*iter)->last->eventId<<")";
        iter++;
    }
    cout<<"}\n";
}

bool Pattern::checkForRedundancy(Pattern* otherPattern){
    if(otherPattern->checkSubset(this)){
        if(this->minWindows.size()==1){
            if(otherPattern->minWindows.size()==1){
                list<Window*>::iterator iter1 = this->minWindows.begin(), iter2 = otherPattern->minWindows.begin();
                return checkSubsetinPosition((*iter1), (*iter2));
            }
            else{
                list<Window*>::iterator iter1 = this->minWindows.begin(), iter2 = otherPattern->minWindows.begin(), iter2end = otherPattern->minWindows.end();
                while(iter2 != iter2end){
                    if(checkSubsetinPosition((*iter1), (*iter2)))
                        return true;
                    iter2++;
                }
                return false;
            }
        }
        else{
            list<Window*>::iterator iter1 = this->minWindows.begin(), iter1end = this->minWindows.end();
            if(otherPattern->minWindows.size()==1){
                list<Window*>::iterator iter2 = otherPattern->minWindows.begin();
                while(iter1 != iter1end){
                    if(checkSubsetinPosition((*iter1), (*iter2)))
                        return true;
                    iter1++;
                }
                return false;
            }
            else{
                list<Window*>::iterator iter2end = otherPattern->minWindows.end();
                while(iter1 != iter1end){
                    list<Window*>::iterator iter2 = otherPattern->minWindows.begin();
                    while(iter2 != iter2end){
                        if(checkSubsetinPosition((*iter1), (*iter2)))
                            return true;
                        iter2++;
                    }
                    iter1++;
                }
                return false;
            }
        }
    }
    return false;
}

//return true if window2 is contained in/equal to window1
bool Pattern::checkSubsetinPosition(Window *window1, Window *window2){
    if((window2->first->eventId >= window1->first->eventId) && (window2->last->eventId <= window1->last->eventId))
        return true;
    else
        return false;
}

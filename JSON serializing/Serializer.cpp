#include <iostream>
#include <fstream>
#include <string>
#include "Serializer.h"

using namespace std;
using namespace Json;


void Serializer::serialize(vector<pattern *> serializedPatternsVector, Value &jsonValue){

    for (int i = 0; i < serializedPatternsVector.size(); i++){
            string symbols="";
            for (int k=0;k<serializedPatternsVector[i]->size();k++){
                symbols.append(to_string(serializedPatternsVector[i]->symbols[k]));
            }
            Value patternValue;
            patternValue["symbols"]= symbols;
            patternValue["size"] = serializedPatternsVector[i]->size();
            patternValue["usage"] = serializedPatternsVector[i]->cur.usage;
            patternValue["support"] = serializedPatternsVector[i]->win.usage;

            //for combined patterns
            if (serializedPatternsVector[i]->firstPattern!=NULL){
                string symbols2="";
                for (int k=0;k<serializedPatternsVector[i]->firstPattern->size();k++){
                    symbols2.append(to_string(serializedPatternsVector[i]->firstPattern->symbols[k]));
                }
                patternValue["firstPattern"]["firstPatternSymbols"] = symbols2;
                patternValue["firstPattern"]["firstPatternSupport"] = serializedPatternsVector[i]->firstPattern->win.usage;
                patternValue["firstPattern"]["firstPatternSize"] = serializedPatternsVector[i]->firstPattern->size();
            }

            if (serializedPatternsVector[i]->secondPattern!=NULL){
                string symbols2="";
                for (int k=0;k<serializedPatternsVector[i]->secondPattern->size();k++){
                    symbols2.append(to_string(serializedPatternsVector[i]->secondPattern->symbols[k]));
                }
                    patternValue["secondPattern"]["secondPatternSymbols"] = symbols2;
                    patternValue["secondPattern"]["secondPatternSupport"] = serializedPatternsVector[i]->secondPattern->win.usage;
                    patternValue["secondPattern"]["secondPatternSize"] = serializedPatternsVector[i]->secondPattern->size();
            }


            for(int j=0;j<serializedPatternsVector[i]->minimalWindows.size();j++){
                Value windowValue;

                windowValue["firstEvent"]["symbol"]= serializedPatternsVector[i]->minimalWindows[j]->first->symbol;
                windowValue["firstEvent"]["index"]= serializedPatternsVector[i]->minimalWindows[j]->first->index;
                windowValue["firstEvent"]["id"]= serializedPatternsVector[i]->minimalWindows[j]->first->id;
                windowValue["firstEvent"]["simid"]= serializedPatternsVector[i]->minimalWindows[j]->first->simid;
                windowValue["firstEvent"]["seqid"]= serializedPatternsVector[i]->minimalWindows[j]->first->seqid;

                windowValue["lastEvent"]["symbol"]= serializedPatternsVector[i]->minimalWindows[j]->last->symbol;
                windowValue["lastEvent"]["index"]= serializedPatternsVector[i]->minimalWindows[j]->last->index;
                windowValue["lastEvent"]["id"]= serializedPatternsVector[i]->minimalWindows[j]->last->id;
                windowValue["lastEvent"]["simid"]= serializedPatternsVector[i]->minimalWindows[j]->last->simid;
                windowValue["lastEvent"]["seqid"]= serializedPatternsVector[i]->minimalWindows[j]->last->seqid;

                patternValue["minWindows"].append(windowValue);
            }

            jsonValue["codeTable"].append(patternValue);


    }
}




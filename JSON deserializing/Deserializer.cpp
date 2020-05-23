#include "Deserializer.h"

using namespace std;
using namespace Json;

void Deserializer::deserializeCodeTable(ifstream &jsonFile, Algorithm &algorithmItem){

    Value root;
    CharReaderBuilder jsonReader;
    string errs;
    bool parsingSuccesful = parseFromStream(jsonReader, jsonFile, &root, &errs);
    int totalUsageCounter=0;

    if (parsingSuccesful){
        cout<<"successfully parsing CT JOSN file in the deserializer\n";
        Value codeTable = root["codeTable"];
        for (int i=0;i<codeTable.size();i++){
            int pSize = codeTable[i]["size"].asInt();
            int pSupport = codeTable[i]["support"].asInt();
            string pSymbols = codeTable[i]["symbols"].asString();
            int pUsage = codeTable[i]["usage"].asInt();
            totalUsageCounter = totalUsageCounter+pUsage;
            auto autoMinWindows = codeTable[i]["minWindows"];
            list <Window*> pMinWindows;
            for (int j=0;j<autoMinWindows.size();j++){
                int firstId = autoMinWindows[j]["firstEvent"]["id"].asInt();
                int firstIndex = autoMinWindows[j]["firstEvent"]["index"].asInt();
                int firstseqId = autoMinWindows[j]["firstEvent"]["seqid"].asInt();
                int firstsimId = autoMinWindows[j]["firstEvent"]["simid"].asInt();
                int firstSymbol = autoMinWindows[j]["firstEvent"]["symbol"].asInt();

                Event *firstEvent = new Event(firstId,firstIndex,firstseqId,firstsimId,firstSymbol);

                int lastId = autoMinWindows[j]["lastEvent"]["id"].asInt();
                int lastIndex = autoMinWindows[j]["lastEvent"]["index"].asInt();
                int lastseqId = autoMinWindows[j]["lastEvent"]["seqid"].asInt();
                int lastsimId = autoMinWindows[j]["lastEvent"]["simid"].asInt();
                int lastSymbol = autoMinWindows[j]["lastEvent"]["symbol"].asInt();

                Event *lastEvent = new Event(lastId,lastIndex,lastseqId,lastsimId,lastSymbol);

                Window *w = new Window(firstEvent,lastEvent);
                pMinWindows.push_back(w);
            }
            Pattern *p = new Pattern(pSize, pSupport, pSymbols, pUsage, pMinWindows);
            algorithmItem.CTPatterns.push_back(p);
        }
        algorithmItem.totalUsage = totalUsageCounter;
        algorithmItem.calculateCTCodeLength();
    }
    else{
        cout<<"ERROR parsing CT JSON file in the deserializer\n";

    }
}

void Deserializer::deserializeDoubleCT(ifstream &jsonFile, Algorithm &algorithmItem){

    Value root;
    CharReaderBuilder jsonReader;
    string errs;
    bool parsingSuccesful = parseFromStream(jsonReader, jsonFile, &root, &errs);

    if (parsingSuccesful){
        cout<<"successfully parsing CTxCT JSON file in the deserializer\n";
        Value codeTable = root["codeTable"];
        for (int i=0;i<codeTable.size();i++){

            int pSize = codeTable[i]["size"].asInt();
            int pSupport = codeTable[i]["support"].asInt();
            string pSymbols = codeTable[i]["symbols"].asString();
            int pUsage = codeTable[i]["usage"].asInt();
            auto autoMinWindows = codeTable[i]["minWindows"];
            list <Window*> pMinWindows;

            for (int j=0;j<autoMinWindows.size();j++){
                int firstId = autoMinWindows[j]["firstEvent"]["id"].asInt();
                int firstIndex = autoMinWindows[j]["firstEvent"]["index"].asInt();
                int firstseqId = autoMinWindows[j]["firstEvent"]["seqid"].asInt();
                int firstsimId = autoMinWindows[j]["firstEvent"]["simid"].asInt();
                int firstSymbol = autoMinWindows[j]["firstEvent"]["symbol"].asInt();

                Event *firstEvent = new Event(firstId,firstIndex,firstseqId,firstsimId,firstSymbol);

                int lastId = autoMinWindows[j]["lastEvent"]["id"].asInt();
                int lastIndex = autoMinWindows[j]["lastEvent"]["index"].asInt();
                int lastseqId = autoMinWindows[j]["lastEvent"]["seqid"].asInt();
                int lastsimId = autoMinWindows[j]["lastEvent"]["simid"].asInt();
                int lastSymbol = autoMinWindows[j]["lastEvent"]["symbol"].asInt();

                Event *lastEvent = new Event(lastId,lastIndex,lastseqId,lastsimId,lastSymbol);

                Window *w = new Window(firstEvent,lastEvent);
                pMinWindows.push_back(w);
            }

            int pFirstSize = codeTable[i]["firstPattern"]["firstPatternSize"].asInt();
            int pFirstSupport = codeTable[i]["firstPattern"]["firstPatternSupport"].asInt();
            string pFirstSymbols = codeTable[i]["firstPattern"]["firstPatternSymbols"].asString();
            Pattern *first = new Pattern(pFirstSize, pFirstSupport, pFirstSymbols);

            int pSecondSize = codeTable[i]["secondPattern"]["secondPatternSize"].asInt();
            int pSecondSupport = codeTable[i]["secondPattern"]["secondPatternSupport"].asInt();
            string pSecondSymbols = codeTable[i]["secondPattern"]["secondPatternSymbols"].asString();
            Pattern *second = new Pattern(pSecondSize, pSecondSupport, pSecondSymbols);

            Pattern *p = new Pattern(pSize, pSupport, pSymbols, pUsage, pMinWindows, first, second);
            algorithmItem.CTxCT.push_back(p);
        }
    }
    else{
        cout<<"ERROR parsing CTxCT JSON file in the deserializer\n";

    }
}

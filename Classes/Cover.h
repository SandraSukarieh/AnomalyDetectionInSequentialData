/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Cover.h
 * Author: Sandra
 *
 * Created on December 23, 2019, 7:22 PM
 */

#ifndef COVER_H
#define COVER_H

#include <iostream>

#include "Pattern.h"
#include "Window.h"
#include "Algorithm.h"


class Cover{

public:

    void orderPatternsforCovering (vector<Pattern*> CTPatterns);
    void findAllOccurrences (Sliding_Window window, string pattern, vector<int> & positions);
    void coverSlidingWindows (vector<Sliding_Window> &dataSlidingWindows, vector<Pattern*> orderedPatternsforCovering, vector<STItem> STPatterns, int answer);
    void coverOneWindowbyOneCTPattern (Sliding_Window &window, Pattern *pattern, vector<int> positions);
    void coverRemainingSingletons (vector<Sliding_Window> &dataSlidingWindows, vector<STItem> STPatterns);
    void coverRemainingSingletonsWithFuture (vector<Sliding_Window> &dataSlidingWindows, vector<STItem> STPatterns);
    void coverRemainingSingletonsWithHistoryandFuture (vector<Sliding_Window> &dataSlidingWindows, vector<STItem> STPatterns);
    void coverFirstSigleton (vector<Sliding_Window> &dataSlidingWindows, int i, int j, vector<STItem> STPatterns);
    void coverLastSigleton (vector<Sliding_Window> &dataSlidingWindows, int i, int j, vector<STItem> STPatterns);
    double calculatPossibilityWeight (vector<Pattern*> possibilities, int k, double STPatternCTLength, bool isSingleton);

    vector<Pattern*> orderedPatternsforCovering;

};


#endif /* COVER_H */


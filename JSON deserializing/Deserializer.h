/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Deserializer.h
 * Author: Sandra
 *
 * Created on December 23, 2019, 7:59 PM
 */

#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <json/json.h>

#include "Window.h"
#include "Pattern.h"
#include "Event.h"
#include "Algorithm.h"




class Deserializer{

public:

    void deserializeCodeTable(ifstream &jsonFile, Algorithm &algorithmItem);
    void deserializeDoubleCT(ifstream &jsonFile, Algorithm &algorithmItem);
};



#endif /* DESERIALIZER_H */


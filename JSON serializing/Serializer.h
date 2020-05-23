#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED

#include <iostream>
#include <json/json.h>

using namespace std;
using namespace Json;

class Serializer{
public:

        void serialize (vector<pattern *> serializedPatternsVector,Value &jsonValue);
};

#endif // SERIALIZER_H_INCLUDED

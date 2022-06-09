#pragma once

#include <iostream>
#include <string>

#include "transport_catalogue.h"

namespace catalogue::input{
    void FillDatabase(std::istream &in, TransportCatalogue& catalogue);
    
    std::pair<string, vector<std::pair<string, int>>> ParseStop(string_view text, TransportCatalogue& catalogue);
    vector<std::pair<string, int>> ParseDistancesForStop (string_view text);
    
    void ParseBus(string_view text, TransportCatalogue& catalogue);
}

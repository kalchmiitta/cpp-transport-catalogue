#include "input_reader.h"
#include <iomanip>

namespace catalogue::input {
    
    vector<std::pair<string, int>> ParseDistancesForStop (string_view text) {
        vector<std::pair<string, int>> distances_to_stops;
        while (text.find('m') != string::npos) {
            size_t pos_m = text.find('m');
            int distance = std::stoi(string(text.substr(0, pos_m)));
            string_view name;
            size_t pos_end = text.find(',');
            if (pos_end != string::npos) {
                name = text.substr(pos_m + 5, pos_end - (pos_m + 5));
                text.remove_prefix(pos_end + 2);
            } else {
                name = text.substr(pos_m + 5);
                text.remove_prefix(text.size());
            }
            distances_to_stops.push_back({string(name), distance});
        }
        
        return distances_to_stops;
    }


    std::pair<string, vector<std::pair<string, int>>> ParseStop (string_view text,
                                                                 TransportCatalogue &catalogue) {
        Stop new_stop;
        size_t name_end = text.find(':');
        new_stop.name = text.substr(0, name_end);
        size_t lat_end = text.find(',', name_end + 1);
        
        new_stop.stop_coordinate.lat = std::stod(string(text.substr(name_end + 2, text.size() - lat_end - 1)));
        
        if (text.find(',', lat_end + 2) == string::npos) {
            new_stop.stop_coordinate.lng = std::stod(string(text.substr(lat_end + 2)));
            text.remove_prefix(text.size());
        } else {
            size_t lng_end = text.find(',', lat_end + 2);
            new_stop.stop_coordinate.lng = std::stod(string(text.substr(lat_end + 2, text.size() - lng_end - 1)));
            text.remove_prefix(lng_end + 2);
        }
        
        catalogue.AddStop(new_stop);
        
        vector<std::pair<string, int>> distances;
        if (!text.empty()) {
            distances = ParseDistancesForStop(text);
        }
        return {string(new_stop.name), distances};
    }
    
    void ParseBus (string_view text, TransportCatalogue &catalogue) {
        Bus new_bus;
        
        size_t name_end = text.find(':');
        new_bus.name = text.substr(0, name_end);
        
        string_view separator;
        if (text.find(" > ") != string::npos) {
            new_bus.type = RouteType::ring;
            separator = " > ";
        } else {
            new_bus.type = RouteType::forward;
            separator = " - ";
        }
        
        size_t stop_name_start = text.find(':') + 2;
        while (stop_name_start < text.size()) {
            size_t stop_name_end = text.find(separator, stop_name_start);
            if (stop_name_end == string::npos) {
                stop_name_end = text.size();
            }
            new_bus.stops.push_back(text.substr(stop_name_start, stop_name_end - stop_name_start));
            stop_name_start = stop_name_end + 3;
        }
        new_bus.unique_stops = {new_bus.stops.begin(), new_bus.stops.end()};
        
        
        catalogue.AddBus(new_bus);
        
    }
    
    void FillDatabase (std::istream &in, TransportCatalogue &catalogue) {
        
        size_t requests_count;
        in >> requests_count;
        in.get();
        
        vector<string> buses;
        buses.reserve(requests_count);
        vector<std::pair<string, vector<std::pair<string, int>>>> distances_for_stops;
        distances_for_stops.reserve(requests_count);
        while (requests_count --) {
            string text;
            std::getline(in, text);
            if (text.substr(0, 4) == "Stop") {
                auto res = ParseStop(text.substr(5), catalogue);
                distances_for_stops.push_back(res);
            } else {
                buses.push_back(std::move(text.substr(4)));
            }
            
        }
        
        for (auto[stop_name, info]: distances_for_stops) {
            catalogue.AddDistances(stop_name, info);
        }
        
        for (const auto &bus_stops: buses) {
            ParseBus(bus_stops, catalogue);
        }
        
    }
}

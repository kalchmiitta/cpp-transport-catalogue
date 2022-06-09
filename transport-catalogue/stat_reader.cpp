#include "stat_reader.h"
#include <iomanip>
#include <iostream>
namespace catalogue::output {
    
    std::ostream &operator<< (std::ostream &out, const BusInfo &bus) {
        out << "Bus " << bus.bus_name << ": " << bus.number_of_stops << " stops on route, "
            << bus.number_of_unique_stops << " unique stops, ";
        out << bus.road_length << " route length, ";
        out << bus.road_length / bus.route_length << " curvature";
        return out;
    }
    
    void BusResponse (std::ostream &out, string_view bus_name, const TransportCatalogue &catalogue) {
        if (auto bus_information = catalogue.GetBusIfo(bus_name)) {
            out << *bus_information << '\n';
        } else {
            out << "Bus " << bus_name << ": not found\n";
        }
    }
    
    void StopResponse (std::ostream &out, string_view stop_name, const TransportCatalogue &catalogue) {
        
        if (auto stop_information = catalogue.GetStopInfo(stop_name)) {
            if (stop_information->empty()) {
                out << "Stop " << stop_name << ": no buses\n";
            } else {
                out << "Stop " << stop_name << ": buses";
                for (const auto &name: stop_information.value()) {
                    out << " " << name;
                }
                out << '\n';
            }
        } else {
            out << "Stop " << stop_name << ": not found\n";
        }
        
    }
    
    
    void Responses (std::istream &in, std::ostream &out, const TransportCatalogue &catalogue) {
        out << std::fixed << std::setprecision(6);
        size_t requests_count;
        in >> requests_count;
        in.get();
        
        vector<string> buses;
        buses.reserve(requests_count);
        
        while (requests_count --) {
            string text;
            std::getline(in, text);
            text[0] == 'S' ? StopResponse(out, text.substr(5), catalogue) : BusResponse(out, text.substr(4), catalogue);
        }
    }
}

#include "transport_catalogue.h"

namespace catalogue {
    
    size_t Bus::StopsCounter () const {
        return (type == RouteType::ring ? stops.size() : (stops.size() * 2 - 1));
    }
    
    void TransportCatalogue::AddBus (Bus bus) {
        for (auto &stop: bus.stops) {
            stop = stops_.find(stop)->first;
        }
        
        const auto added_bus = all_buses_.insert(all_buses_.begin(), std::move(bus));
        buses_.insert({added_bus->name, &(*added_bus)});
        
        for (const auto &stop: added_bus->stops) {
            buses_for_stops_[stop].insert(added_bus->name);
        }
        
    }
    
    std::optional<BusInfo> TransportCatalogue::GetBusIfo (string_view bus_name) const {
        if (! buses_.count(bus_name))
            return std::nullopt;
        
        const Bus *bus = buses_.at(bus_name);
        
        BusInfo bus_inf_result;
        bus_inf_result.bus_name = bus->name;
        bus_inf_result.number_of_stops = bus->StopsCounter();
        bus_inf_result.number_of_unique_stops = bus->unique_stops.size();
        bus_inf_result.route_length = GetGeoRouteLength(bus_name);
        bus_inf_result.road_length = GetRoadRoute(bus_name);
        
        return bus_inf_result;
    }
    
    void TransportCatalogue::AddStop (Stop stop) {
        if (! stops_.count(stop.name)) {
            const auto added_stop = all_stops_.insert(all_stops_.begin(), std::move(stop));
            stops_.insert({added_stop->name, &(*added_stop)});
        }
    }
    
    void TransportCatalogue::AddDistances (string_view stop_name, const vector<std::pair<string, int>>& distances) {
        for (auto[to_stop_name, distance]: distances) {
            distances_for_stops_.insert({{stops_.at(stop_name), stops_.at(to_stop_name)}, distance});
        }
    }
    
    double TransportCatalogue::GetGeoRouteLength (std::string_view bus_name) const {
        double result_length = 0.;
        
        if (buses_.at(bus_name)->type == RouteType::ring) {
            // расчет длины кольцевого маршрута - пройдем по всем остановками и посчитаем длину
            
            for (auto itr = buses_.at(bus_name)->stops.begin(); itr + 1 != buses_.at(bus_name)->stops.end(); itr ++) {
                auto start = stops_.at(*itr)->stop_coordinate;
                auto end = stops_.at(*(itr + 1))->stop_coordinate;
                result_length += ComputeDistance(start, end);
            }
        } else {
            // расчет длины прямого возвращающегося маршрута - пройдем по всем остановками и удвоим расстояние
            for (auto itr = buses_.at(bus_name)->stops.begin(); itr + 1 < buses_.at(bus_name)->stops.end(); itr ++) {
                geo::Coordinates crd1{stops_.at(*itr)->stop_coordinate};
                geo::Coordinates crd2{stops_.at(*(itr + 1))->stop_coordinate};
                result_length += ComputeDistance(crd1, crd2);
            }
            result_length *= 2.;
        }
        
        return result_length;
        
    }
    
    std::optional<std::set<string_view>> TransportCatalogue::GetStopInfo (string_view stop_name) const {
        if (! stops_.count(stop_name)) {
            return std::nullopt;
        }
        if (! buses_for_stops_.count(stop_name)) {
            return std::set<string_view>{};
        }
        return buses_for_stops_.at(stop_name);
    }
    
    size_t TransportCatalogue::GetRouteLengthBetweenTwoStops (string_view stop_from, string_view stop_to) const {
        const Stop *from = stops_.at(stop_from);
        const Stop *to = stops_.at(stop_to);
        if (distances_for_stops_.count({from, to})) {
            return distances_for_stops_.at({from, to});
        } else {
            return distances_for_stops_.at({to, from});
        }
    }
    
    size_t TransportCatalogue::GetRoadRoute (string_view bus_name) const {
        size_t result_length = 0;
        
        if (buses_.at(bus_name)->type == RouteType::ring) {
            // расчет длины кольцевого маршрута - пройдем по всем остановками и посчитаем длину
            
            for (auto itr = buses_.at(bus_name)->stops.begin(); itr + 1 != buses_.at(bus_name)->stops.end(); itr ++) {
                auto start = *itr;
                auto end = *(itr + 1);
                result_length += GetRouteLengthBetweenTwoStops(start, end);
            }
        } else {
            // расчет длины прямого возвращающегося маршрута - пройдем по всем остановками и удвоим расстояние
            auto itr = buses_.at(bus_name)->stops.begin();
            for (; itr + 1 != buses_.at(bus_name)->stops.end(); itr ++) {
                auto start = *itr;
                auto end = *(itr + 1);
                result_length += GetRouteLengthBetweenTwoStops(start, end);
                result_length += GetRouteLengthBetweenTwoStops(end, start);
            }
        }
        return result_length;
    }
    
}


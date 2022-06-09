#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <map>

#include "geo.h"

using std::string;
using std::string_view;
using std::vector;
using std::unordered_map;
using std::deque;

namespace catalogue {
    enum class RouteType {
        ring,
        forward
    };
    
    struct Stop {
        string name;
        geo::Coordinates stop_coordinate;
        
        [[nodiscard]]size_t Hash () const {
            return std::hash<string>{}(name) +
                   std::hash<double>{}(stop_coordinate.lat) * prime_num +
                   std::hash<double>{}(stop_coordinate.lat) * prime_num * prime_num;
        }
    
    private:
        static const size_t prime_num = 13;
    };
    
    struct Bus {
        string name;
        RouteType type;
        vector<string_view> stops;
        std::unordered_set<string_view> unique_stops;
        
        size_t StopsCounter () const;
        
    };
    
    struct BusInfo {
        string_view bus_name;
        size_t number_of_stops;
        size_t number_of_unique_stops;
        double route_length;
        size_t road_length;
    };
    
    class TransportCatalogue {
    
    public:
        TransportCatalogue() = default;
        void AddBus(Bus bus);
        void AddStop(Stop bus);
        std::optional<BusInfo> GetBusIfo(string_view bus_name) const;
        double GetGeoRouteLength(std::string_view bus_name) const;
        std::optional<std::set<string_view>> GetStopInfo(string_view stop_name) const;
        void AddDistances(string_view stop_name, const vector<std::pair<string, int>>& distances);
        size_t GetRouteLengthBetweenTwoStops(string_view sop_from, string_view stop_to) const;
        size_t GetRoadRoute(string_view bus_name) const;
    
    private:
        
        deque<Stop> all_stops_;
        unordered_map<string_view, const Stop *> stops_;
        
        deque<Bus> all_buses_;
        unordered_map<string_view, const Bus *> buses_;
        
        std::map<string_view, std::set<string_view>> buses_for_stops_;    // <имя остановки; имена автобусов>
        
        
        using StopPairs = std::pair<const Stop*, const Stop*>;
        struct StopPairHash {
            size_t operator()(const StopPairs & pair) const {
                return pair.first->Hash() + pair.second->Hash() * prime_num;
            }
        
        private:
            static const size_t prime_num = 11;
        };
        std::unordered_map<StopPairs, size_t, StopPairHash> distances_for_stops_;
        
    };
}

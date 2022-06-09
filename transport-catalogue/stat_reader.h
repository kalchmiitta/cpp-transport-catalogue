#pragma once

#include <ostream>
#include "transport_catalogue.h"

namespace catalogue::output {
    void Responses (std::istream &in, std::ostream &out, const TransportCatalogue &catalogue);
    
    void BusResponse (std::ostream &out, string_view text, const TransportCatalogue &catalogue);
    
    void StopResponse (std::ostream &out, string_view stop_name, const TransportCatalogue &catalogue);
}

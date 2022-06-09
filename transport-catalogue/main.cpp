#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"


int main(){
    catalogue::TransportCatalogue catalogue;
    catalogue::input::FillDatabase(std::cin, catalogue);
    catalogue::output::Responses(std::cin, std::cout, catalogue);
}

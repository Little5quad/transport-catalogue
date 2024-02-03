#pragma once
#include <iostream>
#include <vector>
#include <set>
#include "transport_catalogue.h"

namespace transportcatalogue{

namespace statreader{

namespace details{
struct out_q {
	std::string type;
	std::string request;
};
}//details
void PrintBus(std::ostream& output,  const std::string& request, transport::TransportCatalogue& trcat);
    
void PrintStop(std::ostream& output,  const std::string& request, transport::TransportCatalogue& trcat);
    
void Reader(std::istream& input, std::ostream& output, transport::TransportCatalogue& trcat);
    
}//statreader
}//transportcatalogue
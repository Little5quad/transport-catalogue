#include "stat_reader.h"

namespace transportcatalogue{

namespace statreader{
    
void PrintBus(std::ostream& output,  const std::string& request, transport::TransportCatalogue& trcat){
    transport::TransportCatalogue::BusInfo businf = trcat.GetBusInfo(request);
    if (businf.is_found) {
        output << "Bus " << businf.name << ": " << businf.quantity << " stops on route, " << businf.uniq_quan << " unique stops, " << businf.distance << " route length, " << businf.curvature << " curvature";
    }
    else {
        output << "Bus " << businf.name << ": not found";
    }
    output << std::endl;
}
    
void PrintStop(std::ostream& output,  const std::string& request, transport::TransportCatalogue& trcat){
    std::pair <bool,std::unordered_set<std::string>> buff= trcat.GetStopInfo(request);
    if (!buff.first) {
        output << "Stop " << request << ": not found" << std::endl;
    }
    else if (!buff.second.size()) {
        output << "Stop " << request << ": no buses" << std::endl;
    }
    else {
        std::set<std::string> buses(buff.second.begin(), buff.second.end());
        output << "Stop " << request << ": buses";
        for (auto word : buses) {
        output << " " << word;
        }
        output << std::endl;
    }  
}
    
void Reader(std::istream& input, std::ostream& output, transport::TransportCatalogue& trcat) {
    
    size_t l;
    input >> l;
    std::vector<details::out_q> que;
    for (size_t lines = 0; lines <= l; ++lines) {
        std::string line;
        std::getline(input, line);
        if (line.find("Bus") != std::string::npos && line.find("Bus") <= 4) {
            details::out_q buff;
            buff.type = "Bus";
            auto space = line.find(' ');
            buff.request = std::string(line.begin() + space + 1, line.end());
            que.push_back(buff);
        }
        if (line.find("Stop") != std::string::npos && line.find("Stop") <= 4) {
            details::out_q buff;
            buff.type = "Stop";
            auto space = line.find(' ');
            buff.request = std::string(line.begin() + space + 1, line.end());
            que.push_back(buff);
        }
    }
    for (const auto& [type, request] : que) {
        if (!request.empty()) {
            if (type == "Bus") {
                PrintBus(output, request, trcat);
            }
            else {
                if (type == "Stop") {
                    PrintStop(output, request, trcat);
                }
            }
        }
    }
}
}//statreader
}//transportcatalogue

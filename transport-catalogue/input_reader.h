#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include "stat_reader.h"

#include "transport_catalogue.h"

namespace transportcatalogue{

namespace input{

namespace details{
    
struct q {
    std::string type;
    std::string text;
};

struct que_dist {
    que_dist(std::string f, std::string t, int d) : from(f), to(t), dist(d) {
    }

    std::string from;
    std::string to;
    int dist;
};

}// namespace details
std::vector<std::string> SplitIntoSentence(const std::string& text);

std::vector<std::string> SplitIntoWords(const std::string& text);

void Load(std::istream& input, transport::TransportCatalogue& trcat);

std::pair<std::string, std::vector<std::string>> LoadStop(const std::string& text);

std::pair<std::string, std::vector<std::string>> LoadBus(const std::string& text);

void LoadDist(const std::string& from, const std::string& to, const std::string& dist);

}//input::
}//transportcatalogue
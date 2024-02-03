// напишите решение с нуля
// код сохраните в свой git-репозиторий
#include "input_reader.h"

namespace transportcatalogue{

namespace input{
    std::vector<std::string> SplitIntoSentence(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ',') {
            if (!word.empty()) {
                words.push_back(word.substr(1));
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

std::pair<std::string, std::vector<std::string>> LoadStop(const std::string& text) {
    size_t colon = text.find(':');
    std::string name({ text.begin(), text.begin() + colon });
    std::vector<std::string> words = SplitIntoSentence(std::string{ text.begin() + colon + 1, text.end() });
    return { name, words };
}

std::pair<std::string, std::vector<std::string>> LoadBus(const std::string& text) {
    std::vector<std::string> words;
    size_t colon = text.find(':');
    if (colon == std::string::npos) {
        return { text, words };
    }
    std::string name({ text.begin(), text.begin() + colon });
    std::string a = text.substr(colon + 1, text.size());
    
    if (text.find('-') != std::string::npos) {
        std::string word;
        for (const char c : a) {
            if (c == '-') {
                if (!word.empty()) {
                    words.push_back(word.substr(1, word.size() - 2));
                    word.clear();
                }
            }
            else {
                word += c;
            }
        }
        if (!word.empty()) {
            words.push_back(word.substr(1, word.size() - 1));
        }
        for (size_t i = words.size() - 2; i != -1; --i) {
            words.push_back(words[i]);
        }
    }
    else {
        std::string word;
        for (const char c : a) {
            if (c == '>') {
                if (!word.empty()) {
                    words.push_back(word.substr(1, word.size() - 2));
                    word.clear();
                }
            }
            else {
                word += c;
            }
        }
        if (!word.empty()) {
            words.push_back(word.substr(1, word.size() - 1));
        }
    }
    return { name, words };
}

void Load(std::istream& input, transport::TransportCatalogue& trcat) {
    std::vector<details::q> queue_request;
    std::vector<details::que_dist> q_dist;
    size_t l;
    input >> l;
    for (size_t lines = 0; lines <= l; ++lines) {
        std::string line; 
        std::getline(input, line);
        if (line.find("Stop") != std::string::npos && line.find("Stop") <= 4) {
            auto space = line.find(' ');
            auto [name, coordinats] = LoadStop(std::string(line.begin() + space + 1, line.end()));
            trcat.AddStop(name, {std::stod(coordinats[0]), std::stod(coordinats[1])});
            if (coordinats.size() != 2) {
                for (auto it = coordinats.begin() + 2; it != coordinats.end(); ++it) {
                    std::vector<std::string> buff_v = SplitIntoWords(*it);
                    size_t to_pos = it->find("to ");
                    std::string to = it->substr(to_pos + 3);
                    details::que_dist buff_q(name, to, std::stoi(buff_v[0].substr(0, buff_v[0].size()-1)));
                    q_dist.push_back(buff_q);
                }
            }
        }
        if (line.find("Bus") != std::string::npos && line.find("Bus") <= 4) {
            details::q buff;
            buff.type = "Bus";
            auto space = line.find(' ');
            buff.text = std::string(line.begin() + space + 1, line.end());
            queue_request.push_back(buff);
        }
    }
    for (const details::q& request : queue_request) {
        if (request.type == "Bus") {
            auto [name, stops] = LoadBus(request.text);
            trcat.AddBus(name, stops);
        }
    }
    
    for (const details::que_dist& request : q_dist) {
        trcat.SetDistanceBetween(request.from, request.to, request.dist);
    }
}
}
}//transportcatalogue

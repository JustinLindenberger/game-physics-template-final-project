#include <chrono>
#include <string>
#include <unordered_map>
#include <iostream>

struct ProfileData {
    double totalDuration = 0;
    int count = 0;
};


class Timer {
public:
    Timer(const std::string& name, std::unordered_map<std::string, ProfileData>& logs)
        : name(name), logs(logs), start(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        logs[name].totalDuration += elapsed.count();
        logs[name].count++;
        if (logs[name].count == 1000) {
            if(name=="1. Reset"){
                std::cout<<"\n------------------------------------\n";
            }
            
            // Print the result simply
            std::cout << name << " per 1000 iterations: " << logs[name].totalDuration << " ms" << std::endl;

            // Reset to 0 so we can count the next 1000 frames
            logs[name].totalDuration = 0;
            logs[name].count = 0;
        }
    }

private:
    std::string name;
    std::unordered_map<std::string, ProfileData>& logs;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

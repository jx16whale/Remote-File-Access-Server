#include "..\include\HashMap.h"

// inserts key (filename) and value (machineID,expirytime) into hashmap
void HashMap::insert(const std::string& key, const std::string& machineID,
                     const std::chrono::system_clock::time_point& expiryTime) {
    hashMap[key] = std::make_pair(machineID, expiryTime);
}

// Function to get the machineID value from the HashMap based on the key
std::string HashMap::getValue(const std::string& key) {
    // Check if the key exists in the HashMap
    if (hashMap.find(key) != hashMap.end()) {
        // Return the machineID associated with the key
        return hashMap[key].first;
    } else {
        // Return an empty string if the key does not exist
        return "";
    }
}

// remove entire entry from hashmap if expired
void HashMap::removeIfExpired() {
    auto now = std::chrono::system_clock::now();
    for (auto it = hashMap.begin(); it != hashMap.end();) {
        if (it->second.second < now) {
            it = hashMap.erase(it);
        } else {
            ++it;
        }
    }
}

void HashMap::printAll() {
    for (const auto& entry : hashMap) {
        std::cout << "Key: " << entry.first
                  << ", MachineID: " << entry.second.first << ", ExpiryTime: "
                  << std::chrono::system_clock::to_time_t(entry.second.second)
                  << std::endl;
    }
}

// removes if expired and checks if key exists
bool HashMap::contains(const std::string& keyToCheck) {
    // Remove expired entries
    removeIfExpired();

    // Check if the parameter is a key
    auto it = hashMap.find(keyToCheck);
    if (it != hashMap.end()) {
        // Key found, return true
        std::cout << "Key: " << it->first << ", Value: " << it->second.first
                  << ", ExpiryTime: "
                  << std::chrono::system_clock::to_time_t(it->second.second)
                  << std::endl;
        return true;
    } else {
        // Key not found, return false
        return false;
    }
}

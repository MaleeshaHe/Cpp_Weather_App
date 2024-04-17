#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <string>
#include "rapidjson/document.h"
#include <cmath> 
#include <vector>


// Structure to represent a location
struct Location {
    std::string id;
    std::string name;
    double latitude;
    double longitude;
};

// Vector to store locations
std::vector<Location> locations;

// Function to add a new location
void AddNewLocation() {
    Location newLocation;
    std::cout << "Enter location ID: ";
    std::cin >> newLocation.id;
    std::cout << "Enter location name: ";
    std::cin.ignore();
    std::getline(std::cin, newLocation.name);
    std::cout << "Enter latitude and longitude: ";
    std::cin >> newLocation.latitude >> newLocation.longitude;
    locations.push_back(newLocation);
    std::cout << "Location added: " << newLocation.name << std::endl;
}

// Function to remove a location
void RemoveLocation() {
    std::string locationIdOrName;
    std::cout << "Enter location ID or name to remove: ";
    std::cin >> locationIdOrName;
    bool removed = false;
    for (auto it = locations.begin(); it != locations.end(); ++it) {
        if (it->id == locationIdOrName || it->name == locationIdOrName) {
            std::cout << "Location removed: " << it->name << std::endl;
            locations.erase(it);
            removed = true;
            break;
        }
    }
    if (!removed) {
        std::cout << "Location not found." << std::endl;
    }
}

// Function to modify location data
void ModifyLocation() {
    std::string locationIdOrName;
    std::cout << "Enter location ID or name to modify: ";
    std::cin >> locationIdOrName;
    bool modified = false;
    for (auto& location : locations) {
        if (location.id == locationIdOrName || location.name == locationIdOrName) {
            std::cout << "Enter new location name: ";
            std::cin.ignore();
            std::getline(std::cin, location.name);
            std::cout << "Enter new latitude and longitude: ";
            std::cin >> location.latitude >> location.longitude;
            std::cout << "Location modified: " << location.name << std::endl;
            modified = true;
            break;
        }
    }
    if (!modified) {
        std::cout << "Location not found." << std::endl;
    }
}

// Function to search for locations
void SearchLocations() {
    std::string searchQuery;
    std::cout << "Enter the search query (name or part of name): ";
    std::cin.ignore();
    std::getline(std::cin, searchQuery);
    std::cout << "Search results for '" << searchQuery << "':" << std::endl;
    bool found = false;
    for (const auto& location : locations) {
        if (location.name.find(searchQuery) != std::string::npos) {
            std::cout << "ID: " << location.id << ", Name: " << location.name << ", Latitude: " << location.latitude << ", Longitude: " << location.longitude << std::endl;
            found = true;
        }
    }
    if (!found) {
        std::cout << "No matching locations found." << std::endl;
    }
}

// Callback function to write response data to a string
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

// Function to fetch weather data from OpenWeatherMap API using latitude and longitude
std::string GetWeatherData(const std::string& apiKey, double latitude, double longitude) {
    std::string url = "http://api.openweathermap.org/data/2.5/weather?lat=" + std::to_string(latitude) +
                      "&lon=" + std::to_string(longitude) + "&appid=" + apiKey;

    CURL *curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL.");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error("Failed to fetch weather data: " + std::string(curl_easy_strerror(res)));
    }

    return response;
}

// Function to fetch weather forecast data from OpenWeatherMap API
std::string GetWeatherForecastData(const std::string& apiKey, double latitude, double longitude) {
    std::string url = "http://api.openweathermap.org/data/2.5/forecast?lat=" + std::to_string(latitude) +
                      "&lon=" + std::to_string(longitude) + "&appid=" + apiKey;

    CURL *curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL.");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error("Failed to fetch weather forecast data: " + std::string(curl_easy_strerror(res)));
    }

    return response;
}

// Display weather forecast data
void DisplayForecast(const std::string& forecastData) {
    // Parse JSON response
    rapidjson::Document doc;
    doc.Parse(forecastData.c_str());

    if (!doc.IsObject()) {
        std::cerr << "Failed to parse JSON forecast data." << std::endl;
        return;
    }

    // Display city information
    std::cout << "City: " << doc["city"]["name"].GetString() << ", " << doc["city"]["country"].GetString() << std::endl;

    // Display each forecast entry
    const rapidjson::Value& forecastList = doc["list"];
    for (rapidjson::SizeType i = 0; i < forecastList.Size(); ++i) {
        const rapidjson::Value& forecast = forecastList[i];

        // Extract forecast details
        double temperature = forecast["main"]["temp"].GetDouble() - 273.15; // Convert from Kelvin to Celsius
        std::string description = forecast["weather"][0]["description"].GetString();
        double windSpeed = forecast["wind"]["speed"].GetDouble();
        int humidity = forecast["main"]["humidity"].GetInt();
        std::string forecastTime = forecast["dt_txt"].GetString();

        // Display forecast details
        std::cout << "Forecast at " << forecastTime << ":" << std::endl;
        std::cout << "    Description: " << description << std::endl;
        std::cout << "    Temperature: " << temperature << " °C" << std::endl;
        std::cout << "    Wind Speed: " << windSpeed << " m/s" << std::endl;
        std::cout << "    Humidity: " << humidity << " %" << std::endl;
        std::cout << std::endl;
    }
}

// Function to export weather data as JSON
void ExportWeatherDataJSON(const std::string& weatherData) {
    // Write JSON data to a file
    std::ofstream outFile("weather_data.json");
    if (outFile.is_open()) {
        outFile << weatherData;
        outFile.close();
        std::cout << "Weather data exported to weather_data.json" << std::endl;
    } else {
        std::cerr << "Error opening file for JSON export." << std::endl;
    }
}

// Function to export weather forecast data as JSON
void ExportWeatherForecastJSON(const std::string& forecastData) {
    // Write JSON data to a file
    std::ofstream outFile("weather_forecast.json");
    if (outFile.is_open()) {
        outFile << forecastData;
        outFile.close();
        std::cout << "Weather forecast data exported to weather_forecast.json" << std::endl;
    } else {
        std::cerr << "Error opening file for JSON export." << std::endl;
    }
}

// Function to export weather data as CSV
void ExportWeatherDataCSV(const std::string& weatherData) {
    // Parse JSON response
    rapidjson::Document doc;
    doc.Parse(weatherData.c_str());

    if (!doc.IsObject()) {
        std::cerr << "Failed to parse JSON response." << std::endl;
        return;
    }

    // Write CSV data to a file
    std::ofstream outFile("weather_data.csv");
    if (outFile.is_open()) {
        // Write CSV header
        outFile << "City,Country,Description,Temperature (C),Pressure (hPa),Humidity (%),Wind Speed (m/s)\n";

        // Extract and write weather data
        outFile << doc["name"].GetString() << ","
                << doc["sys"]["country"].GetString() << ","
                << doc["weather"][0]["description"].GetString() << ","
                << std::fixed << round((doc["main"]["temp"].GetDouble() - 273.15) * 100) / 100 << ","
                << doc["main"]["pressure"].GetInt() << ","
                << doc["main"]["humidity"].GetInt() << ","
                << doc["wind"]["speed"].GetDouble() << "\n";
        
        outFile.close();
        std::cout << "Weather data exported to weather_data.csv" << std::endl;
    } else {
        std::cerr << "Error opening file for CSV export." << std::endl;
    }
}

// Function to export weather forecast data as CSV
void ExportWeatherForecastCSV(const std::string& forecastData) {
    // Parse JSON response
    rapidjson::Document doc;
    doc.Parse(forecastData.c_str());

    if (!doc.IsObject()) {
        std::cerr << "Failed to parse JSON forecast data." << std::endl;
        return;
    }

    // Write CSV data to a file
    std::ofstream outFile("weather_forecast.csv");
    if (outFile.is_open()) {
        // Write CSV header
        outFile << "DateTime,Description,Temperature (C),Wind Speed (m/s),Humidity (%)\n";

        // Extract and write forecast data
        const rapidjson::Value& forecastList = doc["list"];
        for (rapidjson::SizeType i = 0; i < forecastList.Size(); ++i) {
            const rapidjson::Value& forecast = forecastList[i];

            // Extract forecast details
            double temperature = forecast["main"]["temp"].GetDouble() - 273.15; // Convert from Kelvin to Celsius
            std::string description = forecast["weather"][0]["description"].GetString();
            double windSpeed = forecast["wind"]["speed"].GetDouble();
            int humidity = forecast["main"]["humidity"].GetInt();
            std::string forecastTime = forecast["dt_txt"].GetString();

            // Write forecast details to CSV
            outFile << forecastTime << ","
                    << description << ","
                    << std::fixed << round(temperature * 100) / 100 << ","
                    << windSpeed << ","
                    << humidity << "\n";
        }

        outFile.close();
        std::cout << "Weather forecast data exported to weather_forecast.csv" << std::endl;
    } else {
        std::cerr << "Error opening file for CSV export." << std::endl;
    }
}

// Function to set favorite locations
void SetFavoriteLocations() {
    // Implement functionality to set favorite locations
    // This is just a placeholder function
    std::cout << "Favorite locations set." << std::endl;
}

int main() {
    const std::string apiKey = "17ae376a54fb6b12ab8f803e775ea1df";
    int choice;

    while (true) {
        std::cout << "Weather Application Menu:" << std::endl;
        std::cout << "1. Get Weather Data" << std::endl;
        std::cout << "2. Get Weather Forecast Data" << std::endl;
        std::cout << "3. Export Weather Data as JSON" << std::endl;
        std::cout << "4. Export Weather Data as CSV" << std::endl;
        std::cout << "5. Export Weather Forecast Data as JSON" << std::endl;
        std::cout << "6. Export Weather Forecast Data as CSV" << std::endl;
        std::cout << "7. Add New Location" << std::endl;
        std::cout << "8. Remove Location" << std::endl;
        std::cout << "9. Modify Location" << std::endl;
        std::cout << "10. Search Locations" << std::endl;
        std::cout << "11. Set Favorite Locations" << std::endl;
        std::cout << "12. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                double latitude, longitude;
                std::cout << "Enter latitude and longitude (e.g., 44.34 10.99): ";
                std::cin >> latitude >> longitude;

                try {
                    std::string weatherData = GetWeatherData(apiKey, latitude, longitude);
                    // Process weather data as needed
                    
                    
                    // Parse JSON response
                    rapidjson::Document doc;
                    doc.Parse(weatherData.c_str());

                    if (!doc.IsObject()) {
                        std::cerr << "Failed to parse JSON response." << std::endl;
                        break;
                    }

                    // Extract and display relevant information
                    std::cout << "Weather data for " << doc["name"].GetString() << ":\n";
                    std::cout << "Description: " << doc["weather"][0]["description"].GetString() << std::endl;
                    std::cout << "Temperature: " << (doc["main"]["temp"].GetDouble() - 273.15) << " °C" << std::endl;
                    std::cout << "Pressure: " << doc["main"]["pressure"].GetInt() << " hPa" << std::endl;
                    std::cout << "Humidity: " << doc["main"]["humidity"].GetInt() << " %" << std::endl;
                    std::cout << "Wind Speed: " << doc["wind"]["speed"].GetDouble() << " m/s" << std::endl;
                    
                    
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 2: {
            	double latitude, longitude;
                std::cout << "Enter latitude and longitude (e.g., 44.34 10.99): ";
                std::cin >> latitude >> longitude;

                try {
                    std::string forecastData = GetWeatherForecastData(apiKey, latitude, longitude);
                    DisplayForecast(forecastData);
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 3: {
                double latitude, longitude;
                std::cout << "Enter latitude and longitude (e.g., 44.34 10.99): ";
                std::cin >> latitude >> longitude;

                try {
                    std::string weatherData = GetWeatherData(apiKey, latitude, longitude);
                    ExportWeatherDataJSON(weatherData);
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 4: {
                
                double latitude, longitude;
                std::cout << "Enter latitude and longitude (e.g., 44.34 10.99): ";
                std::cin >> latitude >> longitude;

                try {
                    std::string weatherData = GetWeatherData(apiKey, latitude, longitude);
                    ExportWeatherDataCSV(weatherData);
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
	
	    case 5: {
                double latitude, longitude;
                std::cout << "Enter latitude and longitude (e.g., 44.34 10.99): ";
                std::cin >> latitude >> longitude;

                try {
                    std::string forecastData = GetWeatherForecastData(apiKey, latitude, longitude);
                    ExportWeatherForecastJSON(forecastData);
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 6: {
                double latitude, longitude;
                std::cout << "Enter latitude and longitude (e.g., 44.34 10.99): ";
                std::cin >> latitude >> longitude;

                try {
                    std::string forecastData = GetWeatherForecastData(apiKey, latitude, longitude);
                    ExportWeatherForecastCSV(forecastData);
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
             case 7: {
                AddNewLocation();
                break;
            }
            case 8: {
                RemoveLocation();
                break;
            }
            case 9: {
                ModifyLocation();
                break;
            }
            case 10: {
                SearchLocations();
                break;
            }
            case 11: {
                SetFavoriteLocations();
                break;
            }
            case 12:
                std::cout << "Exiting program. Goodbye!" << std::endl;
                return 0;

            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}



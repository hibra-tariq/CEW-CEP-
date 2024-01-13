#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
char *makeHttpRequest(const char *url, const char *apiKey, const char *location, long *httpCode);
void processWeatherData(const char *response);

int main() {
    const char *API_URL = "http://api.weatherapi.com/v1/current.json";
    const char *API_KEY = "3d9e7a6fceae4eb98d540834231812";
    const char *LOCATION = "karachi";

    long httpCode;
    char *response = makeHttpRequest(API_URL, API_KEY, LOCATION, &httpCode);

    if (httpCode == 200) {
        // Parse the JSON response using jansson
        json_t *root;
        json_error_t error;
        root = json_loads(response, 0, &error);

        if (root) {
            processWeatherData(response);

            // Print the information to the console
            const char *locationName = json_string_value(json_object_get(json_object_get(root, "location"), "name"));
            const char *region = json_string_value(json_object_get(json_object_get(root, "location"), "region"));
            const char *country = json_string_value(json_object_get(json_object_get(root, "location"), "country"));
            double latitude = json_real_value(json_object_get(json_object_get(root, "location"), "lat"));
            double longitude = json_real_value(json_object_get(json_object_get(root, "location"), "lon"));
            double temperatureC = json_real_value(json_object_get(json_object_get(root, "current"), "temp_c"));
            double windSpeedKph = json_real_value(json_object_get(json_object_get(root, "current"), "wind_kph"));
            double visibilityKm = json_real_value(json_object_get(json_object_get(root, "current"), "vis_km"));
            const char *condition = json_string_value(json_object_get(json_object_get(json_object_get(root, "current"), "condition"), "text"));

            printf("Location: %s, %s, %s\n", locationName, region, country);
            printf("Latitude: %.6f\n", latitude);
            printf("Longitude: %.6f\n", longitude);
            printf("Temperature: %.2f°C\n", temperatureC);
            printf("Wind Speed: %.2f km/h\n", windSpeedKph);
            printf("Visibility: %.2f km\n", visibilityKm);
            printf("Condition: %s\n", condition);

            // Save the information to a text file in append mode
            FILE *file = fopen("raw_data.txt", "a");

            if (file != NULL) {
                fprintf(file, "Location: %s, %s, %s\n", locationName, region, country);
    		fprintf(file, "Latitude: %.6f\n", latitude);
    		fprintf(file, "Longitude: %.6f\n", longitude);
    		fprintf(file, "Temperature: %.2f\n", temperatureC);
    		fprintf(file, "Wind Speed: %.2f\n", windSpeedKph);
    		fprintf(file, "Visibility: %.2f\n", visibilityKm);

                fclose(file);
                printf("Weather information appended to 'raw_data.txt'\n");
            } else {
                fprintf(stderr, "Failed to open the file for writing\n");
            }

            // Free the JSON object
            json_decref(root);
        } else {
            fprintf(stderr, "Failed to parse JSON: %s\n", error.text);
        }
    } else {
        fprintf(stderr, "HTTP request failed with status code: %ld\n", httpCode);
    }

    free(response); // Free the API response
    return 0;
}

void processWeatherData(const char *response) {
    // You can put your data processing logic here if needed
    // This function is called after each API response is received
}

char *makeHttpRequest(const char *url, const char *apiKey, const char *location, long *httpCode) {
    char apiUrl[256];
    snprintf(apiUrl, sizeof(apiUrl), "%s?key=%s&q=%s", url, apiKey, location);

    CURL *curl = curl_easy_init();
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        // Exclude headers from the response
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, httpCode);
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
        *httpCode = -1;
    }

    if (*httpCode != 200) {
        fprintf(stderr, "HTTP request failed with status code: %ld\n", *httpCode);
        free(chunk.memory);
        return NULL;
    }

    return strdup(chunk.memory);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    // Allocate memory for the new content
    char *temp = realloc(mem->memory, mem->size + realsize + 1);
    if (temp == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }

    mem->memory = temp;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}



/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
char *makeHttpRequest(const char *url, const char *apiKey, const char *location, long *httpCode);
void processWeatherData(const char *response);

int main() {
    const char *API_URL = "http://api.weatherapi.com/v1/current.json";
    const char *API_KEY = "3d9e7a6fceae4eb98d540834231812";
    const char *LOCATION = "karachi";

    long httpCode;
    char *response = makeHttpRequest(API_URL, API_KEY, LOCATION, &httpCode);

    if (httpCode == 200) {
        // Parse the JSON response using jansson
        json_t *root;
        json_error_t error;
        root = json_loads(response, 0, &error);

        if (root) {
            processWeatherData(response);

            // Print the information to the console
            const char *locationName = json_string_value(json_object_get(json_object_get(root, "location"), "name"));
            const char *region = json_string_value(json_object_get(json_object_get(root, "location"), "region"));
            const char *country = json_string_value(json_object_get(json_object_get(root, "location"), "country"));
            double latitude = json_real_value(json_object_get(json_object_get(root, "location"), "lat"));
            double longitude = json_real_value(json_object_get(json_object_get(root, "location"), "lon"));
	    double temperatureC = json_real_value(json_object_get(json_object_get(root, "current"), "temp_c"));
	    double windSpeedKph = json_real_value(json_object_get(json_object_get(root, "current"), "wind_kph"));
            double visibilityKm = json_real_value(json_object_get(json_object_get(root, "current"), "vis_km"));
  
            /*double temperatureC = json_real_value(json_object_get(json_object_get(root, "current"), "temp_c"));
            const char *condition = json_string_value(json_object_get(json_object_get(json_object_get(root, "current"), "condition"), "text"));
            double windSpeedKph = json_real_value(json_object_get(json_object_get(root, "current"), "wind_kph"));
            double visibilityKm = json_real_value(json_object_get(json_object_get(root, "current"), "vis_km"));
*/
/*            printf("Location: %s, %s, %s\n", locationName, region, country);
            printf("Latitude: %.6f\n", latitude);
            printf("Longitude: %.6f\n", longitude);
            printf("Temperature: %.2f°C\n", temperatureC);
            printf("Condition: %s\n", condition);
            printf("Wind Speed: %.2f km/h\n", windSpeedKph);
            printf("Visibility: %.2f km\n", visibilityKm);
            
            // Save the information to a text file in append mode
            FILE *file = fopen("raw_data.txt", "a");

            if (file != NULL) {
                fprintf(file, "Location: %s, %s, %s\n", locationName, region, country);
                fprintf(file, "Latitude: %.6f\n", latitude);
                fprintf(file, "Longitude: %.6f\n", longitude);
                fprintf(file, "Temperature: %.2f\n", temperatureC);
                fprintf(file, "Wind Speed: %.2f\n", windSpeedKph);
                fprintf(file, "Visibility: %.2f\n", visibilityKm);
                //fprintf(file, "\n"); // Add a separator between entries

                fclose(file);
                printf("Weather information appended to 'raw_data.txt'\n");
            } else {
                fprintf(stderr, "Failed to open the file for writing\n");
            }

            // Free the JSON object
            json_decref(root);
        } else {
            fprintf(stderr, "Failed to parse JSON: %s\n", error.text);
        }
    } else {
        fprintf(stderr, "HTTP request failed with status code: %ld\n", httpCode);
    }

    free(response); // Free the API response
    return 0;
}

void processWeatherData(const char *response) {
    // You can put your data processing logic here if needed
    // This function is called after each API response is received
}

char *makeHttpRequest(const char *url, const char *apiKey, const char *location, long *httpCode) {
    char apiUrl[256];
    snprintf(apiUrl, sizeof(apiUrl), "%s?key=%s&q=%s", url, apiKey, location);

    CURL *curl = curl_easy_init();
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        // Exclude headers from the response
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, httpCode);
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
        *httpCode = -1;
    }

    if (*httpCode != 200) {
        fprintf(stderr, "HTTP request failed with status code: %ld\n", *httpCode);
        free(chunk.memory);
        return NULL;
    }

    return strdup(chunk.memory);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    // Allocate memory for the new content
    char *temp = realloc(mem->memory, mem->size + realsize + 1);
    if (temp == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }

    mem->memory = temp;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}
*/



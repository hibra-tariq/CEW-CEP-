#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *readFile = fopen("processed_data.txt", "r");

    if (readFile != NULL) {
        double meanTemperature, meanWindSpeed, meanVisibility;

        // Read mean values from the file
        int readCount = fscanf(readFile, "Mean Temperature: %lf\nMean Wind Speed: %lf\nMean Visibility: %lf", &meanTemperature, &meanWindSpeed, &meanVisibility);

        // Move the read file pointer to the beginning of the file
        fseek(readFile, 0, SEEK_SET);

        if (readCount == 3) {
            // Check if mean visibility is below a certain threshold
            if (meanVisibility < 4.0) {
                printf("Low Visibility Alert: %.2f km - Use Fog Lights While Driving, Avoid Unnecessary Outing, Stay At Home\n", meanVisibility);

                // Append the log line to alert_log.txt
                FILE *logFile = fopen("alert_log.txt", "a");
                if (logFile != NULL) {
                    fprintf(logFile, "Low Visibility Alert %.2f recorded at  Date: %s, Time: %s  \n", meanVisibility, __TIME__, __DATE__);
                    fclose(logFile);

                    printf("Email sent successfully\n");
                } else {
                    fprintf(stderr, "Failed to open or create the alert log file for writing\n");
                }

                // Continue with the email sending code
                char email_command[1024];
                snprintf(email_command, sizeof(email_command),
                         "echo -e 'From: muhammadubaid182004@gmail.com\nTo: alishbaahmed1509@gmail.com\nSubject: Low Visibility Alert\n\nVisibility is less than the Threshold - Use Fog Lights While 		Driving, Avoid Unnecessary Outing, Stay At Home: %.2f(km)' | "
                         "curl --ssl-reqd --url 'smtps://smtp.gmail.com:465' "
                         "-u muhammadubaid182004@gmail.com:vbwdvqzpfjzzjvvr  " // Fixed authentication credentials
                         "--mail-from 'muhammadubaid182004@gmail.com' "
                         "--mail-rcpt 'alishbaahmed1509@gmail.com' "
                         "--upload-file -", // Remove --mail-auth for OAuth 2.0
                         meanVisibility);
                system(email_command);
                char notify_command[256];
    		snprintf(notify_command, sizeof(notify_command), "notify-send 'Low Visibility alert' 'Visibility is lower than: %.2f km'", meanVisibility);
    		system(notify_command);
                
            } else {
                printf("Visibility is within the normal range: %.2f km\n", meanVisibility);
            }
        } else {
            fprintf(stderr, "Failed to read mean values from the file. Read Count: %d\n", readCount);
        }

        // Close the read file
        fclose(readFile);
    } else {
        fprintf(stderr, "Failed to open the file for reading\n");
    }

    return 0;
    
}


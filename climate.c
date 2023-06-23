/**
 * climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  gcc -g -Wall -o climate climate.c
 *           (or run make)
 *
 * Run:      ./climate data_tn.tdv data_wa.tdv
 *
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: WA TN
  * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F on Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F on Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F on Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F on Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%
 *
 * TDV format:
 *
 * CA 1428300000000 9prc 93.0 0.0 100.0 0.0 95644.0  277.58716
 * CA 1430308800000 9prc 4.0  0.0 100.0 0.0 99226.0  282.63037
 * CA 1428559200000 9prr 61.0 0.0 0.0   0.0 102112.0 285.07513
 * CA 1428192000000 9prk 57.0 0.0 100.0 0.0 101765.0 285.21332
 * CA 1428170400000 9prd 73.0 0.0 22.0  0.0 102074.0 285.10425
 * CA 1429768800000 9pr6 38.0 0.0 0.0   0.0 101679.0 283.9342
 * CA 1428127200000 9prj 98.0 0.0 100.0 0.0 102343.0 285.75
 * CA 1428408000000 9pr4 93.0 0.0 100.0 0.0 100645.0 285.82413
 *
 * Each field is separated by a tab character \t and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50

/* TODO: Add elements to the climate_info struct as necessary. */
struct climate_info 
{
    char code[3]; 
    unsigned long num_records;

    //Elements for humidity
    long double humidity;
    long double avg_humidity;

    //Element for snow
    int snow;

    //Elements for cloud coverage
    long double cloudcov;
    long double avg_cloudcov;

    //Element for lightning strikes
    int lightning;

    //Elements for pressure
    long double pressure;

    //Elements for surface temperature
    long double temp; 
    long double avg_temp;
    long double max_temp;
    long double min_temp;

    //Elements for timestamp
    int time;
    long int max_time;
    long int min_time;
};

struct climate_info *create_state(char* tokens[], int tokenIndex);
void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);

int main(int argc, char *argv[]) 
{
    /* TODO: fix this conditional. You should be able to read multiple files. */
    if (argc < 2) {
        printf("Usage: %s tdv_file1 tdv_file2 ... tdv_fileN \n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = {NULL};
    FILE* fp;
    char* filename = "";
    int i;

    for (i = 1; i < argc; ++i) {

        /* TODO: Open the file for reading */
        filename = argv[i];
        printf("Opening file: %s\n", filename);
        fp = fopen(filename, "r");

        /* TODO: If the file doesn't exist, print an error message and move on
         * to the next file. */
        if(fp == NULL) {
            printf("Error in opening file.\n");
            return EXIT_FAILURE;
        }

        /* TODO: Analyze the file */
        /* analyze_file(file, states, NUM_STATES); */
        analyze_file(fp, states, NUM_STATES);
        fclose(fp);
    }

    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);

    return 0;
}


//Helper function to allocate space in memory and store information
struct climate_info *create_state(char* tokens[], int tokenIndex)

{ 
    struct climate_info* state = malloc(sizeof(struct climate_info));
    strncpy(state->code, tokens[0], 2);
    state->time = (atoi(tokens[1]))/1000;
    state->humidity = atof(tokens[3]);
    state->snow = atof(tokens[4]);
    state->cloudcov = atof(tokens[5]);
    state->lightning = atof(tokens[6]);
    state->pressure = atof(tokens[7]);
    state->temp = ((atof(tokens[8])) * 1.8 - 459.67);
    //Kelvin to Farenheit is calculated as follows: (K * 1.8) - 459.67

    return state;
}

//Helper function to tokenize each line
struct climate_info *state_tokens(char* line)
{ 
    char* token = strtok(line, "\t");
    char* tokens[9];

    int index = 0;

    while (token != NULL && index < 9) {
        tokens[index++] = token;
        token = strtok(NULL, "\t");
    }

    return create_state(tokens, index);

}


//Helper function to add each state to the state array if it is not  there yet
void add_state(struct climate_info *states[], struct climate_info *state, int index)
{
        
        if (states[index] == NULL) {
            states[index] = state;
            states[index]->num_records = 1;

        }

}

//Helper function to update information on each state if the state is already in array
void add_stateinfo(struct climate_info *states[], struct climate_info *state, int index)
{
    long double max_temp = state->temp;
    long double min_temp = state->temp;

    states[index]->num_records++;

    //Updating humidity
    states[index]->humidity += state->humidity;

    //Updating average humidity
    states[index]->avg_humidity = ((states[index]-> humidity)/states[index]->num_records);

    //Updating snow
    states[index]->snow += state->snow; 

    //Updating cloud coverage
    states[index]->cloudcov += state->cloudcov;

    //Updating average cloud coverage
    states[index]->avg_cloudcov = ((states[index]->cloudcov)/states[index]->num_records);

    //Updating lightning strikes 
    states[index]->lightning += state->lightning;

    //Updating pressure
    states[index]->pressure += state->pressure;

    //Updating surface temperature
    states[index]->temp += state->temp;

    //Updating average temperature
    states[index]->avg_temp = ((states[index]-> temp)/states[index]->num_records);
    
    //Updating max temperature
    if (max_temp > states[index]->max_temp) {
        states[index]->max_temp = state->temp;
        states[index]->max_time = state->time;
    }

    //Updating min temp
    if (min_temp < states[index]->min_temp) {
        states[index]->min_temp = state->temp;
        states[index]->min_time = state->time;
    }

}


void analyze_file(FILE *file, struct climate_info **states, int num_states) 
{

    const int line_sz = 100;
    char line[line_sz];

    while (fgets(line, line_sz, file) != NULL) {
        struct climate_info *state = state_tokens(line);

        if (state == NULL) {
            continue;
        }

        char *code = state->code;

        for(int i = 0; i < num_states;i++) {                                                   

            if (states[i] == NULL) {                                         
                add_state(states, state,i);                 
                break;                                      
            } else {                                        
                int val = strcmp(states[i]->code, code);    
                
                if (val == 0) {                                           
                    add_stateinfo(states, state, i);             
                    break;                                  
                }                                           

            }                                               

        }         
        
    }

}

void print_report(struct climate_info *states[], int num_states)
{
    printf("States found: ");
    int i;
    for (i = 0; i < num_states; ++i) 
    {
        if (states[i] != NULL) 
        {
            struct climate_info *info = states[i];
            printf("%s ", info->code);
        }
    }
    printf("\n");

    for (i = 0; i < num_states; ++i) 
    {
        if (states[i] != NULL) 
        {
            printf("State: %s\n", states[i]->code); 
            printf("Number of Records: %lu\n",states[i]->num_records);
            printf("Average Humidity: %0.1Lf%s\n", states[i]->avg_humidity, "%");
            printf("Average Temperature: %0.1Lf%s\n", states[i]->avg_temp,"F");
            printf("Max Temperature: %0.1Lf%s", states[i]->max_temp,"F on ");
            printf("%s", ctime(&states[i]->max_time));
            printf("Min Temperature: %0.1Lf%s", states[i]->min_temp, "F on ");
            printf("%s", ctime(&states[i]->min_time));
            printf("Lightning Strikes: %d\n", states[i]->lightning);
            printf("Records with Snow Cover: %d\n", states[i]->snow);
            printf("Average Cloud Cover: %0.1Lf%s\n", states[i]->avg_cloudcov, "%");
            printf("%s\n", "---------------------------");
        }
    }
}
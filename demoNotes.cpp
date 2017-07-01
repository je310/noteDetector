#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <iostream>

#define SAMPLING_RATE	8000.0	//8kHz
#define N	205	//Block size
#define PI 3.14

float floatN;

// a class holding the info and state for each note. State is wiped after each run, performance could be improved by keeping it and operation on a circular buffer.
class note{
public:
    int num;
    float freq;
    std::string str;
    float Q1;
    float Q2;
    float sine;
    float cosine;
    float coeff;
    int k;
    float	omega;
    float mag;
};

//where the samples are stored
unsigned char testData[N];


void runGeortzel(note &inNote)
{

    inNote.Q2 = 0;
    inNote.Q1 = 0;

    printf("For test frequency %f:\n", inNote.freq);

    /* Process the samples */
    for (int index = 0; index < N; index++)
    {
        float Q0;
        Q0 = inNote.coeff * inNote.Q1 - inNote.Q2 + (float) testData[index];
        inNote.Q2 = inNote.Q1;
        inNote.Q1 = Q0;
    }


    inNote.mag = inNote.Q1 * inNote.Q1 + inNote.Q2 * inNote.Q2 - inNote.Q1 * inNote.Q2 * inNote.coeff;
    printf("Relative magnitude squared = %f\n", inNote.mag);

    inNote.Q2 = 0;
    inNote.Q1 = 0;
}


/* Synthesize some test data at a given frequency. */
void Generate(float frequency)
{
    int	index;
    float	step;

    step = frequency * ((2.0 * PI) / SAMPLING_RATE);

    /* Generate the test data */
    for (index = 0; index < N; index++)
    {
        testData[index] = (unsigned char) (100.0 * sin(index * step) + 100.0);
    }
}

//numbers here for easy changing will be loaded into 'note' class.
const int noteNum = 7;
int nums[noteNum] ={48,50,52,53,55,57,59}; //midi note numbers.
float freqs[noteNum] = {261.63,293.66,329.63,349.23,392.00,440.00,493.88}; //correct notes for c4 , d4, e4 ect.
std::string strs[noteNum] = {"C4","D4","E4","F4","G4","A4","B4"};

std::vector<note> notes;
int main(void)
{
    //load up vector with notes to look for.
    for(int i = 0; i < noteNum; i++){
        note thisNote;
        thisNote.freq = freqs[i];
        thisNote.num = nums[i];
        thisNote.str = strs[i];
        notes.push_back(thisNote);
    }


    //initialise numbers that don't change.
    floatN = (float) N;
    for(int i = 0; i < notes.size(); i++){
        notes[i].k = (int) (0.5 + ((floatN * notes[i].freq) / SAMPLING_RATE));
        notes[i].omega = (2.0 * PI * notes[i].k) / floatN;
        notes[i].sine = sin(notes[i].omega);
        notes[i].cosine = cos(notes[i].omega);
        notes[i].coeff = 2.0 * notes[i].cosine;
    }

    // generate a note of a particular frequency.
    Generate(460);

    //run Geortzel for each of the notes.
    for(int i = 0; i < notes.size(); i++){
        runGeortzel(notes[i]);
    }

    //return the biggest one, as long as it is bigger than the threshold.
    float thresh = 10000000;
    int largestNote= -1;
    float largestMag = 0;
    for(int i = 0; i < notes.size(); i++){
        if(notes[i].mag > largestMag && notes[i].mag > thresh){
            largestNote = i;
            largestMag = notes[i].mag;
        }
    }
    if(largestNote!=-1) std::cout << "I heard a " << notes[largestNote].str << std::endl;
    else std::cout << "I did not hear anything"<< std::endl;
    return 0;
}

#include "math.h"
#include "adc.h"

#define PI 3.1415
#define SAMPLE_RATE 24000
Serial pc(USBTX, USBRX); // tx, rx

PwmOut loc1_led(LED1);
PwmOut loc2_led(LED2);
PwmOut loc3_led(LED3);
PwmOut loc4_led(LED4);

int Counter = 0;
int Buffer[6000];
int spread = 4;
PinName active_pin;

ADC adc(SAMPLE_RATE, 1);

/*
Algorithm as defined on Wikipedia:

Nterms defined here
Kterm selected here
ω = 2 * π * Kterm / Nterms;
cr = cos(ω);
ci = sin(ω);
coeff = 2 * cr;

sprev = 0;
sprev2 = 0;
for each index n in range 0 to Nterms-1
  s = x[n] + coeff * sprev - sprev2;
  sprev2 = sprev;
  sprev = s;
end

power = sprev2 * sprev2 + sprev * sprev - coeff * sprev * sprev2;

*/
float goertzelFilter(int samples[], float frequency, int N) {
    float s_prev = 0.0;
    float s_prev2 = 0.0;
    float coeff;
    float normalizedFrequency;
    float power;
    float s;
    float w;
    float cr;
    //float ci;         //Listed in algorithm but never used: Commented out!
    int i;

    //Since we are dealing with measurements at a specific sampling rate, 
    //we need use the normalized frequency (cycles/sample, not cycles/unit of time)
    normalizedFrequency = frequency / SAMPLE_RATE;      
    
    //In this case, our normalised frequency is our kterm. We have no nterms here
    //as it is 1 in this instance, so can be left out.
    w = 2 * PI * normalizedFrequency;
    //ci = sin(w);
    cr = cos(w);
    coeff = 2*cr;
    for (i=0; i<N; i++) {
        s = samples[i] + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
    }
    power = s_prev2*s_prev2+s_prev*s_prev-coeff*s_prev*s_prev2;
    return power;
}

void readPin(int chan, uint32_t value) {
    Buffer[Counter] = adc.read(active_pin);
    Counter += 1;
}

void read(PinName pin)
{
        active_pin = pin;
        
        adc.append(readPin);
        adc.startmode(0,0);
        adc.burst(1);
        adc.setup(pin,1);
        adc.interrupt_state(pin,1);
        wait(.1);
        adc.interrupt_state(pin,0);
        adc.setup(pin,0);
        adc.append(readPin);
}

float extractFreq(int frequency)
{
    float count = 0;
    
    for(int i = -(spread / 2); i < (spread / 2); i++)
    {
        count += goertzelFilter(Buffer, (frequency + i), Counter);
    }      
    
    count = count / spread;
    return count;
}

void process(PwmOut led)
{
    float count_500 = extractFreq(500);   
    float count_550 = extractFreq(550);
         
    if(count_500 > 100000000000)
    {
        led = 1;
    }
    else if(count_550 > 100000000000)
    {
        led = 0.005;
    }
    else
    {
        led = 0;
    }
    
    Counter = 0; 
}
    

int main() {

    while(1) {
        read(p17);    
        process(loc1_led);
        
        read(p18);    
        process(loc2_led);
        
        read(p19);    
        process(loc3_led);
        
        read(p20);    
        process(loc4_led);
       
    }
}

#ifndef MLX90614_HANDLER_H
#define MLX90614_HANDLER_H

#include <Adafruit_MLX90614.h>

class MLX90614Handler {
public:
    MLX90614Handler();
    void begin();
    float update();
    
private:
    Adafruit_MLX90614 mlx;
    bool isSampling = false;
    int sampleCount = 0;
    float ambientSum = 0.0f;
    float objectSum = 0.0f;
};

#endif
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

using namespace std;

struct Ball{
    double x;
    double y;
};

struct Environment{
    string rawData;
    double field_lenght;
    double field_width;
    Ball ball;
};

#endif
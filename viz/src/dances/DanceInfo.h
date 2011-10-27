#pragma once

//#include "danceVideo.h"
#include <string>
using namespace std;

struct DanceInfo {
    int id;
    string hash;
    string city;
    string url;
    string creationTime;
    int numFrames;
    bool isNew;
    bool largeVideoDownloaded;
};
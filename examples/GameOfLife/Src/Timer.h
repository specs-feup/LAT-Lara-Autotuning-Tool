#ifndef TIMER_H_
#define TIMER_H_

// A simple timer for Linux and Windows
// @version: 1.0
// @author: Ralf Ratering, Peter Post

class Timer {
public:
	Timer() : startStamp(0), endStamp(0) {}
	~Timer() {}
	void start();
	void stop();
	void reset();
	long getTimeInMS();
	unsigned long long readRDTSC();

private:
	long long startStamp, endStamp;
	long long getTimeStamp(); 
};

#endif /*TIMER_H_*/
#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H 1

#include <string>

struct Subscriber;

struct Subscribers_process {
	virtual void Process(std::vector<std::shared_ptr<Figure>> &buffer) = 0;
	virtual ~Subscribers_process() = default;
};

struct Console_process : Subscribers_process {
	void Process(std::vector<std::shared_ptr<Figure>> &buffer) override {
		for (const auto figure : buffer) {
			figure->Print(std::cout);
		}
	}
};

struct File_process : Subscribers_process {
	void Process(std::vector<std::shared_ptr<Figure>> &buffer) override {
		std::ofstream os(std::to_string(name));
		for (const auto figure : buffer) {
			figure->Print(os);
		}
		name++;
	}

	size_t name = 0;
};

struct Subscriber {

	void operator()() {
		for(;;) {			
			std::unique_lock<std::mutex> guard(mtx);
			cv.wait(guard, [&](){
				return buffer.size() == buffer.capacity() || end; 
			});
			if (end) {
				break;
			}
			for (size_t i = 0; i < processes.size(); i++) {
				processes[i]->Process(buffer);
			}
			buffer.clear();
			success = true;
			cv.notify_all();
		}
	}

	bool end = false;
	bool success = false;
	std::vector<std::shared_ptr<Figure>> buffer;
	std::vector<std::shared_ptr<Subscribers_process>> processes;
	std::condition_variable cv;
	std::mutex mtx;
};

#endif // SUBSCRIBER_H
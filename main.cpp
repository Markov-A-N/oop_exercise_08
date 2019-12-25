#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <cstdlib>

#include "factory.h"
#include "figures.h"

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
		std::string filename;
		std::cin >> filename;
		std::ofstream os(filename);
		for (const auto figure : buffer) {
			figure->Print(os);
		}
	}
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
			for (int i = 0; i < processes.size(); i++) {
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

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cout << "./name size\n";
		return 1;
	}

	int vector_size = std::atoi(argv[1]);
	Factory factory;
	
	Subscriber subscriber;
	subscriber.buffer.reserve(vector_size);
	std::cout << "i'm here\n";
	subscriber.processes.push_back(std::make_shared<Console_process>());
	subscriber.processes.push_back(std::make_shared<File_process>());

	std::thread subscriber_thread(std::ref(subscriber));

	std::string cmd;
	std::cout << "quit or add\n";
	while (std::cin >> cmd) {
		if (cmd == "quit" || cmd == "exit" || cmd == "q" || cmd == "e") {
			subscriber.end = true;
			subscriber.cv.notify_all();
			break;
		} else if (cmd == "add" || cmd == "a") {
			std::unique_lock<std::mutex> main_lock(subscriber.mtx);
			std::string figure_type;

			for (int id = 0; id < vector_size; id++) {
				std::cout << "figure type\n";
				std::cin >> figure_type;
				if (figure_type == "triangle" || figure_type == "t") {
					std::pair<double, double> *vertices = new std::pair<double, double>[3];
					for (int i = 0; i < 3; i++) {
						std::cin >> vertices[i].first >> vertices[i].second;
					}
					subscriber.buffer.push_back(factory.FigureCreate(TRIANGLE, vertices, id));
				} else if (figure_type == "square" || figure_type == "s") {
					std::pair<double, double> *vertices = new std::pair<double, double>[4];
					for (int i = 0; i < 4; i++) {
						std::cin >> vertices[i].first >> vertices[i].second;
					}
					subscriber.buffer.push_back(factory.FigureCreate(SQUARE, vertices, id));
				} else if (figure_type == "rectangle" || figure_type == "r") {
					std::pair<double, double> *vertices = new std::pair<double, double>[4];
					for (int i = 0; i < 4; i++) {
						std::cin >> vertices[i].first >> vertices[i].second;
					}
					subscriber.buffer.push_back(factory.FigureCreate(RECTANGLE, vertices, id));
				}
			}
			std::cout << "out add\n";

			if (subscriber.buffer.size() == subscriber.buffer.capacity()) {
				subscriber.cv.notify_all();
				subscriber.cv.wait(main_lock, [&subscriber]() {
					return subscriber.success == true;
				});
				subscriber.success = false;
			}
			std::cout << "i'm here\n";
		}
	}

	subscriber_thread.join();




	return 0;
}
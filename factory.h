#ifndef FACTORY_H
#define FACTORY_H 1

#include "figures.h"

class Factory {
public:
	using Vertex = std::pair<double, double>;

	std::shared_ptr<Figure> FigureCreate(FigureType type) const {
		std::shared_ptr<Figure> res;
		if (type == TRIANGLE) {
			res = std::make_shared<Triangle>();
		} else if (type == SQUARE) {
			res = std::make_shared<Square>();
		} else if (type == RECTANGLE) {
			res = std::make_shared<Rectangle>();
		}

		return res;
	}

	std::shared_ptr<Figure> FigureCreate(FigureType type, Vertex *vertices, int id) const {
		std::shared_ptr<Figure> res;
		if (type == TRIANGLE) {
			res = std::make_shared<Triangle>(vertices[0], vertices[1], vertices[2], id);
		} else if (type == SQUARE) {
			res = std::make_shared<Square>(vertices[0], vertices[1], vertices[2], vertices[3], id);
		} else if (type == RECTANGLE) {
			res = std::make_shared<Rectangle>(vertices[0], vertices[1], vertices[2], vertices[3], id);
		}

		return res;
	}
};

#endif //FAACTORY_H
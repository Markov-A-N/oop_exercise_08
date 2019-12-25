#ifndef FIGURES_H
#define FIGURES_H 1

#include <iostream>
#include <fstream>
#include <utility>
#include <cmath>
#include <memory>


enum FigureType {
	TRIANGLE,
	SQUARE,
	RECTANGLE
};

class Figure {
public:
	virtual double Area() const = 0;
	virtual std::pair<double, double> Center() const = 0;
	virtual std::ostream &Print(std::ostream &out) const = 0;
	virtual void Serialize(std::ofstream &os) const = 0;
	virtual void Deserialize(std::ifstream &is) = 0;
	virtual int getId() const = 0;
	virtual ~Figure() = default;
};

namespace Geometry {
	using Vertex = std::pair<double, double>;
	double Product(const Vertex &v1, const Vertex &v2) {
		return v1.first * v2.first + v1.second * v2.second;
	}

	double PointDistance(const Vertex &v1, const Vertex &v2) {
		return sqrt(pow((v2.first - v1.first), 2) + 
			pow((v2.second - v1.second), 2));
	}

	class Vector {
		double x, y;
	public:
		Vector(double x_cord, double y_cord) : x{x_cord}, y{y_cord} {};

		Vector(Vertex &v1, Vertex &v2) : x{v2.first - v1.first},
						y{v2.second - v1.second} {};

		double operator*(const Vector &a) const {
			return (x * a.x) + (y * a.y);
		}

		Vector &operator=(const Vector &a) {
			x = a.x;
			y = a.y;

			return *this;
		}
		friend double LengthVector(const Vector &a);
		friend bool VectorsAreParallel(const Vector &a, const Vector &b);
	};

	double LengthVector(const Vertex &v1, const Vertex &v2) {
		return PointDistance(v1, v2);
	}

	double LengthVector(const Vector &a) {
		return sqrt(pow(a.x, 2) + pow(a.y, 2));
	}

	bool VectorsAreParallel(const Vector &a, const Vector &b) {
		return (a.x * b.y) - (a.y * b.x) == 0;
	}

	double Area(const Vertex *vertices, int n) {
		double res = 0;

		for (int i = 0; i < n - 1; i++) {
			res += (vertices[i].first * vertices[i + 1].second -
				vertices[i + 1].first * vertices[i].second);
		}
		res += (vertices[n - 1].first * vertices[0].second -
				vertices[0].first * vertices[n - 1].second);

		return 0.5 * std::abs(res);
	}

	Vertex Center(const Vertex *vertices, int n) {
		double x = 0, y = 0;

		for (int i = 0; i < n; i++) {
			x += vertices[i].first;
			y += vertices[i].second;
		}

		return std::make_pair(x / n, y / n);
	}
}

std::ostream &operator<<(std::ostream &out, std::pair<double, double> v) {
	out << "(" << v.first << ", " << v.second << ")";
	return out;
}

class Triangle : public Figure {
	using Vertex = std::pair<double, double>;
	int Id;
	Vertex *vertices;
public:
	Triangle() : Id{0}, vertices{new Vertex[3]} {
		for (int i = 0; i < 3; i++) {
			vertices[i] = std::make_pair(0, 0);
		}
	}

	Triangle(Vertex a, Vertex b, Vertex c, int id) : Id{id}, 
									vertices{new Vertex[3]} {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		double AB = Geometry::PointDistance(a, b), BC = 
		Geometry::PointDistance(b, c), AC = Geometry::PointDistance(a, c);
		if (AB >= BC + AC || BC >= AB + AC || AC >= AB + BC) {
			throw std::logic_error("Points must not be on the same line.");
		}
	}

	~Triangle() {
		delete [] vertices;
		vertices = nullptr;
	}

	double Area() const override {
		return Geometry::Area(vertices, 3);
	}

	Vertex Center() const override {
		return Geometry::Center(vertices, 3);
	}

	std::ostream &Print(std::ostream &out) const override{
		out << "Id: " << Id << "\n";
		out << "Figure: Triangle\n";
		out << "Coords:\n";
		for (int i = 0; i < 3; i++) {
			out << vertices[i] << "\n";
		}
		return out;
	}

	void Serialize(std::ofstream &os) const override{
		FigureType type = TRIANGLE;
		os.write((char *) &type, sizeof(type));
		os.write((char *) &Id, sizeof(Id));
		for (int i = 0; i < 3; i++) {
			os.write((char *) &(vertices[i].first), 
				sizeof(vertices[i].first));
			os.write((char *) &(vertices[i].second), 
				sizeof(vertices[i].second));
		}
	}

	void Deserialize(std::ifstream &is) override {
		is.read((char *) &Id, sizeof(Id));
		for (int i = 0; i < 3; i++) {
			is.read((char *) &(vertices[i].first), 
				sizeof(vertices[i].first));
			is.read((char *) &(vertices[i].second), 
				sizeof(vertices[i].second));
		}
	}	

	int getId() const override {
		return Id;
	}
};

class Square : public Figure {
	using Vertex = std::pair<double, double>;
	int Id;
	Vertex *vertices;
public:
	Square() : Id{0}, vertices{new Vertex[4]} {
		for (int i = 0; i < 4; i++) {
			vertices[i] = std::make_pair(0, 0);
		}
	}

	Square(Vertex a, Vertex b, Vertex c, Vertex d, int id) : 
						Id{id},	vertices{new Vertex[4]} {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		vertices[3] = d;
		Geometry::Vector AB{ a, b }, BC{ b, c }, CD{ c, d }, DA{ d, a };
		if (!Geometry::VectorsAreParallel(DA, BC)) {
			std::swap(vertices[0], vertices[1]);
			AB = { vertices[0], vertices[1] };
			BC = { vertices[1], vertices[2] };
			CD = { vertices[2], vertices[3] };
			DA = { vertices[3], vertices[0] };
		} 
		if (!Geometry::VectorsAreParallel(AB, CD)) {
			std::swap(vertices[1], vertices[2]);
			AB = { vertices[0], vertices[1] };
			BC = { vertices[1], vertices[2] };
			CD = { vertices[2], vertices[3] };
			DA = { vertices[3], vertices[0] };
		}

		if (AB * BC || BC * CD || CD * DA || DA * AB) {
			throw std::logic_error("The sides of the square should be perpendicular");
		} 
		if (LengthVector(AB) != LengthVector(BC) || LengthVector(BC) != LengthVector(CD) || LengthVector(CD) != LengthVector(DA) || LengthVector(DA) != LengthVector(AB)) {
			throw std::logic_error("The sides of the square should be equal");
		}
		if (!LengthVector(AB) || !LengthVector(BC) || !LengthVector(CD) || !LengthVector(DA)) {
			throw std::logic_error("The sides of the square must be greater than zero");
		}
	}

	~Square() {
		delete [] vertices;
		vertices = nullptr;
	}

	double Area() const override {
		return Geometry::Area(vertices, 4);
	}

	Vertex Center() const override {
		return Geometry::Center(vertices, 4);
	}

	std::ostream &Print(std::ostream &out) const override{
		out << "Id: " << Id << "\n";
		out << "Figure: Square\n";
		out << "Coords:\n";
		for (int i = 0; i < 4; i++) {
			out << vertices[i] << "\n";
		}
		return out;
	}

	void Serialize(std::ofstream &os) const override{
		FigureType type = SQUARE;
		os.write((char *) &type, sizeof(type));
		os.write((char *) &Id, sizeof(Id));
		for (int i = 0; i < 4; i++) {
			os.write((char *) &(vertices[i].first), 
				sizeof(vertices[i].first));
			os.write((char *) &(vertices[i].second), 
				sizeof(vertices[i].second));
		}
	}

	void Deserialize(std::ifstream &is) override {
		is.read((char *) &Id, sizeof(Id));
		for (int i = 0; i < 4; i++) {
			is.read((char *) &(vertices[i].first), 
				sizeof(vertices[i].first));
			is.read((char *) &(vertices[i].second), 
				sizeof(vertices[i].second));
		}
	}	

	int getId() const override {
		return Id;
	}
};

class Rectangle : public Figure {
	using Vertex = std::pair<double, double>;
	int Id;
	Vertex *vertices;
public:
	Rectangle() : Id{0}, vertices{new Vertex[4]} {
		for (int i = 0; i < 4; i++) {
			vertices[i] = std::make_pair(0, 0);
		}
	}

	Rectangle(Vertex a, Vertex b, Vertex c, Vertex d, int id) : 
						Id{id},	vertices{new Vertex[4]} {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		vertices[3] = d;
		Geometry::Vector AB{ a, b }, BC{ b, c }, CD{ c, d }, DA{ d, a };
		if (!Geometry::VectorsAreParallel(DA, BC)) {
			std::swap(vertices[0], vertices[1]);
			AB = { vertices[0], vertices[1] };
			BC = { vertices[1], vertices[2] };
			CD = { vertices[2], vertices[3] };
			DA = { vertices[3], vertices[0] };
		} 
		if (!Geometry::VectorsAreParallel(AB, CD)) {
			std::swap(vertices[1], vertices[2]);
			AB = { vertices[0], vertices[1] };
			BC = { vertices[1], vertices[2] };
			CD = { vertices[2], vertices[3] };
			DA = { vertices[3], vertices[0] };
		}

		if (AB * BC || BC * CD || CD * DA || DA * AB) {
			throw std::logic_error("The sides of the square should be perpendicular");
		} 
		if (!LengthVector(AB) || !LengthVector(BC) || !LengthVector(CD) || !LengthVector(DA)) {
			throw std::logic_error("The sides of the square must be greater than zero");
		}
	}

	~Rectangle() {
		delete [] vertices;
		vertices = nullptr;
	}

	double Area() const override {
		return Geometry::Area(vertices, 4);
	}

	Vertex Center() const override {
		return Geometry::Center(vertices, 4);
	}

	std::ostream &Print(std::ostream &out) const override{
		out << "Id: " << Id << "\n";
		out << "Figure: Rectangle\n";
		out << "Coords:\n";
		for (int i = 0; i < 4; i++) {
			out << vertices[i] << "\n";
		}
		return out;
	}

	void Serialize(std::ofstream &os) const override{
		FigureType type = RECTANGLE;
		os.write((char *) &type, sizeof(type));
		os.write((char *) &Id, sizeof(Id));
		for (int i = 0; i < 4; i++) {
			os.write((char *) &(vertices[i].first), 
				sizeof(vertices[i].first));
			os.write((char *) &(vertices[i].second), 
				sizeof(vertices[i].second));
		}
	}

	void Deserialize(std::ifstream &is) override {
		is.read((char *) &Id, sizeof(Id));
		for (int i = 0; i < 4; i++) {
			is.read((char *) &(vertices[i].first), 
				sizeof(vertices[i].first));
			is.read((char *) &(vertices[i].second), 
				sizeof(vertices[i].second));
		}
	}	

	int getId() const override {
		return Id;
	}
};

#endif // FIGURES_H
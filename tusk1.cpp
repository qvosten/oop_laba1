#include <iostream>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>


class Complex {
private:
	double real_val; 
	double imag_val; 

public:
	Complex(double real = 0.0, double imag = 0.0) : real_val(real), imag_val(imag) {}


	double real() const {
		return real_val;
	}
	double imag() const {
		return imag_val;
	}

	Complex operator+(const Complex& other) const {
		return Complex(real_val + other.real_val, imag_val + other.imag_val);
	}

	Complex operator-(const Complex& other) const {
		return Complex(real_val - other.real_val, imag_val - other.imag_val);
	}

	Complex operator*(const Complex& other) const {
		return Complex(
			real_val * other.real_val - imag_val * other.imag_val,
			real_val * other.imag_val + imag_val * other.real_val
		);
	}

	Complex operator/(const Complex& other) const {
		double denominator = other.real_val * other.real_val + other.imag_val * other.imag_val;
		if (denominator == 0.0) {
			throw std::runtime_error("Division by zero in complex numbers.");
		}
		return Complex(
			(real_val * other.real_val + imag_val * other.imag_val) / denominator,
			(imag_val * other.real_val - real_val * other.imag_val) / denominator
		);
	}

	bool operator==(const Complex& other) const {
		return real_val == other.real_val && imag_val == other.imag_val;
	}

	bool operator!=(const Complex& other) const {
		return !(*this == other);
	}

	double magnitude() const {
		return std::sqrt(real_val * real_val + imag_val * imag_val);
	}

	Complex sqrt() const {
		double r = magnitude();
		double realpart = std::sqrt((r + real_val) / 2.0);
		double imagpart = std::sqrt((r - real_val) / 2.0);
		if (imag_val < 0) {
			imagpart = -imagpart;
		}
		return Complex(realpart, imagpart);
	}
};

class Equation {
public:
	virtual ~Equation() = default;
	virtual Complex evaluate(const Complex& x) const = 0;
	virtual std::vector<Complex> solve() const = 0;
	Complex operator()(const Complex& x) const {
		return evaluate(x);
	}
};

class LinearEquation : public Equation {
private:
	Complex a;
	Complex b;
public:
	LinearEquation(const Complex& a, const Complex& b) : a(a), b(b) {
		if (a == Complex(0.0, 0.0)) {
			throw std::invalid_argument("Linear equation coefficient 'a' cannot be zero.");
		}
	}
	Complex evaluate(const Complex& x) const override {
		return a * x + b;
	}

	std::vector<Complex> solve() const override {
		std::vector<Complex> roots;
		Complex zero(0.0, 0.0);
		Complex root = (zero - b) / a;
		roots.push_back(root);
		return roots;
	}
	
};

class QuadraticEquation : public Equation {
private:
	Complex a;
	Complex b;
	Complex c;
public:
	QuadraticEquation(const Complex& a, const Complex& b, const Complex& c) : a(a), b(b), c(c) {
		if (a == Complex(0.0, 0.0)) {
			throw std::invalid_argument("Quadratic equation coefficient 'a' cannot be zero.");
		}
	}
	Complex evaluate(const Complex& x) const override {
		Complex x_squared = x * x;
		return a * x_squared + b * x + c;
	}
	std::vector<Complex> solve() const override {
		std::vector<Complex> roots;
		Complex two(2.0, 0.0);
		Complex four(4.0, 0.0);

		Complex zero(0.0, 0.0);
		Complex minus_b = zero - b;
		Complex denominator = two * a;

		Complex discriminant = (b * b) - (four * a * c);
		Complex sqrt_disc = discriminant.sqrt();

		Complex root1 = (minus_b + sqrt_disc) / denominator;
		Complex root2 = (minus_b - sqrt_disc) / denominator;

		roots.push_back(root1);
		roots.push_back(root2);

		return roots;
	}
};
class EquationCollection {
private:
	std::vector<std::unique_ptr<Equation>> equations;

public:
	void add_equation(std::unique_ptr<Equation> eq) {
		if (eq) {
			equations.push_back(std::move(eq));
		}
	}
	std::vector<std::vector<Complex>> solve_all() const {
		std::vector<std::vector<Complex>> all_roots;
		for (const auto& eq : equations) {
			all_roots.push_back(eq->solve());
		}
		return all_roots;
	}
	std::vector<Complex> evaluate_all(const Complex& x) const {
		std::vector<Complex> results;
		for (const auto& eq : equations) {
			results.push_back((*eq)(x));
		}
		return results;
	}
	std::vector<size_t> find_equations_with_root(const Complex& x) const {
		std::vector<size_t> matching_indices;
		const double eps = 1e-5; 

		for (size_t i = 0; i < equations.size(); ++i) {
			
			Complex val = (*equations[i])(x);

			if (val.magnitude() < eps) {
				matching_indices.push_back(i);
			}
		}
		return matching_indices;
	}

	size_t size() const {
		return equations.size();
	}
};

void print_complex(const Complex& z) {
	std::cout << "(" << z.real() << " + " << z.imag() << "i)";
}

int main() {
	std::setlocale(LC_ALL, "Russian");

	std::cout << "=== ЗАПУСК АВТОМАТИЧЕСКИХ ТЕСТОВ ПО ЛАБОРАТОРНОЙ РАБОТЕ ===" << std::endl;
	std::cout << "--------------------------------------------------------------" << std::endl;

	try {
		// ТЕСТ 1: Проверка комплексных чисел и перегрузки операторов
		std::cout << "[TEST 1] Проверка класса Complex и операций..." << std::endl;
		Complex z1(3.0, 4.0);
		Complex z2(1.0, 2.0);

		Complex z_sum = z1 + z2;
		Complex z_mul = z1 * z2;

		if (z_sum.real() == 4.0 && z_sum.imag() == 6.0) {
			std::cout << " > Сложение Complex: УСПЕШНО" << std::endl;
		}
		else {
			std::cout << " > Сложение Complex: ОШИБКА" << std::endl;
		}

		// Проверка модуля (для 3 + 4i модуль должен быть равен 5)
		if (std::abs(z1.magnitude() - 5.0) < 1e-9) {
			std::cout << " > Вычисление модуля (magnitude): УСПЕШНО" << std::endl;
		}
		else {
			std::cout << " > Вычисление модуля (magnitude): ОШИБКА" << std::endl;
		}

		// Проверка деления на ноль в комплексных числах (должно вызвать исключение)
		bool division_exception_caught = false;
		try {
			Complex zero_complex(0.0, 0.0);
			Complex res = z1 / zero_complex;
		}
		catch (const std::runtime_error& e) {
			division_exception_caught = true;
		}
		if (division_exception_caught) {
			std::cout << " > Обработка деления на ноль в Complex: УСПЕШНО" << std::endl;
		}
		else {
			std::cout << " > Обработка деления на ноль в Complex: ОШИБКА" << std::endl;
		}
		std::cout << "--------------------------------------------------------------" << std::endl;

		// ТЕСТ 2: Проверка ограничений конструкторов (коэффициент a != 0)[cite: 7]
		std::cout << "[TEST 2] Проверка валидации конструкторов (a = 0)..." << std::endl;
		bool linear_invalid_caught = false;
		try {
			LinearEquation bad_linear(Complex(0.0, 0.0), Complex(5.0, 0.0));
		}
		catch (const std::invalid_argument&) {
			linear_invalid_caught = true;
		}

		bool quad_invalid_caught = false;
		try {
			QuadraticEquation bad_quad(Complex(0.0, 0.0), Complex(2.0, 0.0), Complex(1.0, 0.0));
		}
		catch (const std::invalid_argument&) {
			quad_invalid_caught = true;
		}

		if (linear_invalid_caught && quad_invalid_caught) {
			std::cout << " > Блокировка некорректных коэффициентов (a = 0): УСПЕШНО" << std::endl;
		}
		else {
			std::cout << " > Блокировка некорректных коэффициентов (a = 0): ОШИБКА" << std::endl;
		}
		std::cout << "--------------------------------------------------------------" << std::endl;

		// ТЕСТ 3: Проверка работы LinearEquation и QuadraticEquation через функтор и solve
		std::cout << "[TEST 3] Решение уравнений и использование функтора..." << std::endl;

		// Линейное уравнение: 2x + 4 = 0 => x = -2
		LinearEquation lin_eq(Complex(2.0, 0.0), Complex(4.0, 0.0));
		auto lin_roots = lin_eq.solve();
		std::cout << " > Корень линейного уравнения 2x + 4 = 0: " << lin_roots[0].real() << std::endl;

		// Проверка функтора evaluate для x = 2: f(2) = 2*2 + 4 = 8
		Complex val = lin_eq(Complex(2.0, 0.0));
		if (std::abs(val.real() - 8.0) < 1e-9) {
			std::cout << " > Использование функтора LinearEquation: УСПЕШНО" << std::endl;
		}
		else {
			std::cout << " > Использование функтора LinearEquation: ОШИБКА" << std::endl;
		}

		// Квадратичное уравнение: x^2 - 4 = 0 => корни 2 и -2
		QuadraticEquation quad_eq(Complex(1.0, 0.0), Complex(0.0, 0.0), Complex(-4.0, 0.0));
		auto quad_roots = quad_eq.solve();
		std::cout << " > Корни квадратичного уравнения x^2 - 4 = 0 найдены (всего: " << quad_roots.size() << ")" << std::endl;
		std::cout << "--------------------------------------------------------------" << std::endl;

		// ТЕСТ 4: Работа с коллекцией уравнений (`EquationCollection`)[cite: 8, 9]
		std::cout << "[TEST 4] Проверка коллекции уравнений (EquationCollection)..." << std::endl;
		EquationCollection collection;

		// Добавляем уравнения в общую коллекцию через unique_ptr
		collection.add_equation(std::make_unique<LinearEquation>(Complex(1.0, 0.0), Complex(-3.0, 0.0))); // x - 3 = 0 (корень x = 3)
		collection.add_equation(std::make_unique<QuadraticEquation>(Complex(1.0, 0.0), Complex(0.0, 0.0), Complex(-9.0, 0.0))); // x^2 - 9 = 0 (корни 3 и -3)

		std::cout << " > Всего уравнений в коллекции: " << collection.size() << std::endl;

		// Решаем все уравнения в коллекции[cite: 9]
		auto all_solved = collection.solve_all();
		std::cout << " > Решение всех уравнений в коллекции выполнено успешно." << std::endl;

		// Проверяем поиск уравнений, для которых заданное число x = 3 является корнем (f(3) == 0)[cite: 8, 9]
		Complex test_root(3.0, 0.0);
		auto matching_eqs = collection.find_equations_with_root(test_root);

		std::cout << " > Найдено уравнений с корнем x = 3: " << matching_eqs.size() << std::endl;
		if (matching_eqs.size() == 2) {
			std::cout << " > Поиск уравнений по корню через коллекцию: УСПЕШНО" << std::endl;
		}
		else {
			std::cout << " > Поиск уравнений по корню через коллекцию: ОШИБКА" << std::endl;
		}

		std::cout << "--------------------------------------------------------------" << std::endl;
		std::cout << "ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ!" << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "КРИТИЧЕСКАЯ ОШИБКА В ТЕСТАХ: " << e.what() << std::endl;
	}

	return 0;
}

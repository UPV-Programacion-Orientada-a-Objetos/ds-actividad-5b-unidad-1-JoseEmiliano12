#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;

// Clase base abstracta define lo comun para cualquier tipo de matriz
template <typename T>
class MatrizBase {
protected:
    int filas_;
    int columnas_;

public:
    MatrizBase() : filas_(0), columnas_(0) {}
    MatrizBase(int f, int c) : filas_(f), columnas_(c) {}
    virtual ~MatrizBase() {}

    int filas() const { return filas_; }
    int columnas() const { return columnas_; }

    // Metodos de clase hijas
    virtual void cargarValores() = 0;
    virtual T get(int i, int j) const = 0;
    virtual void set(int i, int j, T v) = 0;
    virtual void imprimir() const = 0;
    virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;

    // Sobrecarga del operador + para usar la suma
    MatrizBase<T>* operator+(const MatrizBase<T>& otra) const {
        return this->sumar(otra);
    }

protected:
    static void verificarDimensiones(const MatrizBase<T>& a, const MatrizBase<T>& b) {
        if (a.filas() != b.filas() || a.columnas() != b.columnas()) {
            throw runtime_error("Las dimensiones no coinciden para la suma");
        }
    }
};

// Matriz dinamica con punteros con memoria dinamica
template <typename T>
class MatrizDinamica : public MatrizBase<T> {
    T** datos;

    void reservar(int f, int c) {
        this->filas_ = f;
        this->columnas_ = c;
        datos = new T*[f];
        for (int i = 0; i < f; i++) {
            datos[i] = new T[c];
            for (int j = 0; j < c; j++) datos[i][j] = 0;
        }
    }

    void liberar() {
        if (datos) {
            for (int i = 0; i < this->filas_; i++) delete[] datos[i];
            delete[] datos;
        }
        datos = nullptr;
    }

public:
    MatrizDinamica() : datos(nullptr) {}
    MatrizDinamica(int f, int c) { reservar(f, c); }
    ~MatrizDinamica() { liberar(); }

    // Constructor
    MatrizDinamica(const MatrizDinamica& otra) {
        reservar(otra.filas_, otra.columnas_);
        for (int i = 0; i < this->filas_; i++)
            for (int j = 0; j < this->columnas_; j++)
                datos[i][j] = otra.datos[i][j];
    }

    // Operador de asignacion
    MatrizDinamica& operator=(const MatrizDinamica& otra) {
        if (this == &otra) return *this;
        liberar();
        reservar(otra.filas_, otra.columnas_);
        for (int i = 0; i < this->filas_; i++)
            for (int j = 0; j < this->columnas_; j++)
                datos[i][j] = otra.datos[i][j];
        return *this;
    }

    // Constructores de movimiento
    MatrizDinamica(MatrizDinamica&& otra) noexcept {
        this->filas_ = otra.filas_;
        this->columnas_ = otra.columnas_;
        datos = otra.datos;
        otra.datos = nullptr;
    }

    MatrizDinamica& operator=(MatrizDinamica&& otra) noexcept {
        if (this == &otra) return *this;
        liberar();
        this->filas_ = otra.filas_;
        this->columnas_ = otra.columnas_;
        datos = otra.datos;
        otra.datos = nullptr;
        return *this;
    }

    void cargarValores() override {
        for (int i = 0; i < this->filas_; i++)
            for (int j = 0; j < this->columnas_; j++)
                cin >> datos[i][j];
    }

    T get(int i, int j) const override { return datos[i][j]; }
    void set(int i, int j, T v) override { datos[i][j] = v; }

    void imprimir() const override {
        for (int i = 0; i < this->filas_; i++) {
            cout << "| ";
            for (int j = 0; j < this->columnas_; j++) {
                cout << fixed << setprecision(1) << datos[i][j];
                if (j + 1 < this->columnas_) cout << " | ";
            }
            cout << " |\n";
        }
    }

    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        MatrizBase<T>::verificarDimensiones(*this, otra);
        MatrizDinamica<T>* resultado = new MatrizDinamica<T>(this->filas_, this->columnas_);
        for (int i = 0; i < this->filas_; i++)
            for (int j = 0; j < this->columnas_; j++)
                resultado->set(i, j, this->get(i, j) + otra.get(i, j));
        return resultado;
    }
};

// Matriz estatica con tamaño fijo 
template <typename T, int F, int C>
class MatrizEstatica : public MatrizBase<T> {
    T datos[F][C];

public:
    MatrizEstatica() : MatrizBase<T>(F, C) {
        for (int i = 0; i < F; i++)
            for (int j = 0; j < C; j++)
                datos[i][j] = 0;
    }

    void cargarValores() override {
        for (int i = 0; i < F; i++)
            for (int j = 0; j < C; j++)
                cin >> datos[i][j];
    }

    T get(int i, int j) const override { return datos[i][j]; }
    void set(int i, int j, T v) override { datos[i][j] = v; }

    void imprimir() const override {
        for (int i = 0; i < F; i++) {
            cout << "| ";
            for (int j = 0; j < C; j++) {
                cout << fixed << setprecision(1) << datos[i][j];
                if (j + 1 < C) cout << " | ";
            }
            cout << " |\n";
        }
    }

    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        MatrizBase<T>::verificarDimensiones(*this, otra);
        MatrizEstatica<T, F, C>* r = new MatrizEstatica<T, F, C>();
        for (int i = 0; i < F; i++)
            for (int j = 0; j < C; j++)
                r->set(i, j, this->get(i, j) + otra.get(i, j));
        return r;
    }
};

int main() {
    cout << "--- Sistema Generico de Algebra Lineal ---\n\n";

    MatrizBase<float>* A = new MatrizDinamica<float>(3, 2);
    A->set(0,0,1.5f); A->set(0,1,2.0f);
    A->set(1,0,0.0f); A->set(1,1,1.0f);
    A->set(2,0,4.5f); A->set(2,1,3.0f);

    cout << "Matriz A:\n";
    A->imprimir();
    cout << "\n";

    MatrizBase<float>* B = new MatrizEstatica<float,3,2>();
    B->set(0,0,0.5f); B->set(0,1,1.0f);
    B->set(1,0,2.0f); B->set(1,1,3.0f);
    B->set(2,0,1.0f); B->set(2,1,1.0f);

    cout << "Matriz B:\n";
    B->imprimir();
    cout << "\n";

    cout << "Suma de A + B:\n";
    MatrizBase<float>* C = (*A) + (*B);
    C->imprimir();
    cout << "\n";

    delete C;
    delete A;
    delete B;

    MatrizBase<int>* E1 = new MatrizEstatica<int,2,2>();
    MatrizBase<int>* E2 = new MatrizEstatica<int,2,2>();

    E1->set(0,0,1); E1->set(0,1,2);
    E1->set(1,0,3); E1->set(1,1,4);
    E2->set(0,0,5); E2->set(0,1,6);
    E2->set(1,0,7); E2->set(1,1,8);

    cout << "Matriz E1:\n";
    E1->imprimir();
    cout << "\nMatriz E2:\n";
    E2->imprimir();
    cout << "\nResultado E1 + E2:\n";

    MatrizBase<int>* R = (*E1) + (*E2);
    R->imprimir();
    cout << "\n";

    delete R;
    delete E1;
    delete E2;

    cout << "Programa finalizado.\n";
    return 0;
}

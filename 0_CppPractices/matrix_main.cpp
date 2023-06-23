#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <map>
#include <vector>
#include <utility>
using namespace std;

template <class T>
class Matrix {
public:
    Matrix() {
        rows = 0;
        cols = 0;
        data = nullptr;
    }

    // constructor with initilizer list
    Matrix(int a, int b) {
        rows = a;
        cols = b;
        data = new T[rows * cols];
    }

    Matrix(const Matrix& other) {
        rows = other.rows;
        cols = other.cols;
        data = new T[rows * cols];

        for (int i = 0; i < rows * cols; i++)
            data[i] = other.data[i];
    }//д����+�ŵ�ʱ����û������Matrix C=A+B����������ѧ�������ͬѧ��˵�ǿ���������������캯��

    Matrix(Matrix&& other) noexcept
        : rows(other.rows), cols(other.cols), data(other.data) {
        other.data = nullptr;
    }//�����ͬѧ��ӵģ���û�п�����

    ~Matrix() {
        delete[] data;
        data = nullptr;
        rows = 0;
        cols = 0;
    }

    int nrow() const {return rows;}
    int ncol() const {return cols;}

    // operator overloding
    T& operator()(int row, int col) { 
        return this->data[cols * row + col]; 
    }
    T& operator[](int i) { 
        return data[i];
    }

    Matrix col(int c) {
        Matrix A(rows, 1);
        for (int i = 0; i < rows; i++) {
            A(i, 0) = this->data[i * cols + c];
        }
        return A;
    }
    Matrix row(int r) {
        Matrix A(1, cols);
        for (int i = 0; i < cols; i++) {
            A(0, i) = this->data[r * cols + i];
        }
        return A;
    }
    Matrix submat(int r1, int r2, int c1, int c2) const {
        assert(r2 >= r1 && c2 >= c1);//������Ҫ��Ͳ���
        Matrix A(r2 - r1 + 1, c2 - c1 + 1);
        for (int i = r1; i <= r2; i++) {
            for (int j = c1; j <= c2; j++) {
                A(i - r1, j - c1) = data[i * cols + j];
            }
        }
        return A;
    }

    Matrix& operator= (const Matrix& rhs) {
        if (this == &rhs) {
            return *this;
        }
        cols = rhs.cols;
        rows = rhs.rows;
        double* pdata = new T[cols * rows];
        for (int i = 0; i < cols * rows; i++) {
            pdata[i] = rhs.data[i];
        }
        delete[] data;
        data = pdata;
        return *this;
    }

    Matrix operator+ (const Matrix& rhs) {
        assert(cols == rhs.cols && rows == rhs.rows);//�к��б�����ͬ����Ȼ��ô���
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] + rhs.data[i * cols + j];
            }
        }
        return A;
    }
    Matrix operator- (const Matrix& rhs) {
        assert(cols == rhs.cols && rows == rhs.rows);//�к��б�����ͬ����Ȼ��ô���
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] - rhs.data[i * cols + j];
            }
        }
        return A;
    }
    Matrix operator* (const Matrix& rhs) {
        assert(cols == rhs.rows);
        Matrix A(rows, rhs.cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rhs.cols; j++) {
                A(i, j) = 0;
                for (int k = 0; k < cols; k++) {
                    A(i, j) += data[i * cols + k] * rhs.data[k * cols + j];
                }
            }
        }
        return A;
    }
    Matrix operator/ (const Matrix& rhs) {//�����������Ϊ��ӦԪ�����
        assert(cols == rhs.cols && rows == rhs.rows);
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] / rhs.data[i * cols + j];
            }
        }
        return A;
    }

    Matrix operator+= (const Matrix& rhs) {
        assert(cols == rhs.cols && rows == rhs.rows);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                data[i * cols + j] = data[i * cols + j] + rhs.data[i * cols + j];
            }
        }
        return *this;
    }
    Matrix operator-= (const Matrix& rhs) {
        assert(cols == rhs.cols && rows == rhs.rows);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                data[i * cols + j] = data[i * cols + j] - rhs.data[i * cols + j];
            }
        }
        return *this;
    }
    Matrix operator*= (const Matrix& rhs) {
        assert(cols == rhs.rows);
        Matrix A(rows, rhs.cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rhs.cols; j++) {
                A(i, j) = 0;
                for (int k = 0; k < cols; k++) {
                    A(i, j) += data[i * cols + k] * rhs.data[k * cols + j];
                }
            }
        }
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rhs.cols; j++) {
                data[i * cols + j] = A(i, j);
            }
        }
        return *this;
    }
    Matrix operator/= (const Matrix& rhs) {
        assert(cols == rhs.cols && rows == rhs.rows);
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] / rhs.data[i * cols + j];
            }
        }
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                data[i * cols + j] = A(i, j);
            }
        }
        return *this;
    }

    Matrix operator+ (T v) {
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] + v;
            }
        }
        return A;
    }
    Matrix operator- (T v) {
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] - v;
            }
        }
        return A;
    }
    Matrix operator* (T v) {
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] * v;
            }
        }
        return A;
    }
    Matrix operator/ (T v) {
        assert(v != 0);//�и���������
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] / v;
            }
        }
        return A;
    }

    Matrix operator+= (T v) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                data[i * cols + j] += v;
            }
        }
        return *this;
    }
    Matrix operator-= (T v) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                data[i * cols + j] -= v;
            }
        }
        return *this;
    }
    Matrix operator*= (T v) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                data[i * cols + j] *= v;
            }
        }
        return *this;
    }
    Matrix operator/= (T v) {
        assert(v != 0);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                data[i * cols + j] /= v;
            }
        }
        return *this;
    }

    void print () const {
        printf("this matrix has size (%d x %d)\n", rows, cols);
        printf("the entries are:\n");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                std::cout << data[i * cols + j]<<" ";
            }
            std::cout << std::endl;
        }
        cout << endl;
    }

private:
int rows, cols;
T *data;
};

// BONUS: write a sparse matrix class
template<typename R>
class SparseMatrix {
public:
    SparseMatrix() {
        rows = 0;
        cols = 0;
    }
    SparseMatrix(const SparseMatrix& other) {
        data = other.data;
        rows = other.rows;
        cols = other.cols;
    }
    SparseMatrix(int a, int b) {
        rows = a;
        cols = b;
    }

public:
    //operator overloading
    //������ظ���cpu�������ˣ������������綼�����������
    SparseMatrix& operator= (const SparseMatrix& rhs) {
        if (this == &rhs) {
            return *this;
        }
        cols = rhs.cols;
        rows = rhs.rows;
        data = rhs.data;
        return *this;
    }
    SparseMatrix operator+ (const SparseMatrix& rhs) {
        assert(cols == rhs.cols && rows == rhs.rows);
        SparseMatrix A(rows, cols);
        R a, b;
        //��rhs�ķ���Ԫ�ؼӵ�A��
        for (auto itr = rhs.data.begin(); itr != rhs.data.end(); itr++) {
            a = itr->first.first; b = itr->first.second;
            A.data[make_pair(a,b)] += itr->second;
        }
        //��*this�ķ���Ԫ�ؼӵ�A��
        for (auto itr = data.begin(); itr != data.end(); itr++) {
            a = itr->first.first; b = itr->first.second;
            A.data[make_pair(a, b)] += itr->second;
        }
        return A;
    }
      
    

    SparseMatrix operator* (const SparseMatrix& rhs) {
        //����˷��ĸ��Ӷȱ������˷�С��һ������
        assert(cols == rhs.rows);
        SparseMatrix A(rows, rhs.cols);
        for (auto itr = data.begin(); itr != data.end(); itr++) {//�ȱ���*this�еķ���Ԫ��
            for (int i = 0; i < rhs.cols; i++) {
                auto itr1 = rhs.data.find(make_pair(itr->first.second, i));//�жϵ�����ָ��ķ���Ԫ���ھ���˷����ܳ˵���λ������û�з����Ԫ��
                if (itr1 != rhs.data.end()) {//�����
                    A.data[make_pair(itr->first.first, i)] += itr->second * itr1->second;//��ô����˲��ӵ�A�Ķ�Ӧλ��
                }
            }
        }
        return A;
    }

    //�������ϡ�����
    SparseMatrix create(int n) {
        SparseMatrix X(n, n);
        
        R element = 0;
        for (int i = 0; i < n; i++) {
            element = rand();
            X.data.emplace(make_pair(i, i), element);
        }
        for (int i = 0; i < n - 1; i++) {
            X.data.emplace(make_pair(i, i + 1), 1);
        }
        return X;
    }

    void print() const {
        printf("this matrix has size (%d x %d)\n", rows, cols);
        printf("the entries are:\n");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                auto itr = data.find(make_pair(i, j));
                cout << (itr == data.end() ? 0 : itr->second) <<" ";
                    //����ҵ��ˣ��������ֵ��û�ҵ���������ָ��end���Ǿ����0
            }
            cout << endl;
        }
        cout << endl;
    }
private:
    int rows, cols;
    map<pair<int, int>, R> data;
};

int main() {
    int p=3, q=3, r=3;//��ΪA��B������
    Matrix<double> A(p, q);
    Matrix<double> B(q, r);
    srand(time(nullptr));
    // todo 6: fill A anb B with random numbers
    
    for(int i=0; i<A.nrow(); i++)
        for(int j=0; j<A.ncol(); j++)
            A(i, j) = rand();
    for (int i = 0; i < B.nrow(); i++)
        for (int j = 0; j < B.ncol(); j++)
            B(i, j) = rand();
    cout << "A=" << endl;
    A.print();
    cout << "B=" << endl;
    B.print();

    Matrix<double> C;
    
    //test operator
    cout << "A+B=" << endl;
    C = A + B;//����仰����������Сʱ
    C.print();
    cout << "A-B=" << endl;
    C = A - B;
    C.print();
    cout << "A*B=" << endl;
    C = A * B;
    C.print();
    cout << "A/B=" << endl;
    C = A / B;
    C.print();

    cout << "B+=A" << endl;
    C = B;
    C += A;
    C.print();
    cout << "B-=A" << endl;
    C = B;
    C -= A;
    C.print();
    cout << "B*=A" << endl;
    C = B;
    C *= A;
    C.print();
    cout << "B/=A" << endl;
    C = B;
    C /= A;
    C.print();

    cout << "C=A+1=" << endl;
    C = A - 1;
    C.print();//����Ӽ�����֤
    cout << "C-=1" << endl;
    C += 1;
    C.print();
    
    //test template
    cout << "test template int��" << endl;
    Matrix<int> D(3,3),E(3,3);
    for (int i = 0; i < D.nrow(); i++)
        for (int j = 0; j < D.ncol(); j++)
            D(i, j) = rand();
    for (int i = 0; i <E.nrow(); i++)
        for (int j = 0; j < E.ncol(); j++)
            E(i, j) = rand();
    cout << "D=" << endl;
    D.print();
    cout << "E=" << endl;
    E.print();
    cout << "D/E=" << endl;
    D /= E;
    D.print();


    cout << "�������ϡ�������" << endl;

    //����ֱ��ȡϡ�����Ϊ�ԽǾ���+N��Jordan��׼��������Ǹ�N��
    SparseMatrix<double> X = X.create(10);//��֪�����createд�ĺ����ʣ�����������д����
    SparseMatrix<double> Y = Y.create(10);
    SparseMatrix<double> Z;
    cout << "X=" << endl;
    X.print(); 
    cout << "Y=" << endl;
    Y.print();
    cout << "X+Y=" << endl;
    Z = X + Y;
    Z.print();
    cout << "X*Y=" << endl;
    Z = X * Y;
    Z.print();
    
    // todo 7: benchmark with runtime, using std::chrono
    // https://en.cppreference.com/w/cpp/chrono


    //Matrix C = A*B;
    //A.print();
    //B.print();
    //C.print();

    // todo 8: use Eigen and compare

    return 0;
}

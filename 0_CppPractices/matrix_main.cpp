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
    }//写重载+号的时候发现没法进行Matrix C=A+B操作，问了学计算机的同学，说是可以添加这两个构造函数

    Matrix(Matrix&& other) noexcept
        : rows(other.rows), cols(other.cols), data(other.data) {
        other.data = nullptr;
    }//这个是同学添加的，并没有看懂（

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
        assert(r2 >= r1 && c2 >= c1);//不满足要求就不行
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
        assert(cols == rhs.cols && rows == rhs.rows);//行和列必须相同，不然怎么相加
        Matrix A(rows, cols);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                A(i, j) = data[i * cols + j] + rhs.data[i * cols + j];
            }
        }
        return A;
    }
    Matrix operator- (const Matrix& rhs) {
        assert(cols == rhs.cols && rows == rhs.rows);//行和列必须相同，不然怎么相减
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
    Matrix operator/ (const Matrix& rhs) {//这里除法定义为对应元素相除
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
        assert(v != 0);//有个限制条件
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
    //这个重载给我cpu都干烧了，周日整个下午都在做这个东西
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
        //将rhs的非零元素加到A中
        for (auto itr = rhs.data.begin(); itr != rhs.data.end(); itr++) {
            a = itr->first.first; b = itr->first.second;
            A.data[make_pair(a,b)] += itr->second;
        }
        //将*this的非零元素加到A中
        for (auto itr = data.begin(); itr != data.end(); itr++) {
            a = itr->first.first; b = itr->first.second;
            A.data[make_pair(a, b)] += itr->second;
        }
        return A;
    }
      
    

    SparseMatrix operator* (const SparseMatrix& rhs) {
        //这个乘法的复杂度比正常乘法小了一个量级
        assert(cols == rhs.rows);
        SparseMatrix A(rows, rhs.cols);
        for (auto itr = data.begin(); itr != data.end(); itr++) {//先遍历*this中的非零元素
            for (int i = 0; i < rhs.cols; i++) {
                auto itr1 = rhs.data.find(make_pair(itr->first.second, i));//判断迭代器指向的非零元素在矩阵乘法中能乘到的位置中有没有非零的元素
                if (itr1 != rhs.data.end()) {//如果有
                    A.data[make_pair(itr->first.first, i)] += itr->second * itr1->second;//那么就相乘并加到A的对应位置
                }
            }
        }
        return A;
    }

    //随机构造稀疏矩阵
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
                    //如果找到了，就输出数值，没找到迭代器就指向end，那就输出0
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
    int p=3, q=3, r=3;//作为A和B的行列
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
    C = A + B;//就这句话搞了我两个小时
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
    C.print();//其余加减不验证
    cout << "C-=1" << endl;
    C += 1;
    C.print();
    
    //test template
    cout << "test template int型" << endl;
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


    cout << "下面测试稀疏矩阵类" << endl;

    //这里直接取稀疏矩阵为对角矩阵+N（Jordan标准型里面的那个N）
    SparseMatrix<double> X = X.create(10);//我知道这个create写的很劣质，但是这样好写啊（
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

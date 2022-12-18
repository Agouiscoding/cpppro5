#ifndef _MAT_H
#define _MAT_H

#include <iostream>
#include <cstring>
#include <sstream>
#include <cstddef>
#include <omp.h>
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math")

template <class T>
class Matrix
{
private:
    size_t row;
    size_t col;
    size_t channel;
    T *data;
    size_t ROIBeginRow;
    size_t ROIBeginCol;
    size_t ROIRow;
    size_t ROICol;
    size_t span;
    size_t *ref_count;

public:
    // 一系列get函数
    size_t get_row()
    {
        return this->row;
    }

    size_t get_col()
    {
        return this->col;
    }

    size_t get_channel()
    {
        return this->channel;
    }

    size_t get_ref_count()
    {
        return this->ref_count;
    }

    size_t get_span()
    {
        return this->span;
    }

    // 默认构造器
    // 根据数组创建矩阵
    Matrix(size_t row = 3, size_t col = 3, size_t channel = 1, T *data = nullptr, size_t ROIBeginRow = 0, size_t ROIBeginCol = 0, size_t ROIRow = 0, size_t ROICol = 0, size_t span = 0, size_t *ref_count = nullptr);

    // copy函数
    Matrix(const Matrix &);

    // 赋值的重载(soft copy)
    Matrix &operator=(const Matrix &mat);

    // 析构函数
    ~Matrix();

    // 运算符的重载
    // 重载<<方便打印
    template <class TT>
    friend std::ostream &operator<<(std::ostream &os, Matrix<TT> &mat);

    // 重载()便于访问元素
    T &operator()(size_t row, size_t col, size_t channel);

    // 加法
    Matrix operator+(T data) const;
    Matrix operator+(const Matrix &addend) const;
    // 友元函数实现 num+Matrix
    template <class TT>
    friend Matrix operator+(TT data, const Matrix<TT> &mat)
    {
        return mat + data;
    }

    // 减法
    Matrix operator-(T data) const;
    Matrix operator-(const Matrix &mat) const;
    // 友元函数实现 num-Matrix
    template <class TT>
    friend Matrix operator-(TT data, const Matrix<TT> &mat)
    {
        return mat - data;
    }

    // 乘法
    Matrix operator*(T data) const;
    Matrix operator*(Matrix &mat);
    // 友元函数实现 num*Matrix
    template <class TT>
    friend Matrix operator*(TT data, const Matrix<TT> &mat)
    {
        return mat * data;
    }

    //==的重载
    bool operator==(Matrix &mat);

    // 将!重载为矩阵转置
    Matrix operator!() const;

    // 返回矩阵中的元素个数
    size_t size()
    {
        size_t size = this->col * this->row;
        return size;
    }

    // ROI的相关操作
    // 子矩阵返回
    Matrix ROI() const;
    // 调整子矩阵位置
    void setROI(size_t row, size_t col, size_t ROIRow, size_t ROICol);
    void setROIPosition(size_t row, size_t col);
    void setROISize(size_t r, size_t c);
};

// 默认构造器
template <typename T>
Matrix<T>::Matrix(size_t row, size_t col, size_t channel, T *data, size_t ROIBeginRow, size_t ROIBeginCol, size_t ROIRow, size_t ROICol, size_t span, size_t *ref_count)
{
    if (row < 0 || col < 0 || ROIBeginCol < 0 || ROIBeginRow < 0 || ROIRow < 0 || ROICol < 0)
    {
        std::cerr << "In default constructor" << std::endl;
        std::cerr << "The input of rows and columns should not less than 0." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (channel < 1 || channel > 3)
    {
        std::cerr << "In default constructor" << std::endl;
        std::cerr << "The input of channel should between 1 and 3" << std::endl;
        exit(EXIT_FAILURE);
    }
    // 如果该矩阵的元素数组没有被引用过，则初始化为1，否则将引用指针指向传入指针
    if (ref_count == nullptr)
    {
        this->ref_count = new size_t[1];
        this->ref_count[0] = 1;
    }
    else
    {
        this->ref_count = ref_count;
    }

    // 对ROI的边界的合法性进行检查
    if (ROIBeginRow + ROIRow > row || ROIBeginCol + ROICol > col)
    {
        std::cerr << "In default constructor" << std::endl;
        std::cerr << "The region of interest should not exceed the region of the matrix." << std::endl;
        exit(EXIT_FAILURE);
    }

    // 赋值
    this->row = row;
    this->col = col;
    this->channel = channel;
    this->ROIBeginRow = ROIBeginRow;
    this->ROIBeginCol = ROIBeginCol;
    this->ROIRow = ROIRow;
    this->ROICol = ROICol;
    if (span == 0)
    {
        this->span = channel * col;
    }
    else
    {
        this->span = span;
    }

    // 根据通道数创建元素数组
    if (data == nullptr)
    {
        this->data = new T[row * col * channel];
    }
    else
    {
        size_t element = row * col * channel;
        this->data = new T[element];
        for (size_t i = 0; i < element; i++)
        {
            this->data[i] = data[i];
        }
    }
}

// soft copy
template <typename T>
Matrix<T>::Matrix(const Matrix<T> &mat)
{
    this->row = mat.row;
    this->col = mat.col;
    this->channel = mat.channel;
    this->ROIBeginRow = mat.ROIBeginRow;
    this->ROIBeginCol = mat.ROIBeginCol;
    this->ROIRow = mat.ROIRow;
    this->ROICol = mat.ROICol;
    this->ref_count = mat.ref_count;
    this->span = mat.span;

    this->ref_count[0]++;
    this->data = mat.data;
}

// 析构函数
template <typename T>
Matrix<T>::~Matrix()
{
    if (this->ref_count[0] == 1)
    {
        // printf("In ref_count\n");
        delete[] this->ref_count;
        this->ref_count = nullptr;
        // printf("After deleting ref_count pointer.\n");
        delete[] this->data;
        this->data = nullptr;
        // std::cout << "The element array has been free.\n";
    }
    else
    {
        this->ref_count[0]--;
        // std::cout << this -> ref_count[0] << " matrices share the element array.\n";
    }
}

// 赋值的重载(soft copy)
template <typename T>
Matrix<T> &Matrix<T>::operator=(const Matrix<T> &mat)
{

    // 防止自赋值
    if (this == &mat)
    {
        return *this;
    }
    // 如果不是自赋值，则释放原有对象成员指针指向的内存并将非指针变量一一赋值
    if (this->ref_count[0] == 1)
    {
        delete[] this->data;
        this->data = nullptr;
        delete[] this->ref_count;
        this->ref_count = nullptr;
        // std::cout << "The original element array has been free.\n";
    }
    else
    {
        this->ref_count[0]--;
    }

    this->row = mat.row;
    this->col = mat.col;
    this->channel = mat.channel;
    this->ROIBeginRow = mat.ROIBeginRow;
    this->ROIBeginCol = mat.ROIBeginCol;
    this->ROIRow = mat.ROIRow;
    this->ROICol = mat.ROICol;
    this->span = mat.span;
    this->ref_count = mat.ref_count;
    this->ref_count[0]++;
    // std::cout << this -> ref_count[0] << " matrices share the element array.\n";
    this->data = mat.data;
    return *this;
}

// 重载()便于访问元素
template <typename T>
T &Matrix<T>::operator()(size_t row, size_t col, size_t ch)
{
    // 参数合法性检查
    if (row < 0 || col < 0)
    {
        std::cerr << "The input of row and column should not be less than 0." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (ch < 1 || ch > channel)
    {
        std::cerr << "The input of channel should between 1 and 3" << std::endl;
        exit(EXIT_FAILURE);
    }

    return data[row * this->span + col * this->channel + ch - 1];
}

// 重载<<运算符
template <class T>
std::ostream &operator<<(std::ostream &os, Matrix<T> &mat)
{
    size_t ch = mat.channel;
    size_t r = mat.row;
    size_t c = mat.col;
    os << "[";
    for (size_t i = 0; i < r; i++)
    {
        for (size_t j = 0; j < c; j++)
        {
            os << "{";
            for (size_t k = 1; k <= ch; k++)
            {
                os << mat(i, j, k);
                if (k != ch)
                    os << " ";
            }
            os << "}";
            if (j != c - 1)
                os << ", ";
        }
        if (i != r - 1)
            os << "\n";
    }
    os << "]";
    return os;
}

// 加法运算符重载
template <typename T>
Matrix<T> Matrix<T>::operator+(T data) const
{
    Matrix<T> mat;
    mat.row = this->row;
    mat.col = this->col;
    mat.channel = this->channel;
    mat.ROIBeginRow = this->ROIBeginRow;
    mat.ROIBeginCol = this->ROIBeginCol;
    mat.ROIRow = this->ROIRow;
    mat.ROICol = this->ROICol;
    mat.span = this->span;

    size_t element = this->row * this->col * this->channel;
    mat.data = new T[element];

    for (size_t i = 0; i < element; i++)
    {
        mat.data[i] = this->data[i] + data;
    }

    return mat;
}

template <typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T> &addend) const
{

    // 参数合法性检查
    if (row != addend.row || col != addend.col || channel != addend.channel)
    {
        std::cerr << "In '+' operator overloading..." << std::endl;
        std::cerr << "The size of the two matrix should be the same." << std::endl;
        exit(EXIT_FAILURE);
    }

    Matrix<T> mat;
    mat.row = this->row;
    mat.col = this->col;
    mat.ROIBeginRow = this->ROIBeginRow;
    mat.ROIBeginCol = this->ROIBeginCol;
    mat.ROIRow = this->ROIRow;
    mat.ROICol = this->ROICol;
    mat.channel = this->channel;
    mat.span = this->span;

    size_t element = this->row * this->col * this->channel;
    mat.data = new T[element];

    for (size_t i = 0; i < element; i++)
    {
        mat.data[i] = this->data[i] + addend.data[i];
    }

    return mat;
}

// 减法运算符重载
template <typename T>
Matrix<T> Matrix<T>::operator-(T data) const
{
    Matrix<T> mat;
    mat.row = this->row;
    mat.col = this->col;
    mat.channel = this->channel;
    mat.ROIBeginRow = this->ROIBeginRow;
    mat.ROIBeginCol = this->ROIBeginCol;
    mat.ROIRow = this->ROIRow;
    mat.ROICol = this->ROICol;
    mat.span = this->span;

    size_t element = this->row * this->col * this->channel;
    mat.data = new T[element];

    for (size_t i = 0; i < element; i++)
    {
        mat.data[i] = this->data[i] - data;
    }

    return mat;
}

template <typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T> &subtrahend) const
{

    // 参数合法性检查
    if (row != subtrahend.row || col != subtrahend.col || channel != subtrahend.channel)
    {
        std::cerr << "In '-' operator overloading..." << std::endl;
        std::cerr << "The size of the two matrix should be the same." << std::endl;
        exit(EXIT_FAILURE);
    }

    Matrix<T> mat;
    mat.row = this->row;
    mat.col = this->col;
    mat.ROIBeginRow = this->ROIBeginRow;
    mat.ROIBeginCol = this->ROIBeginCol;
    mat.ROIRow = this->ROIRow;
    mat.ROICol = this->ROICol;
    mat.channel = this->channel;
    mat.span = this->span;

    size_t element = this->row * this->col * this->channel;
    mat.data = new T[element];

    for (size_t i = 0; i < element; i++)
    {
        mat.data[i] = this->data[i] - subtrahend.data[i];
    }

    return mat;
}

// 乘法运算符重载
template <typename T>
Matrix<T> Matrix<T>::operator*(T data) const
{
    Matrix mat;
    mat.row = this->row;
    mat.col = this->col;
    mat.channel = this->channel;
    mat.ROIBeginRow = this->ROIBeginRow;
    mat.ROIBeginCol = this->ROIBeginCol;
    mat.ROIRow = this->ROIRow;
    mat.ROICol = this->ROICol;
    mat.span = this->span;

    size_t element = this->row * this->col * this->channel;
    mat.data = new T[element];

    for (size_t i = 0; i < element; i++)
    {
        mat.data[i] = this->data[i] * data;
    }

    return mat;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(Matrix<T> &multiplier)
{
    // 判断参数合法性
    if (multiplier.row != this->col || multiplier.channel != channel)
    {
        std::cerr << "In operator * overloading..." << std::endl;
        std::cerr << "The two matrices' sizes and channel number should be the same." << std::endl;
        exit(EXIT_FAILURE);
    }

    Matrix<T> product;
    product.row = this->row;
    product.col = multiplier.col;
    product.channel = this->channel;
    product.ROIBeginRow = 0;
    product.ROIBeginCol = 0;
    product.ROIRow = 0;
    product.ROICol = 0;
    product.span = product.channel * product.col;
    size_t element = product.row * product.col * product.channel;
    product.data = new T[element];

   #pragma omp parallel for schedule(dynamic)
    for (size_t k = 0; k < multiplier.row; k++)
    {
        for (size_t i = 0; i < this->row; i++)
        {
            for (size_t j = 0; j < multiplier.col; j++)
            {
                for (size_t ch = 1; ch <= this->channel; ch++)
                {
                    product(i, j, ch) += (*this)(i, k, ch) * multiplier(k, j, ch);
                }
            }
        }
    }
    return product;
}

// 相等运算符重载
template <typename T>
bool Matrix<T>::operator==(Matrix<T> &mat)
{

    if (row != mat.row || col != mat.col || ROIBeginRow != mat.ROIBeginRow || ROIBeginCol != mat.ROIBeginCol || ROIRow != mat.ROIRow || ROICol != mat.ROIRow || channel != mat.channel)
    {
        return false;
    }

    size_t element = this->row * this->col * this->channel;

    for (size_t i = 0; i < element; i++)
    {
        if (this->data[i] != mat.data[i])
        {
            return false;
        }
    }

    return true;
}

// 矩阵转置运算符
template <typename T>
Matrix<T> Matrix<T>::operator!() const
{
    Matrix<T> mat;
    mat.row = this->col;
    mat.col = this->row;
    mat.ROIBeginRow = this->ROIBeginCol;
    mat.ROIBeginCol = this->ROIBeginRow;
    mat.ROIRow = this->ROICol;
    mat.ROICol = this->ROIRow;
    mat.channel = this->channel;
    size_t element = mat.row * mat.col * mat.channel;
    mat.data = new T[element];
    mat.span = mat.col * mat.channel;

    for (size_t i = 0; i < mat.row; i++)
    {
        for (size_t j = 0; j < mat.col; j++)
        {
            for (size_t k = 1; k <= mat.channel; k++)
            {
                mat(i, j, k) = this->data[j * this->span + i * this->channel + k - 1];
            }
        }
    }
    return mat;
}

// ROI相关操作
// 返回子矩阵
template <typename T>
Matrix<T> Matrix<T>::ROI() const
{
    // 对ROI的边界的合法性进行检查
    if (ROIBeginRow + ROIRow > row || ROIBeginCol + ROICol > col)
    {
        std::cerr << "The region of interest should not exceed the region of the matrix." << std::endl;
        exit(EXIT_FAILURE);
    }
    Matrix mat;

    mat.row = this->ROIRow;
    mat.col = this->ROICol;
    mat.channel = this->channel;
    mat.data = this->data + ROIBeginRow * col * channel + ROIBeginCol * channel;
    mat.ref_count = this->ref_count;
    mat.ref_count[0]++;
    mat.span = this->col * this->channel;
    return mat;
}

template <typename T>
void Matrix<T>::setROI(size_t row, size_t col, size_t ROIRow, size_t ROICol)
{
    // 参数正确性检查
    if (row < 0 || col < 0 || ROIRow < 0 || ROICol < 0)
    {
        std::cerr << "In function setROI..." << std::endl;
        std::cerr << "The input of row and column should not be less than 0." << std::endl;
        exit(EXIT_FAILURE);
    }

    this->ROIBeginCol = col;
    this->ROIBeginRow = row;
    this->ROIRow = ROIRow;
    this->ROICol = ROICol;
}

template <typename T>
void Matrix<T>::setROIPosition(size_t row, size_t col)
{
    // 参数正确性检查
    if (row < 0 || col < 0)
    {
        std::cerr << "In function setROIPosition..." << std::endl;
        std::cerr << "The input of row and column should not be less than 0." << std::endl;
        exit(EXIT_FAILURE);
    }
    if (row > this->row || col > this->col)
    {
        std::cerr << "In function setROIPosition..." << std::endl;
        std::cerr << "The input of row and column should not be larger than the row number or colomn number of the source matrix." << std::endl;
        exit(EXIT_FAILURE);
    }
    // 赋值
    this->ROIBeginRow = row;
    this->ROIBeginCol = col;
}

template <typename T>
void Matrix<T>::setROISize(size_t row, size_t col)
{
    // 参数正确性检查
    if (row < 0 || col < 0)
    {
        std::cerr << "In function setROISize..." << std::endl;
        std::cerr << "The input of row and column should not be less than 0." << std::endl;
        exit(EXIT_FAILURE);
    }

    this->ROIRow = row;
    this->ROICol = col;
};

#endif
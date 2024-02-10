#include <math.h>
#include <iostream>


#ifndef LINALG_H_INCLUDED
#define LINALG_H_INCLUDED 

template <typename T, int dim> class Vec{
    private:
        const int dimension = dim;
    public:
        T components[dim];
        Vec(T initVals[dim]);
        Vec(T fillVal);
        Vec();
        Vec<T,dim> operator+(Vec<T,dim> const& v);
        Vec<T,dim> operator*(T const& scalar);
        Vec<T,dim> operator-(Vec<T,dim> const& v);
        void operator=(Vec<T,dim> const& v);
        T operator*(Vec<T,dim> const& v);
        void Show();
};

template <typename T,int rowDim,int colDim> class Transform{
    private:
        const int numRows = rowDim;
        const int numCols = colDim;
    public:
        T data[rowDim][colDim];
        Transform(T data[rowDim][colDim]);
        Transform();
        template<int BcolDim> Transform<T,rowDim,BcolDim> operator*(Transform<T, colDim, BcolDim> const& B);
        Transform operator*(T const& scalar);
        Transform operator+(Transform const& B);
        Transform operator-(Transform const& B);
        Vec<T,rowDim> operator*(Vec<T,colDim> const& x);
        void operator=(Transform<T,rowDim,colDim> const& M);
        void Show();
};

class Rotation2D : public Transform<double,2,2>{
    public:
        Rotation2D(double theta);
};

class Scaling2D : public Transform<double,2,2>{
    public:
        Scaling2D(double scalar);
};

#endif
#include "LinAlg.hpp"
/* Vector methods */
template<typename T, int dim>
Vec<T,dim>::Vec(T initVals[dim]){
    for(int i = 0; i<dim; i++){
        components[i] = initVals[i];
    }
};

template<typename T, int dim>
Vec<T,dim>::Vec(T fillVal){
    for(int i=0; i<dim; i++){
        components[i] = fillVal;
    }
}

template<typename T, int dim>
Vec<T,dim>::Vec(void){
   for(int i=0; i<dim; i++){
    components[i] = 0;
   } 
}

template<typename T, int dim>
Vec<T,dim> Vec<T,dim>::operator+(Vec<T,dim>const& v){
    T newVals[dim];
    for(int i =0; i<dim; i++){
        newVals[i] = components[i] + v.components[i];
    }
    return Vec<T,dim>(newVals);
};

template<typename T, int dim>
Vec<T,dim> Vec<T,dim>::operator*(T const& scalar){
    T newVals[dim];
    for(int i =0; i<dim; i++){
        newVals[i] = components[i]*scalar;
    }
    return Vec<T,dim>(newVals);
}

template<typename T, int dim>
Vec<T,dim> Vec<T,dim>::operator-(Vec<T,dim>const& v){
    T newVals[dim];
    for(int i =0; i<dim; i++){
        newVals[i] = components[i] - v.components[i];
    }
    return Vec<T,dim>(newVals);
};

template<typename T, int dim>
T Vec<T,dim>::operator*(Vec<T,dim> const& v){
    T dotVal = 0; 
    for(int i=0; i<dim; i++){
        dotVal += components[i]*v.components[i];
    }
    return dotVal;
}

template <typename T, int dim>
void Vec<T,dim>::operator=(Vec<T,dim> const& v){
    for(int i=0; i<dim; i++){
        components[i] = v.components[i];
    }
}
template<typename T, int dim>
void Vec<T,dim>::Show(){
    std::cout << "< ";
    for(int i=0; i<dim; i++){
        std::cout << components[i] << " ";
    }
    std::cout << ">\n";
}

/* Transform definitions */
template <typename T,int rowDim,int colDim>
Transform<T,rowDim,colDim>::Transform(T inputData[rowDim][colDim]){
    for(int row =0; row<rowDim; row++){
        for(int col =0; col<colDim; col++){
            data[row][col] = inputData[row][col];
        }
    }
};

template <typename T,int rowDim,int colDim>
Transform<T,rowDim,colDim>::Transform(){
    for(int row =0; row<rowDim; row++){
        for(int col =0; col<colDim; col++){
            data[row][col] = 0;
        }
    }
};

template <typename T,int ArowDim, int sharedDim> template<int BcolDim>
Transform<T,ArowDim,BcolDim> Transform<T,ArowDim,sharedDim>::operator*(Transform<T,sharedDim,BcolDim> const& B){
    T newData[ArowDim][BcolDim];
    for (int outRow = 0; outRow < ArowDim; outRow ++){
        for (int outCol =0; outCol < BcolDim; outCol ++){
            newData[outRow][outCol] = 0;
            for (int outShared = 0; outShared < sharedDim; outShared++){
                newData[outRow][outCol] += data[outRow][outShared]*B.data[outShared][outCol];
            }
    }
    }
    return Transform<T,ArowDim,BcolDim>(newData);
};

template <typename T,int rowDim, int colDim>
void Transform<T,rowDim,colDim>::Show(){
    std::cout << "[ ";
    for(int row = 0; row < rowDim-1; row++){
        for (int col =0; col < colDim; col++){
            std::cout << data[row][col] << " ";
        }
        std::cout << "\n";
    }
    for(int col =0; col < colDim; col++){
        std::cout << data[rowDim-1][col] << " ";
    }
    std::cout << "]\n";
}

template <typename T>
Transform<T,2,2> Inverse(Transform<T,2,2> const& A){
    T scaleFactor = 1/(A.data[0][0]*A.data[1][1] - A.data[0][1]*A.data[1][0]);
    T newData[2][2] = {scaleFactor*A.data[1][1], -scaleFactor*A.data[0][1] , -scaleFactor*A.data[1][0], scaleFactor*A.data[0][0]};
    return Transform<T,2,2>(newData);
}


template <typename T,int rowDim,int colDim>
Transform<T,rowDim,colDim> Transform<T,rowDim,colDim>::operator*(T const& scalar){
    T newData[rowDim][colDim];
    for (int row = 0; row < rowDim; row++){
        for (int col =0; col < colDim; col++){
            newData[row][col] = data[row][col]*scalar;
        }
    }
    return Transform<T,rowDim,colDim>(newData);
};


template <typename T,int rowDim,int colDim>
Transform<T,rowDim,colDim> Transform<T,rowDim,colDim>::operator+(Transform const& B){
    T newData[rowDim][colDim];
    for (int row =0; row < rowDim; row++){
        for (int col =0; col < colDim; col++){
            newData[row][col] = data[row][col] + B.data[row][col];
        }
    }
    return Transform<T,rowDim,colDim>(newData);
};

template <typename T,int rowDim,int colDim>
Transform<T,rowDim,colDim> Transform<T,rowDim,colDim>::operator-(Transform const& B){
    T newData[rowDim][colDim];
    for (int row =0; row < rowDim; row++){
        for (int col =0; col < colDim; col++){
            newData[row][col] = data[row][col] - B.data[row][col];
        }
    }
    return Transform<T,rowDim,colDim>(newData);
};

template <typename T,int rowDim,int colDim>
Vec<T,rowDim> Transform<T,rowDim,colDim>::operator*(Vec<T,colDim> const& x){
    T newVals[rowDim];
    for(int row =0; row< rowDim; row++){
        newVals[row] = 0;
        for(int col =0; col< colDim; col++){
            newVals[row] += data[row][col]*x.components[col]; 
        }
    }
    return Vec<T,rowDim>(newVals);
}

template <typename T, int rowDim, int colDim>
void Transform<T,rowDim,colDim>::operator=(Transform<T,rowDim,colDim> const& M){
    for(int i =0; i< rowDim; i++){
        for(int j=0; j<colDim; j++){
            data[i][j] = M.data[i][j]; 
        }
    }
}


Rotation2D::Rotation2D(double theta){
    data[0][0] = cos(theta);
    data[0][1] = -sin(theta);
    data[1][0] = sin(theta);
    data[1][1] = -cos(theta);
};

Scaling2D::Scaling2D(double scalar){
    data[0][0] = scalar;
    data[1][1] = scalar;
}

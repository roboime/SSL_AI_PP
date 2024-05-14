#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <mutex>
#include <atomic>
#include <Eigen/Dense>
#include "../../../base_component.h"
#include "../../../messages/environment.h"
#include "../../../messages/raw_environment.h"

template<int stateVectorDimension, int outputVectorDimension>
class KalmanFilter
{
public:
    typedef Eigen::Matrix<double, stateVectorDimension,
                          stateVectorDimension> stateMatrix;
    typedef Eigen::Matrix<double, outputVectorDimension,
                          stateVectorDimension> outputMatrix;
    typedef Eigen::Matrix<double, outputVectorDimension,
                          outputVectorDimension> observationNoiseCovariance;
    typedef Eigen::Matrix<double, stateVectorDimension,
                          outputVectorDimension> inputMatrix;
    typedef Eigen::Matrix<double, stateVectorDimension, 1> stateVector;
    typedef Eigen::Matrix<double, outputVectorDimension, 1> outputVector;

private:
    stateVector vector_x;
    stateMatrix covarianceMatrix_P; //Accuracy of the state estimate

public:
    stateMatrix matrix_Phi;
    outputMatrix matrix_H;
    stateMatrix covarianceMatrix_Q;
    observationNoiseCovariance covarianceMatrix_R;
    stateMatrix matrix_B; //state transition jacobian

    outputVector vector_measurementStateError;
    KalmanFilter(){
        matrix_Phi = stateMatrix::Identity();
        matrix_H = outputMatrix::Identity();
        covarianceMatrix_Q = stateMatrix::Zero();
        covarianceMatrix_R = observationNoiseCovariance::Zero();
        matrix_B = stateMatrix::Identity();
        vector_measurementStateError = outputVector::Zero();
    }
    explicit KalmanFilter(const stateVector &x, const stateMatrix & P)
        : vector_x(x)
        , covarianceMatrix_P(P)
    {
        matrix_Phi = stateMatrix::Identity();
        matrix_H = outputMatrix::Identity();
        covarianceMatrix_Q = stateMatrix::Zero();
        covarianceMatrix_R = observationNoiseCovariance::Zero();

        matrix_B = stateMatrix::Identity();
        vector_measurementStateError = outputVector::Zero();
    }
    Environment update(RawEnvironment raw_env){

    }
    void predict_timeUpdate(){
        vector_x = matrix_Phi*vector_x;
        covarianceMatrix_P = matrix_Phi*covarianceMatrix_P*matrix_Phi.transpose() + covarianceMatrix_Q;
    };

    void predict_timeUpdate(const stateVector &controlVector_u){
        vector_x = matrix_Phi*vector_x + matrix_B*controlVector_u;
        covarianceMatrix_P = matrix_Phi*covarianceMatrix_P*matrix_Phi.transpose() + covarianceMatrix_Q;
    };

    void correction_measurementUpdate(const outputVector &cameraMeasurementVector_y){
        vector_measurementStateError = cameraMeasurementVector_y - (matrix_H*vector_x);
        observationNoiseCovariance auxiliarMatrix = matrix_H*covarianceMatrix_P*matrix_H.transpose() + covarianceMatrix_R;
        inputMatrix kalmanGain = covarianceMatrix_P*matrix_H.transpose()*auxiliarMatrix.inverse();
        vector_x += kalmanGain*vector_measurementStateError;
        covarianceMatrix_P -= kalmanGain*matrix_H*covarianceMatrix_P;
    };
    const stateVector &getStateVector() const{
        return vector_x;
    }
    const stateMatrix &getCovarianceMatrix_P() const{
        return covarianceMatrix_P;
    }

    void modifyStateVector(int index, double value)
    {
        vector_x(index) = value;
    }

    void setStateVector(const stateVector& newVector_x){
        vector_x = newVector_x;
    }
    void setCovarianceMatrix_P(const stateMatrix &newCovarianceMatrix_P){
        covarianceMatrix_P = newCovarianceMatrix_P;
    }
};

#endif
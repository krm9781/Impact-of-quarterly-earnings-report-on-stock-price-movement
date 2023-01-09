#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>

using namespace std;
namespace fre {
	typedef vector<double> Vector;
	typedef vector<Vector> Matrix;
	// overload operators as independent functions

	ostream& operator<<(ostream& out, Vector& V)
    {
        for (Vector::iterator itr = V.begin(); itr != V.end(); itr++)
            out << *itr << "   ";
        out << endl;
        return out;
    }

    ostream& operator<<(ostream& out, Matrix& W)
    {
        for (Matrix::iterator itr = W.begin(); itr != W.end(); itr++)
            out << *itr;    // Use ostream & operator<<(ostream & out, Vector & V)
        out << endl;
        return out;
    }
	
	Vector operator/(const Vector& V, const double& a)
	{
		int d = (int)V.size();
		Vector U(d);
		for (int j = 0; j < d; j++) U[j] = V[j]/a;
		return U;
	}
	
	Vector operator+(const Vector& V, const Vector& W)
	{
		int d = (int)V.size();
		Vector U(d);
		for (int j = 0; j < d; j++) U[j] = V[j] + W[j];
		return U;
	}
	
    Vector operator+(const double& a, const Vector& V)
    {
		int d = (int)V.size();
		Vector U(d);
		for (int j = 0; j < d; j++) U[j] = a + V[j];
		return U;
    }
    
    Vector operator-(const Vector& V, const Vector& W)
	{
		int d = (int)V.size();
		Vector U(d);
		for (int j = 0; j < d; j++) U[j] = V[j] - W[j];
		return U;
	}
	
	Vector operator-(const double& a, const Vector& V)
    {
		int d = (int)V.size();
		Vector U(d);
		for (int j = 0; j < d; j++) U[j] = a - V[j];
		return U;
    }
    
    Vector operator*(const Matrix& C, const Vector& V)
    {
        int d = (int)C.size();
        Vector W(d);
        for (int j = 0; j < d; j++)
        {
            W[j] = 0.0;
            for (int l = 0; l < d; l++) W[j] = W[j] + C[j][l] * V[l];
        }
        return W;
    }
	
	Vector operator*(const double& a, const Vector& V)
	{
		int d = (int)V.size();
		Vector U(d);
		for (int j = 0; j < d; j++) U[j] = a * V[j];
		return U;
	}
	
	Vector operator*(const Vector& V, const Vector& W)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = V[j] * W[j];
        return U;
    }
    
    Vector exp(const Vector& V)
    {
        int d = (int)V.size();
        Vector U(d);
        for (int j = 0; j < d; j++) U[j] = std::exp(V[j]);
        return U;
    }

    Vector operator^(const Vector& V, const double& a)
    {
        int d = (int)V.size();
		Vector U(d);
		for (int j = 0; j < d; j++) U[j] = pow(V[j],a);
		return U;
    }
    
    void Display(const Matrix &mat)
    {
        for (int i = 0; i < mat.size(); i++)
        {
            for (int j = 0; j < mat[0].size(); j++)
            { cout << fixed << setprecision(5) << mat[i][j] << "\t";} 
            cout << endl;
        } cout << endl;
    }
    
	void Display(const Vector &vec)
    {
        for (int i = 0; i < vec.size(); i++)
        { cout << fixed << setprecision(5) << vec[i] << "\t"; } 
        cout << endl << endl;
    }
    
    void Display(const vector<string> &vec)
    {
        for (int i = 0; i < vec.size(); i++)
        { cout << vec[i] << "\t"; } 
        cout << endl << endl;
    }
	
}



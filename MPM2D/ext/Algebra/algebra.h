#pragma once

#include <algorithm>
#include <iostream>

/* Vector and Matrix classes. */
#pragma region "Vector2f"
class Matrix2f;
class Vector2f
{
public:

	/* Data */
	float val[2];



	/* Constructors */
	Vector2f();
	Vector2f(const float x);
	Vector2f(const float x0, const float x1);
	Vector2f(const Vector2f& V);
	virtual ~Vector2f() {};



	/* Operators */

	// []
	float& operator[](int id);
	const float& operator[](int id) const;

	// -Vector
	const Vector2f operator-() const;

	// ||Vector||
	float norm() const;

	// Vector ^ (-1) (Element-wise inverse)
	const Vector2f inv() const;

	// log(Vector) (Element-wise log /!\ if 0)
	const Vector2f log() const;

	// exp(Vector) (Element-wise exponential)
	const Vector2f exp() const;

	// sum(Vector) 
	float sum() const;

	// SetData pointers
	void setData(const float x0, const float x1);
	void setData(const float x);

	// Set particular values
	void setZeros();
	void setOnes();



	/* Vector and Vector */

	// Vector + Vector
	Vector2f& operator+=(const Vector2f& V);
	const Vector2f operator+(const Vector2f& V) const;

	//Vector - Vector
	Vector2f& operator-=(const Vector2f& V);
	const Vector2f operator-(const Vector2f& V) const;

	// Vector * Vector^T
	const Matrix2f outer_product(const Vector2f& V) const;

	// Vector . Vector
	float dot(const Vector2f &V) const;

	// Vector * Vector (Element-wise product)
	Vector2f& operator*=(const Vector2f& V);
	const Vector2f operator*(const Vector2f& V) const;


	/* Vector and Scalar */

	// Vector + Scalar
	const Vector2f operator+(const float& scal) const;
	Vector2f& operator+=(const float& scal);

	// Vector - Scalar
	const Vector2f operator-(const float& scal) const;
	Vector2f& operator-=(const float& scal);

	// Vector * Scalar
	const Vector2f operator*(const float& scal) const;
	Vector2f& operator*=(const float& scal);

	// Vector / Scalar
	const Vector2f operator/(const float& scal) const;
	Vector2f& operator/=(const float& scal);
};

/* Supp */

const Vector2f operator-(const float& scal, const Vector2f& V);
const Vector2f operator+(const float& scal, const Vector2f& V);
const Vector2f operator*(const float& scal, const Vector2f& V);
const Vector2f operator/(const float& scal, const Vector2f& V);
const std::ostream &operator<<(std::ostream &os, const Vector2f& V);
#pragma endregion "Vector2f"


#pragma region "Matrix2f"
class Matrix2f
{
public:

	/* Data. [row][column] -*/
	float val[2][2];



	/* Constructors */
	Matrix2f();
	Matrix2f(const float x);
	Matrix2f(const float x00, const float x01,
		const float x10, const float x11);
	Matrix2f(const Matrix2f& M);
	virtual ~Matrix2f() {};



	/* Operators */

	// []
	float* operator[](int id);
	const float* operator[](int id) const;

	// -Matrix
	const Matrix2f operator-() const;

	// tr(Matrix)
	float trace() const;

	// SVD(Matrix)
	void svd(Matrix2f* U, Vector2f* Eps, Matrix2f* V) const;

	// SetData pointers
	void setData(const float x00, const float x01, const float x10, const float x11);
	void setData(const float x);

	// Set particular values
	void setZeros();
	void setIdentity();

	// Transpose(Matrix)
	const Matrix2f transpose() const;


	/* Matrix and Matrix */

	// Matrix + Matrix
	const Matrix2f operator+(const Matrix2f& M) const;
	Matrix2f& operator+=(const Matrix2f& M);

	//Matrix - Matrix
	const Matrix2f operator-(const Matrix2f& M) const;
	Matrix2f& operator-=(const Matrix2f& M);

	// Matrix * Matrix
	const Matrix2f operator*(const Matrix2f& M) const;

	// Diagonal Matrix * Matrix  
	const Matrix2f diag_product(const Vector2f& V) const;

	// Diagonal Matrix * Matrix ^ (-1) 
	const Matrix2f diag_product_inv(const Vector2f& V) const;



	/* Matrix and Vector */

	// Matrix * Vector
	const Vector2f operator*(const Vector2f& V) const;



	/* Matrix and Scalar */

	// Matrix + scalar
	const Matrix2f operator+(const float& scal) const;
	Matrix2f& operator+=(const float& scal);

	// Matrix - scalar
	const Matrix2f operator-(const float& scal) const;
	Matrix2f& operator-=(const float& scal);

	// Matrix * scalar
	const Matrix2f operator*(const float& scal) const;
	Matrix2f& operator*=(const float& scal);

	// Matrix / scalar
	const Matrix2f operator/(const float& scal) const;
	Matrix2f& operator/=(const float& scal);
};

/* Supp */

const Matrix2f operator-(const float& scal, const Matrix2f& M);
const Matrix2f operator+(const float& scal, const Matrix2f& M);
const Matrix2f operator*(const float& scal, const Matrix2f& M);
const Matrix2f operator/(const float& scal, const Matrix2f& M);
const std::ostream &operator<<(std::ostream &os, const Matrix2f& M);
#pragma endregion "Matrix2f"
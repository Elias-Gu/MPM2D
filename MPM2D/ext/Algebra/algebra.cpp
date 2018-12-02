#include "algebra.h"


#pragma region "Vector2f"
/*------------------------------------------------------------*/
/*                           Vector2f                         */
/*------------------------------------------------------------*/

/* Constructors */
Vector2f::Vector2f() {
	val[0] = 0; val[1] = 0;
}
Vector2f::Vector2f(const float x) {
	val[0] = x; val[1] = x;
}
Vector2f::Vector2f(const float x0, const float x1) {
	val[0] = x0; val[1] = x1;
}
Vector2f::Vector2f(const Vector2f& V)
{
	val[0] = V[0]; val[1] = V[1];
}


/* Operators */

// []
float& Vector2f::operator[](int id) {
	return val[id];
}
const float& Vector2f::operator[](int id) const {
	return val[id];
}

// -Vector
const Vector2f Vector2f::operator-() const {
	return Vector2f(-val[0], -val[1]);
}

// ||Vector||
float Vector2f::norm() const {
	return
		sqrt(val[0] * val[0] + val[1] * val[1]);
}

// Vector ^ (-1) (Element-wise inverse)
const Vector2f Vector2f::inv() const {
	return
		Vector2f(1.0f / val[0], 1.0f / val[1]);
}

// log(Vector) (Element-wise log /!\ if 0)
const Vector2f Vector2f::log() const {
	return
		Vector2f(std::log(val[0]), std::log(val[1]));
}

// exp(Vector) (Element-wise exponential)
const Vector2f Vector2f::exp() const {
	return
		Vector2f(std::exp(val[0]), std::exp(val[1]));
}

// sum(Vector) 
float Vector2f::sum() const {
	return val[0] + val[1];
}

// clamp(Vector) between low and high
const Vector2f Vector2f::clamp(const float low, const float high) const {
	return Vector2f(std::clamp(val[0], low, high),
		std::clamp(val[1], low, high));
}

// SetData pointers
void Vector2f::setData(const float x0, const float x1) {
	val[0] = x0; val[1] = x1;
}
void Vector2f::setData(const float x) {
	val[0] = x; val[1] = x;
}

// Set particular values
void Vector2f::setZeros() {
	this->setData(0);
}
void Vector2f::setOnes() {
	this->setData(1);
}



/* Vector and Vector */

// Vector + Vector
Vector2f& Vector2f::operator+=(const Vector2f& V) {
	val[0] += V[0]; val[1] += V[1];
	return *this;
}
const Vector2f Vector2f::operator+(const Vector2f& V) const {
	return Vector2f(*this) += V;
}

//Vector - Vector
Vector2f& Vector2f::operator-=(const Vector2f& V) {
	val[0] -= V[0]; val[1] -= V[1];
	return *this;
}
const Vector2f Vector2f::operator-(const Vector2f& V) const {
	return Vector2f(*this) -= V;
}


// Vector * Vector^T
const Matrix2f Vector2f::outer_product(const Vector2f& V) const {
	return Matrix2f(
		val[0] * V.val[0], val[0] * V.val[1],
		val[1] * V.val[0], val[1] * V.val[1]);
}

// Vector . Vector
float Vector2f::dot(const Vector2f &V) const {
	return
		val[0] * V.val[0] + val[1] * V.val[1];
}

// Vector * Vector (Element-wise product)
Vector2f& Vector2f::operator*=(const Vector2f& V) {
	val[0] *= V[0]; val[1] *= V[1];
	return *this;
}
const Vector2f Vector2f::operator*(const Vector2f& V) const {
	return Vector2f(*this) *= V;
}



/* Vector and Scalar */

// Vector + Scalar
const Vector2f Vector2f::operator+(const float& scal) const {
	return Vector2f(*this) += scal;
}
Vector2f& Vector2f::operator+=(const float& scal) {
	val[0] += scal; val[1] += scal;
	return *this;
}

// Vector - Scalar
const Vector2f Vector2f::operator-(const float& scal) const {
	return Vector2f(*this) -= scal;
}
Vector2f& Vector2f::operator-=(const float& scal) {
	val[0] -= scal; val[1] -= scal;
	return *this;
}

// Vector * Scalar
const Vector2f Vector2f::operator*(const float& scal) const {
	return Vector2f(*this) *= scal;
}
Vector2f& Vector2f::operator*=(const float& scal) {
	val[0] *= scal; val[1] *= scal;
	return *this;
}

// Vector / Scalar
const Vector2f Vector2f::operator/(const float& scal) const {
	return Vector2f(*this) /= scal;
}
Vector2f& Vector2f::operator/=(const float& scal) {
	val[0] /= scal; val[1] /= scal;
	return *this;
}



/* Supp */

const Vector2f operator-(const float& scal, const Vector2f& V) {
	return Vector2f(V) - scal;
}
const Vector2f operator+(const float& scal, const Vector2f& V) {
	return Vector2f(V) + scal;
}
const Vector2f operator*(const float& scal, const Vector2f& V) {
	return Vector2f(V) * scal;
}
const Vector2f operator/(const float& scal, const Vector2f& V) {
	return Vector2f(V) / scal;
}
const std::ostream &operator<<(std::ostream &os, const Vector2f& V) {
	os << "[" << V.val[0] << " : " << V.val[1] << "]" << std::endl;
	return os;
}
#pragma endregion "Vector2f"




#pragma region "Matrix2f"
/*------------------------------------------------------------*/
/*                           Matrix2f                         */
/*------------------------------------------------------------*/

/* Constructors */
Matrix2f::Matrix2f() {
	val[0][0] = 0; val[0][1] = 0;
	val[1][0] = 0; val[1][1] = 0;
}
Matrix2f::Matrix2f(const float x) {
	val[0][0] = x; val[0][1] = x;
	val[1][0] = x; val[1][1] = x;
}
Matrix2f::Matrix2f(const float x00, const float x01,
	const float x10, const float x11) {
	val[0][0] = x00; val[0][1] = x01;
	val[1][0] = x10; val[1][1] = x11;
}
Matrix2f::Matrix2f(const Matrix2f& M) {
	val[0][0] = M[0][0]; val[0][1] = M[0][1];
	val[1][0] = M[1][0]; val[1][1] = M[1][1];
}



/* Operators */

// []
float* Matrix2f::operator[](int id) {
	return val[id];
}
const float* Matrix2f::operator[](int id) const {
	return val[id];
}

// -Matrix
const Matrix2f Matrix2f::operator-() const {
	return Matrix2f(-val[0][0], -val[0][1],
		-val[1][0], -val[1][1]);
}

// Matrix^(-1)
const Matrix2f Matrix2f::inv() const {
	float det = Matrix2f(*this).det();
	return Matrix2f(val[1][1], -val[0][1],
		-val[1][0], val[0][0]) / det;
}

// tr(Matrix)
float Matrix2f::trace() const {
	return val[0][0] + val[1][1];
}

// det(Matrix)
float Matrix2f::det() const {
	return val[0][0] * val[1][1] - val[0][1] * val[1][0];
}

// SVD(Matrix)
//http://www.ualberta.ca/~mlipsett/ENGM541/Readings/svd_ellis.pdf
//https://github.com/victorliu/Cgeom/blob/master/geom_la.c
void Matrix2f::svd(Matrix2f* U, Vector2f* Eps, Matrix2f* V) const {
	float MATRIX_EPSILON = 1e-6f;
	if (fabs(val[0][1] - val[1][0]) < MATRIX_EPSILON && fabs(val[0][1]) < MATRIX_EPSILON) {
		U->setData(val[0][0] < 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f, val[1][1] < 0.0f ? -1.0f : 1.0f);
		Eps->setData(fabs(val[0][0]), fabs(val[1][1]));
		V->setData(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		float j = val[0][0] * val[0][0] + val[0][1] * val[0][1],
			k = val[1][0] * val[1][0] + val[1][1] * val[1][1],
			v_c = val[0][0] * val[1][0] + val[0][1] * val[1][1];
		if (fabs(v_c) < MATRIX_EPSILON) {
			float s1 = sqrt(j), s2 = fabs(j - k) < MATRIX_EPSILON ? s1 : sqrt(k);
			Eps->setData(s1, s2);
			V->setData(1.0f, 0.0f, 0.0f, 1.0f);
			U->setData(
				val[0][0] / s1, val[1][0] / s2,
				val[0][1] / s1, val[1][1] / s2);
		}
		else {
			float jmk = j - k,
				jpk = j + k,
				root = sqrt(jmk*jmk + 4 * v_c*v_c),
				eig = (jpk + root) / 2,
				s1 = sqrt(eig),
				s2 = fabs(root) < MATRIX_EPSILON ? s1 : sqrt((jpk - root) / 2);
			Eps->setData(s1, s2);
			float v_s = eig - j,
				len = sqrt(v_s*v_s + v_c * v_c);
			v_c /= len;
			v_s /= len;
			V->setData(v_c, -v_s, v_s, v_c);
			U->setData(
				(val[0][0] * v_c + val[1][0] * v_s) / s1,
				(val[1][0] * v_c - val[0][0] * v_s) / s2,
				(val[0][1] * v_c + val[1][1] * v_s) / s1,
				(val[1][1] * v_c - val[0][1] * v_s) / s2);
		}
	}
}

// polar decomposition
// http://www.cs.cornell.edu/courses/cs4620/2014fa/lectures/polarnotes.pdf
void Matrix2f::polar_decomp(Matrix2f* R, Matrix2f* S) const {
	float th = atan2(val[1][0] - val[0][1], val[0][0] + val[1][1]);
	*R = Matrix2f(cos(th), -sin(th), sin(th), cos(th));
	*S = R->transpose() * *this;
}

// SetData pointers
void Matrix2f::setData(const float x00, const float x01, const float x10, const float x11) {
	val[0][0] = x00; val[0][1] = x01;
	val[1][0] = x10; val[1][1] = x11;
}
void Matrix2f::setData(const float x) {
	val[0][0] = x; val[0][1] = x;
	val[1][0] = x; val[1][1] = x;
}

// Set particular values
void Matrix2f::setZeros() {
	this->setData(0);
}
void Matrix2f::setIdentity() {
	this->setData(1, 0, 0, 1);
}

// Transpose(Matrix)
const Matrix2f Matrix2f::transpose() const {
	return Matrix2f(val[0][0], val[1][0],
		val[0][1], val[1][1]);
}



/* Matrix and Matrix */

// Matrix + Matrix
const Matrix2f Matrix2f::operator+(const Matrix2f& M) const {
	return Matrix2f(*this) += M;
}
Matrix2f& Matrix2f::operator+=(const Matrix2f& M) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			val[i][j] += M.val[i][j];
	return *this;
}

//Matrix - Matrix
const Matrix2f Matrix2f::operator-(const Matrix2f& M) const {
	return Matrix2f(*this) -= M;
}
Matrix2f& Matrix2f::operator-=(const Matrix2f& M) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			val[i][j] -= M.val[i][j];
	return *this;
}

// Matrix * Matrix
const Matrix2f Matrix2f::operator*(const Matrix2f& M) const {
	Matrix2f outM;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				outM.val[i][j] += val[i][k] * M.val[k][j];
	return outM;
}

// Diagonal Matrix * Matrix  
const Matrix2f Matrix2f::diag_product(const Vector2f& V) const {
	Matrix2f outM = Matrix2f(*this);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			outM.val[i][j] *= V[j];
	return outM;
}

// Diagonal Matrix ^ (-1)  * Matrix
const Matrix2f Matrix2f::diag_product_inv(const Vector2f& V) const {
	Matrix2f outM = Matrix2f(*this);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			outM.val[i][j] /= V[j];
	return outM;
}



/* Matrix and Vector */

// Matrix * Vector
const Vector2f Matrix2f::operator*(const Vector2f& V) const {
	return Vector2f(V[0] * val[0][0] + V[1] * val[0][1],
		V[0] * val[1][0] + V[1] * val[1][1]);
}



/* Matrix and Scalar */

// Matrix + scalar
const Matrix2f Matrix2f::operator+(const float& scal) const {
	return Matrix2f(*this) += scal;
}
Matrix2f& Matrix2f::operator+=(const float& scal) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			val[i][j] += scal;
	return *this;
}

// Matrix - scalar
const Matrix2f Matrix2f::operator-(const float& scal) const {
	return Matrix2f(*this) -= scal;
}
Matrix2f& Matrix2f::operator-=(const float& scal) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			val[i][j] -= scal;
	return *this;
}

// Matrix * scalar
const Matrix2f Matrix2f::operator*(const float& scal) const {
	return Matrix2f(*this) *= scal;
}
Matrix2f& Matrix2f::operator*=(const float& scal) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			val[i][j] *= scal;
	return *this;
}

// Matrix / scalar
const Matrix2f Matrix2f::operator/(const float& scal) const {
	return Matrix2f(*this) /= scal;
}
Matrix2f& Matrix2f::operator/=(const float& scal) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			val[i][j] /= scal;
	return *this;
}



/* Supp */

const Matrix2f operator-(const float& scal, const Matrix2f& M) {
	return Matrix2f(M) - scal;
}
const Matrix2f operator+(const float& scal, const Matrix2f& M) {
	return Matrix2f(M) + scal;
}
const Matrix2f operator*(const float& scal, const Matrix2f& M) {
	return Matrix2f(M) * scal;
}
const Matrix2f operator/(const float& scal, const Matrix2f& M) {
	return Matrix2f(M) / scal;
}
const std::ostream &operator<<(std::ostream &os, const Matrix2f& M) {
	os << "[" << M.val[0][0] << " , " << M.val[0][1] << "]" << std::endl;
	os << "[" << M.val[1][0] << " , " << M.val[1][1] << "]" << std::endl;
	return os;
}
#pragma endregion "Matrix2f"
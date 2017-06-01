#ifndef VMATH_H
#define VMATH_H

#ifndef M_PI
#define M_PI           3.141592653589793238462643383279502884197169399375105820974944
#define DEG2RAD(a)     ((a) * (M_PI / 180.0))
#define RAD2DEG(a)     ((a) * (180.0 / M_PI))
#define ANGLE2SHORT(x) ((int)((((double)x) * (65536.0 / 360.0)) + 0.5) & 0xFFFF)
#define SHORT2ANGLE(x) ((x) * (360.0 / 65536.0))
#endif
#ifndef DEG2RADF
#define M_PIF          3.141592653589793238462643383279502884197169399375105820974944f
#define DEG2RADF(a)    ((a) * (M_PIF / 180.0f))
#define RAD2DEGF(a)    ((a) * (180.0f / M_PIF))
#endif

#include <cmath>

double NormalizeAngle180(double angle);
double NormalizeAngle360(double angle);
double Lerp(double from, double to, double frac);

template <typename T>
class _Vector2 {
public:
	inline _Vector2() { return; }

	inline _Vector2(_Vector2<T> *v) {
		x = v->x; y = v->y;
	}

	inline _Vector2(T _x, T _y) {
		x = _x; y = _y;
	}

	inline bool IsEqual(_Vector2<T> *v) {
		return x == v->x && y == v->y;
	}

	inline void ZeroInitialize() {
		x = y = 0;
	}

	inline void Initialize(T _x, T _y) {
		x = _x; y = _y;
	}

	inline void Initialize(_Vector2<T> *v) {
		x = v->x; y = v->y;
	}
	
	inline void Add(_Vector2<T> *v) {
		x += v->x; y += v->y;
	}

	inline void Subtract(_Vector2<T> *v) {
		x -= v->x; y -= v->y;
	}

	inline void Multiply(_Vector2<T> *v) {
		x *= v->x; y *= v->y;
	}

	inline void Scale(T s) {
		x *= s; y *= s;
	}

	inline void Divide(_Vector2<T> *v) {
		x /= v->x; y /= v->y;
	}

	inline void Lerp(_Vector2<T> *v) {
		x += v->x; y += v->y;
	}

	inline T Length() {
		return sqrt(x * x + y * y);
	}

	inline T LengthFast() {
		return x * x + y * y;
	}

	inline T Sum() {
		return x + y;
	}

	inline T Distance(_Vector2<T> *v) {
		_Vector2<T> d(v);
		d.Subtract(this);
		return d.Length();
	}

	inline T DistanceFast(_Vector2<T> *v) {
		_Vector2<T> d(v);
		d.Subtract(this);
		return d.LengthFast();
	}

	inline void Unitize() {
		T length = this->GetLength();
		x /= length; y /= length;
	}

	inline T &operator[](unsigned int i) {
		switch (i) {
		default:
		case 0:
			return x;

		case 1:
			return y;
		}
	}

	T x, y;
};

template <typename T>
class _Vector3 {
public:
	inline _Vector3() { return; }

	inline _Vector3(_Vector3<T> *v) {
		x = v->x; y = v->y; z = v->z;
	}

	inline _Vector3(T _x, T _y, T _z) {
		x = _x; y = _y; z = _z;
	}

	inline bool IsEqual(_Vector3<T> *v) {
		return x == v->x && y == v->y && z == v->z;
	}

	inline void ZeroInitialize() {
		x = y = z = 0;
	}

	inline void Initialize(T _x, T _y, T _z) {
		x = _x; y = _y; z = _z;
	}

	inline void Initialize(_Vector3<T> *v) {
		x = v->x; y = v->y; z = v->z;
	}

	inline void Add(_Vector3<T> *v) {
		x += v->x; y += v->y; z += v->z;
	}

	inline void Add(_Vector3<T> &v) {
		x += v.x; y += v.y; z += v.z;
	}

	inline void Subtract(_Vector3<T> *v) {
		x -= v->x; y -= v->y; z -= v->z;
	}

	inline void Multiply(_Vector3<T> *v) {
		x *= v->x; y *= v->y; z *= v->z;
	}

	inline void Scale(T s) {
		x *= s; y *= s; z *= s;
	}

	inline void Divide(_Vector3<T> *v) {
		x /= v->x; y /= v->y; z /= v->z;
	}

	inline void Divide(T v) {
		x /= v; y /= v; z /= v;
	}

	inline void Lerp(_Vector3<T> *to, double f) {
		x += f * (to->x - x); y += f * (to->y - y); z += f * (to->z - z);
	}

	inline T Length() {
		return sqrt(x * x + y * y + z * z);
	}

	inline T LengthFast() {
		return x * x + y * y + z * z;
	}

	inline T Sum() {
		return x + y + z;
	}

	inline void Abs() {
		if (x < 0.f) {
			x = -x;
		}
		
		if (x < 0.f) {
			y = -y;
		}
		
		if (z < 0.f) {
			z = -z;
		}
	}

	inline T DotProduct(_Vector3<T> *v) {
		return x * v->x + y * v->y + z * v->z;
	}

	inline T Distance(_Vector3<T> *v) {
		_Vector3<T> d(v);
		d.Subtract(this);
		return d.Length();
	}

	inline T DistanceFast(_Vector3<T> *v) {
		_Vector3<T> d(v);
		d.Subtract(this);
		return d.LengthFast();
	}

	inline void Unitize() {
		T length = Length();
		if (length != 0) {
			x /= length; y /= length; z /= length;
		}
	}

	inline void Normalize() {
		Unitize();
	}

	inline void CalculateAngles() {
		T yaw, pitch;

		if ((0 == y) && (0 == x)) {
			yaw = 0;

			if (z > 0) {
				pitch = 90;
			} else {
				pitch = 270;
			}
		} else {
			if (x) {
				yaw = RAD2DEG(atan2(y, x));
			} else if (y > 0) {
				yaw = 90;
			} else {
				yaw = 270;
			}

			if (yaw < 0.0) {
				yaw += 360;
			}

			T forward = sqrt((x * x) + (y * y));

			pitch = RAD2DEG(atan2(z, forward));

			if (pitch < 0.0) {
				pitch += 360;
			}
		}

		x = -pitch;
		y = yaw;
		z = 0;
		Normalize360();
	}

	inline void AngleVectors(_Vector3<T> *f, _Vector3<T> *r, _Vector3<T> *u) {
		T angle, sr, sp, sy, cr, cp, cy;
		
		angle = z * (M_PI * 2 / 360);
		sr    = sin(angle);
		cr    = cos(angle);

		angle = x * (M_PI * 2 / 360);
		sp    = sin(angle);
		cp    = cos(angle);

		angle = y * (M_PI * 2 / 360);
		sy    = sin(angle);
		cy    = cos(angle);

		if (r) {
			r->x = -1 * sr * sp * cy + -1 * cr * -sy;
			r->y = -1 * sr * sp * sy + -1 * cr * cy;
			r->z = -1 * sr * cp;
		}

		if (f) {
			f->x = cp * cy;
			f->y = cp * sy;
			f->z = -sp;
		}

		if (u) {
			u->x = cr * sp * cy + -sr * -sy;
			u->y = cr * sp * sy + -sr * cy;
			u->z = cr * cp;
		}
	}

	inline T &operator[](unsigned int i) {
		switch (i) {
		default:
		case 0:
			return x;

		case 1:
			return y;

		case 2:
			return z;
		}
	}

	inline void Normalize180() {
		x = (T)NormalizeAngle180(x);
		y = (T)NormalizeAngle180(y);
		z = (T)NormalizeAngle180(z);
	}

	inline void Normalize360() {
		x = (T)NormalizeAngle360(x);
		y = (T)NormalizeAngle360(y);
		z = (T)NormalizeAngle360(z);
	}

	inline void NormalizeXY180() {
		x = (T)NormalizeAngle180(x);
		y = (T)NormalizeAngle180(y);
	}

	inline void NormalizeXY360() {
		x = (T)NormalizeAngle360(x);
		y = (T)NormalizeAngle360(y);
	}

	T x, y, z;
};

template <typename T>
class _Vector4 {
public:
	inline _Vector4() { return; }

	inline _Vector4(_Vector4<T> *v) {
		Initialize(v);
	}

	inline _Vector4(unsigned long argb) {
		Initialize(argb);
	}

	inline _Vector4(T _w, T _x, T _y, T _z) {
		Initialize(_w, _x, _y, _z);
	}

	inline bool IsEqual(_Vector4<T> *v) {
		return v[0] == v->v[0] && v[1] == v->v[1] && v[2] == v->v[2] && v[3] == v->v[3];
	}

	inline void ZeroInitialize() {
		v[0] = v[1] = v[2] = v[3] = 0;
	}

	inline void Initialize(unsigned long argb) {
		v[0] = (argb & 0xFF000000) >> 24;
		v[1] = (argb & 0x00FF0000) >> 16;
		v[2] = (argb & 0x0000FF00) >> 8;
		v[3] = (argb & 0x000000FF);
	}

	inline void Initialize(T _w, T _x, T _y, T _z) {
		v[0] = _w; v[1] = _x; v[2] = _y; v[3] = _z;
	}

	inline void Initialize(_Vector4<T> *v) {
		v[0] = v->v[0]; v[1] = v->v[1]; v[2] = v->v[2]; v[3] = v->v[3];
	}

	inline void Add(_Vector4<T> *v) {
		v[0] += v->v[0]; v[1] += v->v[1]; v[2] += v->v[2]; v[3] += v->v[3];
	}

	inline void Subtract(_Vector4<T> *v) {
		v[0] -= v->v[0]; v[1] -= v->v[1]; v[2] -= v->v[2]; v[3] -= v->v[3];
	}

	inline void Multiply(_Vector4<T> *v) {
		v[0] *= v->v[0]; v[1] *= v->v[1]; v[2] *= v->v[2]; v[3] *= v->v[3];
	}

	inline void Scale(T s) {
		v[0] *= s; v[1] *= s; v[2] *= s; v[3] *= s;
	}

	inline void Divide(_Vector4<T> *v) {
		v[0] /= v->v[0]; v[1] /= v->v[1]; v[2] /= v->v[2]; v[3] /= v->v[3];
	}

	inline void Lerp(_Vector4<T> *v, double f) {
		v[0] += f * (v->v[0] - v[0]); y += f * (v->y - y); z += f * (v->z - z);
	}

	inline int GetInt(size_t i) {
		return (int)(v[i] * 255.0f);
	}

	inline unsigned int GetRGBA() {
		return (((unsigned int)(v[0] * 255.0f)) << 24) | (((unsigned int)(v[1] * 255.0f)) << 16) | (((unsigned int)(v[2] * 255.0f)) << 8) | ((unsigned int)(v[3] * 255.0f));
	}

	inline unsigned int GetARGB() {
		return (((unsigned int)(v[3] * 255.0f)) << 24) | (((unsigned int)(v[0] * 255.0f)) << 16) | (((unsigned int)(v[1] * 255.0f)) << 8) | ((unsigned int)(v[2] * 255.0f));
	}

	inline T &operator[](unsigned int i) {
		return v[i];
	}

	T v[4];
};

template <typename T>
class _Matrix {
public:
	inline float *operator[](int i)
	{
		return m[i];
	}

	T m[4][4];
};

class _Matrix3x4 {
public:
	inline float *operator[](int i)
	{
		return m[i];
	}

	float m[3][4];
};

typedef _Vector2<int>    IVector2;
typedef _Vector2<float>  FVector2;
typedef _Vector2<double> DVector2;
typedef _Vector3<float>  FVector3;
typedef _Vector3<double> DVector3;
typedef _Vector4<float>  FVector4;
typedef _Vector4<double> DVector4;
typedef _Matrix<float>   FMatrix;
typedef _Matrix<double>  DMatrix;

#endif

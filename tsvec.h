#pragma once

#include <cstdint>
#include <cstdlib>
#include <cmath>

namespace cm {
    template<uint8_t D>
    struct vec;
    template<uint8_t D>
    vec<D> svec(float v);
    template<uint8_t D>
    struct vec {
        float vals[D];
        vec operator+=(const vec &b) {
            for (uint8_t d = 0; d < D; d++) {
                vals[d] += b.vals[d];
            }
            return *this;
        }
        vec operator-=(const vec &b) {
            for (uint8_t d = 0; d < D; d++) {
                vals[d] -= b.vals[d];
            }
            return *this;
        }
        vec operator+(const vec &b) const {
            vec t = *this;
            return t += b;
        }
        vec operator-(const vec &b) const {
            vec t = *this;
            return t -= b;
        }
        vec operator*=(float b) {
            for (float &val : vals) {
                val *= b;
            }
            return *this;
        }
        vec operator*(float b) const {
            vec t = *this;
            return t *= b;
        }
        vec operator/=(float b) {
            return *this *= (1 / b);
        }
        vec operator/(float b) const {
            vec t = *this;
            return t /= b;
        }
        vec operator*=(const vec &b) {
            for (uint8_t d = 0; d < D; d++) {
                vals[d] *= b.vals[d];
            }
            return *this;
        }
        vec operator*(const vec &b) const {
            vec t = *this;
            return t *= b;
        }
        bool operator==(const vec &b) {
            for (uint8_t d = 0; d < D; d++) {
                if (vals[d] != b.vals[d]) return false;
            }
            return true;
        }
        bool operator!=(const vec &b) {
            return !(*this == b);
        }
        friend float length2(const vec &v) {
            float s = 0.;
            for (float val : v.vals) {
                s += val * val;
            }
            return s;
        }
        float maxon() const {
            float m = -1e20;
            for (uint8_t d = 0; d < D; d++) {
                if (m < vals[d]) m = vals[d];
            }
            return m;
        }
        friend vec<D> abs(const vec<D> &v) {
            vec t;
            for (uint8_t d = 0; d < D; d++) {
                t.vals[d] = fabs(v.vals[d]);
            }
            return t;
        }
        friend float length(const vec<D> &v) {
            return sqrt(length2(v));
        }
        float &operator[](uint8_t i) {
            return vals[i];
        }
        float operator[](uint8_t i) const {
            return vals[i];
        }
    };

    template<uint8_t D>
    vec<D> max(const vec<D> &a, float b) {
        vec<D> ab;
        for (uint8_t d = 0; d < D; d++) {
            ab.vals[d] = std::fmax(a.vals[d], b);
        }
        return ab;
    }
    template<uint8_t D>
    vec<D> svec(float v) {
        vec<D> v_;
        for (uint8_t d = 0; d < D; d++) {
            v_[d] = v;
        }
        return v_;
    }
    template<uint8_t D>
    float dist(float b, vec<D> p0, vec<D> p) {
        p = abs(p - p0) - svec<D>(b);
        return length(max<D>(p, 0)) + std::min(p.maxon(), 0.f);
    }
}
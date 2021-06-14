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
        friend vec abs(const vec &v) {
            vec t;
            for (uint8_t d = 0; d < D; d++) {
                t.vals[d] = abs(v.vals[d]);
            }
            return t;
        }
        friend float length(const vec &v) {
            return sqrt(length2(v));
        }
        friend vec max(const vec &a, float b) {
            vec ab;
            for (uint8_t d = 0; d < D; d++) {
                ab.vals[d] = std::fmax(a.vals[d], b);
            }
            return ab;
        }
        friend float dist(float b, vec p0, vec p) {
            p = abs(p - p0) - svec<D>(b);
            return length(max(p, 0)) + std::fmin(p.maxon(), 0.f);
        }
        float &operator[](uint8_t i) {
            return vals[i];
        }
        float operator[](uint8_t i) const {
            return vals[i];
        }
    };

    template<uint8_t D>
    vec<D> svec(float v) {
        vec<D> v_;
        for (float &val : v_.vals) {
            val = v;
        }
        return v_;
    }
}
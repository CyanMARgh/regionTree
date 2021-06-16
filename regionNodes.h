#include "tsvec.h"
#include <SFML/Graphics.hpp>

namespace cm {
    struct biNodeRefDist {
        void *n;
        float d;
    };
    int cmpNodes(const void *nd0, const void *nd1) {
        float d0 = ((biNodeRefDist *) nd0)->d, d1 = ((biNodeRefDist *) nd1)->d;
        return d0 < d1 ? -1 : d0 > d1 ? 1 : 0;
    }
    template<uint8_t DIM, uint8_t EM, class T>
    struct regionNode {
        uint8_t E = 0;
        regionNode *child[1 << DIM];
        vec<DIM> p0, *v = NULL;
        T *vt = NULL;
        float b;

        regionNode(const vec<DIM> &p0_, float b_) : p0(p0_), b(b_) {
            v = new vec<DIM>[EM];
            vt = new T[EM];
            for (uint8_t i = 0; i < (1 << DIM); i++) {
                child[i] = NULL;
            }
        }
        ~regionNode() {
            for (uint8_t i = 0; i < (1 << DIM); i++) {
                delete child[i];
            }
            delete[] v;
            delete[] vt;
        }

        uint32_t size() const {
            uint32_t s = 1;
            for (uint8_t i = 0; i < (1 << DIM); i++) {
                s += child[i] ? 0 : child[i]->size();
            }
            return s;
        }
        uint32_t count() const {
            if (getState() < 3) {
                return E;
            }
            uint32_t s = 0;
            for (uint8_t i = 0; i < (1 << DIM); i++) {
                s += child[i] ? 0 : child[i]->size();
            }
            return s;
        }

        uint8_t pointToId(const vec<DIM> &p) const {
            uint8_t id = 0;
            for (uint8_t d = 0; d < DIM; d++) {
                id |= (p[d] > p0[d]) << d;
            }
            return id;
        }
        vec<DIM> idToDelta(uint8_t id) const {
            vec<DIM> dp;
            for (uint8_t d = 0; d < DIM; d++) {
                dp[d] = ((id >> d) & 1) ? 1 : -1;
            }
            return dp;
        }
        uint8_t getState() const {
            return v ? (E < EM ? 1 : 2) : 3;
        }

        bool hasSame(const vec<DIM> &p) {
            if (!v) return false;
            for (uint8_t e = 0; e < E; e++) {
                if (v[e] == p)return true;
            }
            return false;
        }
        void split() {
            for (uint8_t e = 0; e < E; e++) {
                add_(v[e], vt[e]);
            }
            delete[] v;
            delete[] vt;
            v = NULL;
            vt = NULL;
        }
        void add_(const vec<DIM> &p, const T& vt_) {
            uint8_t ci = pointToId(p);
            if (!child[ci]) child[ci] = new regionNode(p0 + idToDelta(ci) * b / 2, b / 2);
            child[ci]->add(p, vt_);
        }
        void add(const vec<DIM> &p, const T& vt_) {
            if (hasSame(p)) return;
            uint8_t s = getState();
            if (s == 1) {
                vt[E] = vt_;
                v[E++] = p;
            } else {
                if (s == 2) split();
                add_(p, vt_);
            }
        }
        friend void closest(float &r, T& t, regionNode<DIM, EM, T> *n, vec<DIM> p) {
            if (n) {
                if (n->v) {
                    for (uint8_t e = 0; e < n->E; e++) {
                        float r_ = length(p - (n->v)[e]);
                        if (r_ < r) {
                            r = r_;
                            t = n->vt[e];
                        }
                    }
                    return;
                }
                biNodeRefDist *prefs = new biNodeRefDist[1 << DIM];
                for (uint8_t i = 0; i < 1 << DIM; i++) {
                    prefs[i].n = n->child[i];
                    prefs[i].d = n->child[i] ? dist(n->child[i]->b, n->child[i]->p0, p) : 1e10f;
                }
                std::qsort(prefs, 1 << DIM, sizeof(biNodeRefDist), cmpNodes);
                for (uint8_t i = 0; i < 1 << DIM; i++) {
                    if (r > prefs[i].d) closest(r, t, (regionNode<DIM, EM, T> *) (prefs[i].n), p);
                }
                delete[] prefs;
            }
        }
    };

    template<uint8_t D, uint8_t EM, class T>
    struct regionTree {
        regionNode<D, EM, T> *root = NULL;
        regionTree() {
            if (D == 0 || D > 7) {
                throw std::invalid_argument("incorrect dimension count\n");
            }
            root = new regionNode<D, EM, T>(svec<D>(.5), .5);
        }
        ~regionTree() {
            delete root;
        }
        void add(const vec<D> &p, const T& t) {
            for (uint8_t d = 0; d < D; d++) {
                if (p[d] > 1 || p[d] < 0)return;
            }
            root->add(p, t);
        }
        float distance(const vec<D> &p, T& t) const {
            float r = 1e20;
            closest(r, t, root, p);
            return r;
        }
        uint32_t size() const {
            return root->size();
        }
        uint32_t count() const {
            return root->count();
        }
    };
    typedef regionTree<1, 3, sf::Color> colorBiTree;
    typedef regionTree<2, 3, sf::Color> quadTree;
}

#include "tsvec.h"

namespace cm {
    struct biNodeRefDist {
        void *n;
        float d;
    };
    int cmpNodes(const void *nd0, const void *nd1) {
        float d0 = ((biNodeRefDist *) nd0)->d, d1 = ((biNodeRefDist *) nd1)->d;
        return d0 < d1 ? -1 : d0 > d1 ? 1 : 0;
    }
    template<uint8_t DIM>
    struct regionNode {
        const uint8_t EM = 3;
        uint8_t E = 0;
        regionNode *child[1 << DIM];
        vec<DIM> p0, *v = NULL;
        float b;

        regionNode(const vec<DIM> &p0_, float b_) : p0(p0_), b(b_) {
            v = new vec<DIM>[EM]();
            for (uint8_t i = 0; i < (1 << DIM); i++) {
                child[i] = NULL;
            }
        }
        ~regionNode() {
            for (uint8_t i = 0; i < (1 << DIM); i++) {
                delete child[i];
            }
            delete[] v;
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
                add_(v[e]);
            }
            delete[] v;
            v = NULL;
        }
        void add_(const vec<DIM> &p) {
            uint8_t ci = pointToId(p);
            if (!child[ci]) child[ci] = new regionNode(p0 + idToDelta(ci) * b / 2, b / 2);
            child[ci]->add(p);
        }
        void add(const vec<DIM> &p) {
            if (hasSame(p)) return;
            uint8_t s = getState();
            if (s == 1) {
                v[E++] = p;
            } else {
                if (s == 2) split();
                add_(p);
            }
        }

        friend void closest(float &r, regionNode *n, vec<DIM> p) {
            if (n) {
                if (n->v) {
                    for (uint8_t e = 0; e < n->E; e++) {
                        r = std::min(r, length(p - (n->v)[e]));
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
                    if (r > prefs[i].d) closest(r, (regionNode *) (prefs[i].n), p);
                }
                delete[] prefs;
            }
        }
    };

    template<uint8_t D>
    struct regionTree {
        regionNode<D> *root = NULL;
        regionTree() {
            if (D == 0 || D > 7) {
                throw std::invalid_argument("incorrect dimension count\n");
            }
            root = new regionNode<D>(svec<D>(.5), .5);
        }
        ~regionTree() {
            delete root;
        }
        void add(const vec<D> &p) {
            for (uint8_t d = 0; d < D; d++) {
                if (p[d] > 1 || p[d] < 0)return;
            }
            root->add(p);
        }
        float distance(const vec<D> &p) const {
            float r = 1e20;
            closest(r, root, p);
            return r;
        }
        uint32_t size() const {
            return root->size();
        }
        uint32_t count() const {
            return root->count();
        }
    };
    typedef regionTree<1> biTree;
    typedef regionTree<2> quadTree;
    typedef regionTree<3> octTree;
}

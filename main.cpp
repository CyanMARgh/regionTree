#include <iostream>
#include <SFML/Graphics.hpp>
#include "regionNodes.h"

void printDistanceField(sf::RenderWindow &win, cm::biTree &t) {
    sf::Vector2u s = win.getSize();
    sf::Image img;
    img.create(s.x, s.y);
    auto *img_ = (uint32_t *) img.getPixelsPtr();
    int d = 0;
    for (uint32_t x = 0; x < s.x; x++) {
        d = (int) (t.distance({(float) x / (float) s.x}) * (float) s.y);
        d = d < 0 ? 0 : d >= s.y ? s.y - 1 : d;
        img_[(s.y - 1 - d) * s.x + x] = 0xffffffff;
    }
    sf::Texture tex;
    tex.create(s.x, s.y);
    tex.loadFromImage(img);
    sf::Sprite spr(tex);
    win.draw(spr);
}
void printBox(sf::RenderWindow &win, cm::vec<2> p0, cm::vec<2> b) {
    sf::Vector2u ws = win.getSize();
    sf::Vector2f p0_ = {p0[0] * (float) ws.x, p0[1] * (float) ws.y};
    sf::Vector2f b_ = {b[0] * (float) ws.x, b[1] * (float) ws.y};
    const float bs = 2;
    sf::RectangleShape square(sf::Vector2f((b_.x - bs) * 2, (b_.y - bs) * 2));
    square.setOrigin(b_.x - bs, b_.y - bs);
    square.setPosition(p0_);
    square.setFillColor(sf::Color::Black);
    square.setOutlineThickness(bs);
    square.setOutlineColor(sf::Color::White);
    win.draw(square);
}
void printPoint(sf::RenderWindow &win, cm::vec<2> p, float s = 2, sf::Color color = sf::Color::Red) {
    sf::Vector2u ws = win.getSize();
    sf::CircleShape circle(s);
    circle.setFillColor(color);
    circle.setOrigin(s, s);
    circle.setPosition((float)ws.x * p[0], (float)ws.y * p[1]);
    win.draw(circle);
}
void printQuadNode(sf::RenderWindow &win, cm::regionNode<2> *n, uint8_t L = 0) {
    if (n) {
        printBox(win, n->p0, cm::svec<2>(n->b));
        if (n->v) {
            for (uint8_t e = 0; e < n->E; e++) {
                printPoint(win, (n->v)[e]);
            }
        }
        for (auto &i:n->child) {
            printQuadNode(win, i, L + 1);
        }
    }
}
void printClosestCircle(sf::RenderWindow &win, cm::quadTree &t, sf::Vector2i mpos, cm::vec<2> mpos_) {
    float distance = t.distance(mpos_);
    distance = distance * (float)win.getSize().x;
    printPoint(win, mpos_, distance, sf::Color::Blue);
};
void printQuadTree(sf::RenderWindow &win, cm::quadTree &t) {
    printQuadNode(win, t.root);
}

//#define MODE1D
int main() {
    const uint16_t S = 900;
#ifdef MODE1D
    cm::biTree t;
#else
    cm::quadTree t;
#endif
    sf::RenderWindow mainWindow(sf::VideoMode(S, S), "region tree test");

    while (mainWindow.isOpen()) {
        sf::Event event;
        sf::Vector2i mpos;
        cm::vec<2> mpos_;
        while (mainWindow.pollEvent(event)) {
            mpos = sf::Mouse::getPosition(mainWindow);
            mpos_ = {(float)mpos.x + (float) rand() / RAND_MAX, (float)mpos.y + (float) rand() / RAND_MAX};
            mpos_ /= S;
            switch (event.type) {
                case sf::Event::Closed : {
                    mainWindow.close();
                    break;
                }
                case sf::Event::MouseButtonPressed : {
#ifdef MODE1D
                    t.add(mpos_[0]);
#else
                    t.add(mpos_);
#endif
                }
            }
        }
        mainWindow.clear();
#ifdef MODE1D
        printDistanceField(mainWindow, t);
#else
        printQuadTree(mainWindow, t);
        printClosestCircle(mainWindow, t, mpos, mpos_);
#endif
        mainWindow.display();
    }
    return 0;
}

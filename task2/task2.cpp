#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <omp.h>

#define HEIGHT 10
#define WIDTH 10

using namespace std;

struct point {
    bool is_live;
};

void init_world(point world[][HEIGHT]) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 1);

    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            world[i][j].is_live = dis(gen);
        }
    }
}

void print_world(point world[][HEIGHT]) {
    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            cout <<(world[i][j].is_live ? '*' : ' ') << ' ';
        }
        cout << endl;
    }
}

unsigned int get_live_count(point world[][HEIGHT]) {
    unsigned int count = 0;
    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            count += world[i][j].is_live;
        }
    }
    return count;
}

void next_generation(point world[][HEIGHT], point prev_world[][HEIGHT]) {
#pragma omp parallel for
    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            unsigned int live_nb = 0;
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    if (x == 0 && y == 0) continue;
                    int _x = i + x;
                    int _y = j + y;
                    if (_x >= 0 && _x < WIDTH && _y >= 0 && _y < HEIGHT) {
                        live_nb += prev_world[_x][_y].is_live;
                    }
                }
            }
            if (prev_world[i][j].is_live) {
                world[i][j].is_live = live_nb == 2 || live_nb == 3;
            }
            else {
                world[i][j].is_live = live_nb == 3;
            }
        }
    }
}

void copy_world(point src[][HEIGHT], point dest[][HEIGHT]) {
    for (unsigned int i = 0; i < WIDTH; ++i) {
        for (unsigned int j = 0; j < HEIGHT; ++j) {
            dest[i][j] = src[i][j];
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    omp_set_num_threads(5);
    point world[WIDTH][HEIGHT];
    point prev_world[WIDTH][HEIGHT];

    init_world(world);
    unsigned int live_points = -1;

    do {
        print_world(world);
        cout << "\nновое поколение: \n" << endl;
        copy_world(world, prev_world);
        next_generation(world, prev_world);
        live_points = get_live_count(world);

        if (live_points == 0) {
            cout << "живых не осталось" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    } while (live_points != 0);

    return 0;
}

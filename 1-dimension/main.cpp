#include <cmath>
#include <iostream>
#include <string>
#include <vector>

struct Display {
    int size;
    std::vector<char> display;

    Display(int size) {
        this->size = size;
        display.assign(size, '.');
    }

    void print() {
        for(auto pixel : display) {
            std::cout << pixel;
        }
        std::cout << '\n';
    }
};

struct Point {
    double x;

    Point(double x) {
        this->x = x;
    }

    void draw_pixel(Display &display, char texture) {
        if(0 <= x && x <= display.size - 1){
            display.display[round(x)] = texture;
        }
    }

    void translate(double distance) {
        x += distance;
    }
};

struct LineSegment {
    double a;
    double b;

    LineSegment(double a, double b) {
        this->a = a;
        this->b = b;
    }

    void draw_pixel(Display &display, char texture) {
        int lower_bound = round(a);
        int upper_bound = round(b);

        for(int i = lower_bound; i <= upper_bound; i++) {
            if(0 <= i && i <= display.size - 1) {
                display.display[i] = texture;
            }
        }
    }

    void translate(double distance) {
        a += distance;
        b += distance;
    }

    void scale(double factor) {
        a *= factor;
        b *= factor;
    }
};

struct Object {
    std::string type;
    char texture;
    void *obj;
};

void rasterize(Display &display, const std::vector<Object> &object_list);

int main() {
    Display display(20);

    std::vector<Object> object_list;

    LineSegment line_1(1, 5), line_2(8, 10);
    Point point_1(15.67);
    
    object_list.push_back({"line", '0', &line_1});
    object_list.push_back({"line", '&', &line_2});
    object_list.push_back({"point", '$', &point_1});

    rasterize(display, object_list);

    display.print();

    std::cout << '\n';

    return 0;
}

void rasterize(Display &display, const std::vector<Object> &object_list) {
    for(auto obj : object_list) {
        if(obj.type == "line") {
            ((LineSegment *)obj.obj)->draw_pixel(display, obj.texture);
        } else if(obj.type == "point") {
            ((Point *)obj.obj)->draw_pixel(display, obj.texture);
        }
    }
}

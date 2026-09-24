#include <cmath>
#include <iostream>
#include <vector>

enum class ObjType {
    Point,
    LineSegment,
};

struct Display {
    int size;
    std::vector<char> display;

    Display(int size) : size(size), display(size, '.') {}

    void draw_pixel(int index, char texture) {
        if(0 <= index && index < size) {
            display[index] = texture;
        }
    }

    void print() {
        for(auto pixel : display) {
            std::cout << pixel;
        }
        std::cout << '\n';
    }

    void clear() {
        for(auto &pixel : display) {
            pixel = '.';
        }
    }
};

struct Point {
    double x;

    Point(double x) : x(x) {}

    void translate(double distance) {
        x += distance;
    }
};

struct LineSegment {
    double a;
    double b;

    LineSegment(double a, double b) : a(a), b(b) {}

    void translate(double distance) {
        a += distance;
        b += distance;
    }

    void scale(double factor) {
        double center = (a + b) / 2.0;
        double half_length = (b - a) / 2.0 * factor;
        a = center - half_length;
        b = center + half_length;
    }
};

struct Object {
    ObjType type;
    char texture;
    void *object;
};

void rasterize(Display &display, const std::vector<Object> &object_list) {
    for(const auto &obj : object_list) {
        if(obj.object == nullptr) continue;

        if(obj.type == ObjType::Point) {
            Point *point = static_cast<Point*>(obj.object);
            int pixel_idx = round(point->x);
            display.draw_pixel(pixel_idx, obj.texture);
        } else if(obj.type == ObjType::LineSegment) {
            LineSegment *line = static_cast<LineSegment*>(obj.object);
            
            double start = (line->a < line->b) ? line->a : line->b;
            double end = (line->a < line->b) ? line->b : line->a;

            int lower_bound = round(start);
            int upper_bound = round(end);

            for(int i = lower_bound; i <= upper_bound; i++) {
                display.draw_pixel(i, obj.texture);
            }
        }
    }
}

Object create_point(double x, char texture) {
    Point *point = new Point(x);

    Object obj = {
        ObjType::Point,
        texture,
        static_cast<void*>(point),
    };

    return obj;
}

Object create_line_segment(double a, double b, char texture) {
    LineSegment *line = new LineSegment(a, b);

    Object obj = {
        ObjType::LineSegment,
        texture,
        static_cast<void*>(line)
    };

    return obj;
}

void delete_object(Object &obj) {
    if(obj.object == nullptr) return;

    if(obj.type == ObjType::Point) {
        Point *point = static_cast<Point*>(obj.object);
        delete point;
    } else if(obj.type == ObjType::LineSegment) {
        LineSegment *line = static_cast<LineSegment*>(obj.object);
        delete line;
    }

    obj.object = nullptr;
}

void cleanup_dead_objects(std::vector<Object> &object_list) {
    for(auto it = object_list.begin(); it != object_list.end(); ) {
        if(it->object == nullptr) {
            object_list.erase(it);
        } else {
            it++;
        }
    } 
}

int main() {
    Display display(20);

    std::vector<Object> object_list;

    object_list.push_back(create_line_segment(3, 7, '%'));
    object_list.push_back(create_line_segment(10, 10, '$'));

    rasterize(display, object_list);

    display.print();

    display.clear();

    cleanup_dead_objects(object_list);

    std::cout << object_list.size() << '\n';

    delete_object(object_list[0]);

    rasterize(display, object_list);

    display.print();

    display.clear();

    cleanup_dead_objects(object_list);

    std::cout << object_list.size() << '\n';

    std::cout << '\n';

    for(auto obj : object_list) {
        delete_object(obj);
    }
    object_list.clear();

    return 0;
}


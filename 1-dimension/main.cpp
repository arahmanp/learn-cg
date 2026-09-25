#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class ObjType {
    Point,
    LineSegment,
};

struct Display {
    int size;
    char background;
    std::vector<char> display;

    Display(int size, char background = ' ') : size(size), background(background), display(size, background) {}

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
            pixel = background;
        }
    }
};

struct Point {
    double x;

    Point(double x) : x(x) {}

    void g_translate(double distance) {
        x += distance;
    }
};

struct LineSegment {
    double a;
    double b;

    LineSegment(double a, double b) : a(a), b(b) {}

    void g_translate(double distance) {
        a += distance;
        b += distance;
    }

    void g_scale(double factor) {
        double center = (a + b) / 2.0;
        double half_length = (b - a) / 2.0 * factor;
        a = center - half_length;
        b = center + half_length;
    }
};

struct Object {
    std::string name;
    ObjType type;
    char texture;
    void *object;

    void translate(double distance) {
        switch (type) {
            case ObjType::Point:
                static_cast<Point*>(object)->g_translate(distance);
                break;
            
            case ObjType::LineSegment:
                static_cast<LineSegment*>(object)->g_translate(distance);
                break;
        }
    }

    void scale(double factor) {
        switch (type) {
            case ObjType::Point:
                return;
            
            case ObjType::LineSegment:
                static_cast<LineSegment*>(object)->g_scale(factor);
                break;
        }
    }
};

struct ObjectList {
    int size;
    std::unordered_map<std::string, int> name_to_index;
    std::vector<Object> object_list;

    ObjectList() : size(0) {}

    Object& get_object(std::string object_name) {
        return object_list.at(name_to_index.at(object_name));
    }

    void add_object(Object object) {
        object_list.push_back(object);
        name_to_index[object.name] = size;
        size++;
    }

    void delete_object(std::string object_name) {
        if(!name_to_index.contains(object_name)) return;

        int object_index = name_to_index[object_name];
        Object &deleted_object = object_list[object_index];

        if(deleted_object.object == nullptr) return;

        switch (deleted_object.type) {
            case ObjType::Point: {
                Point *point = static_cast<Point*>(deleted_object.object);
                delete point;
                break;
            }

            case ObjType::LineSegment: {
                LineSegment *line = static_cast<LineSegment*>(deleted_object.object);
                delete line;
                break;
            }
        }

        deleted_object.object = nullptr;
    }

    void cleanup_dead_objects() {
        size_t i = 0;
        while(i < object_list.size()) {
            if(object_list[i].object == nullptr) {
                name_to_index.erase(object_list[i].name);

                if(i != object_list.size() - 1) {
                    object_list[i] = std::move(object_list.back());

                    name_to_index[object_list[i].name] = i;
                }

                object_list.pop_back();

                size--;
            } else {
                i++;
            }
        }
    }
};

void rasterize(Display &display, const ObjectList &object_list) {
    for(const auto &obj : object_list.object_list) {
        if(obj.object == nullptr) continue;

        switch (obj.type) {
            case ObjType::Point: {
                Point *point = static_cast<Point*>(obj.object);
                int pixel_idx = round(point->x);
                display.draw_pixel(pixel_idx, obj.texture);

                break;
            }

            case ObjType::LineSegment: {
                LineSegment *line = static_cast<LineSegment*>(obj.object);
            
                double start = (line->a < line->b) ? line->a : line->b;
                double end = (line->a < line->b) ? line->b : line->a;

                int lower_bound = round(start);
                int upper_bound = round(end);

                for(int i = lower_bound; i <= upper_bound; i++) {
                    display.draw_pixel(i, obj.texture);
                }

                break;
            }
        }
    }
}

Object create_point(std::string name, double x, char texture) {
    Point *point = new Point(x);

    Object obj = {
        name,
        ObjType::Point,
        texture,
        static_cast<void*>(point),
    };

    return obj;
}

Object create_line_segment(std::string name, double a, double b, char texture) {
    LineSegment *line = new LineSegment(a, b);

    Object obj = {
        name,
        ObjType::LineSegment,
        texture,
        static_cast<void*>(line)
    };

    return obj;
}

int main() {
    //

    return 0;

}


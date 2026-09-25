#include <cmath>
#include <iostream>
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

    // Initialize the display and object_list objects
    Display display(20, '.');
    std::vector<Object> object_list;

    // ===========================================
    //  1st Render
    // ===========================================

    // 1. Object modification (here we create a new point at position 2)
    object_list.push_back(create_point(2, '@'));

    // 2. Rasterization (draw objects to the Display)
    rasterize(display, object_list);

    // 3. Display the objects on the screen
    display.print();

    // 4. Call `cleanup_dead_objects` to clear the vector
    cleanup_dead_objects(object_list);

    // 5. Reset the screen buffer to '.'
    display.clear();


    // ===========================================
    //  2nd Render
    // ===========================================

    // 1. Object modification:
    //    - Delete point at position 2
    //    - Create two line segments at position [1, 5] and [14, 16]
    //    - Create a new point at position 8
    delete_object(object_list[0]);
    object_list.push_back(create_line_segment(1, 5, '#'));
    object_list.push_back(create_line_segment(14, 16, '&'));
    object_list.push_back(create_point(8, '*'));

    // 2. Rasterization (draw objects to the Display)
    rasterize(display, object_list);

    // 3. Display the objects on the screen
    display.print();

    // 4. Call `cleanup_dead_objects` to clear the vector
    cleanup_dead_objects(object_list);

    // 5. Reset the screen buffer to '.'
    display.clear();


    // ===========================================
    //  3rd Render
    // ===========================================

    // 1. Object modification:
    //    - Delete line segment at position [1, 5]
    //    - Move point at position 8, 5 step to the left
    //    - Scale the line segment at position [14, 16] by a scale factor of 3.5
    delete_object(object_list[0]);
    object_list[2].translate(-5);
    object_list[1].scale(3.5);

    // 2. Rasterization (draw objects to the Display)
    rasterize(display, object_list);

    // 3. Display the objects on the screen
    display.print();

    // 4. Call `cleanup_dead_objects` to clear the vector
    cleanup_dead_objects(object_list);

    // 5. Reset the screen buffer to '.'
    display.clear();


    std::cout << '\n';

    // Deallocate all remaining objects in `object_list`
    for(auto obj : object_list) {
        delete_object(obj);
    }

    // Clear the ‘object_list’ vector
    object_list.clear();

    return 0;

}


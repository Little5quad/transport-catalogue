#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <optional>
#include <variant>

namespace svg {
    
struct Rgb{
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b){
    }
    Rgb& operator= (Rgb rhs){
        this->red = rhs.red;
        this->green = rhs.green;
        this->blue = rhs.blue;
        return *this;
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};
std::ostream& operator<<(std::ostream& out, Rgb rgb);
    
struct Rgba{
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double o) : red(r), green(g), blue(b), opacity(o) {
    }
    Rgba& operator= (Rgba rhs){
        this->red = rhs.red;
        this->green = rhs.green;
        this->blue = rhs.blue;
        this->opacity = rhs.opacity;
        return *this;
    }
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};
    
using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

struct ColorPrinter {
      std::ostream& out;
      void operator()(std::monostate) const {
           out << "none";
      }
      void operator()(std::string color) const {
            out << color;
      }
      void operator()(Rgb rgb) const {
         out << "rgb(" << static_cast<int>(rgb.red) << "," << static_cast<int>(rgb.green) << "," << static_cast<int>(rgb.blue) << ")";
     }
      void operator()(Rgba rgba) const {
         out << "rgba(" << static_cast<int>(rgba.red) << "," << static_cast<int>(rgba.green) << "," << static_cast<int>(rgba.blue) << "," << rgba.opacity << ")";
     }
};

std::ostream& operator<<(std::ostream& out, Color color);
// Объявив в заголовочном файле константу со спецификатором inline,
// мы сделаем так, что она будет одной на все единицы трансляции,
// которые подключают этот заголовок.
// В противном случае каждая единица трансляции будет использовать свою копию этой константы
inline const Color NoneColor{"none"};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};
std::ostream& operator<<(std::ostream& out, StrokeLineCap str);

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};
 std::ostream& operator<<(std::ostream& out, StrokeLineJoin str);

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    
    
    Owner& SetStrokeLineJoin(StrokeLineJoin s){
        stroke_line_join_ = s;
        return AsOwner();
    }
    
    
    Owner& SetStrokeLineCap(StrokeLineCap s){
        stroke_line_cap_ = s;
        return AsOwner();
    }
    
    Owner& SetStrokeWidth(double width){
        stroke_width_ = width;
        return AsOwner();
    }
protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        //stroke-width="3" stroke-linecap="round" stroke-linejoin="round"
        if (stroke_width_){
            out << " stroke-width=\""sv << *stroke_width_ <<"\""sv;
        }
        if (stroke_line_cap_){
            out << " stroke-linecap=\""sv << *stroke_line_cap_ <<"\""sv;
        }
        if (stroke_line_join_){
            out << " stroke-linejoin=\""sv << *stroke_line_join_<<"\""sv;
        }
    }

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
};
    
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline>  {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
private:
    void RenderObject(const RenderContext& context) const override;
    std::vector<Point> pics_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final: public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);
    
    
    // Прочие данные и методы, необходимые для реализации элемента <text>
private:
    Point pos_;
    Point offset_;
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
    std::string ConvectSymbols(const std::string& str) const;
    void RenderObject(const RenderContext& context) const override;
};
    
class ObjectContainer{
public:
    template <typename Obj>
    void Add(Obj obj) {
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    }
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;  
protected:
    std::deque<std::unique_ptr<Object>> objects_;
}; 
    
class Drawable{
public:
    virtual ~Drawable() = default;
    virtual void Draw(ObjectContainer& obj) const = 0;
};
    
class Document : public ObjectContainer{
public:
    /*
     Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
     Пример использования:
     Document doc;
     doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
    */
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    // Прочие методы и данные, необходимые для реализации класса Document
    void Clear();
};

}  // namespace svg
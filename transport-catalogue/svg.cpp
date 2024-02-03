#include "svg.h"

namespace svg {

using namespace std::literals;

std::ostream& operator<<(std::ostream& out, Rgb rgb){
    out << "rgb(" << rgb.red <<"," <<rgb.green<<","<<rgb.blue<<")";
    return out;
}
    
std::ostream& operator<<(std::ostream& out, Color color){
    std::visit(ColorPrinter{out}, color);
    return out;
}
    
std::ostream& operator<<(std::ostream& out, StrokeLineCap str){
    switch (str){
        case StrokeLineCap::BUTT : out << "butt"; break;
        case StrokeLineCap::ROUND : out << "round"; break;
        case StrokeLineCap::SQUARE : out << "square"; break;
    }
    return out;
} 
 
 std::ostream& operator<<(std::ostream& out, StrokeLineJoin str){
    switch (str){
        case StrokeLineJoin::ARCS : out << "arcs"; break;
        case StrokeLineJoin::ROUND : out << "round"; break;
        case StrokeLineJoin::BEVEL : out << "bevel"; break;
        case StrokeLineJoin::MITER : out << "miter"; break;
        case StrokeLineJoin::MITER_CLIP : out << "miter-clip"; break;
    }
    return out;
}  
    
void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

//----------POLYLINE-------------
    
 Polyline&  Polyline::AddPoint(Point point){
     pics_.push_back(point);
     return *this;
 }
/*<polyline points="20,40 22.9389,45.9549 29.5106,46.9098" /> */
void  Polyline::RenderObject(const RenderContext& context) const{
    bool is_f = true;
    auto& out = context.out;
    out << "<polyline points="sv<<'"';
    for (const Point& p : pics_){
        if (is_f){
            out<<p.x<<","sv<<p.y;
            is_f = false;
        }else{
            out<<" "sv<<p.x<<","sv<<p.y;
        }
    }
    out<<'"';
    RenderAttrs(out);
    out<<"/>"sv;    
}

    
//-------------text-------------------

Text& Text::SetPosition(Point pos){
    pos_ = pos;
    return *this;
}

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset){
    offset_ = offset;
    return *this;
}

    // Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size){
    size_ = size;
    return *this;
}

    // Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family){
    font_family_ = std::move(font_family);
    return *this;
}

    // Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight){
    font_weight_ = font_weight;
    return *this;
}

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data){
    data_ = data;
    return *this;
}
    
std::string Text::ConvectSymbols(const std::string& str) const {
    std::string res;

    for(auto ch : str) {
        if(ch == '"') {
            res += "&quot;";
        }
        else if(ch == '\'') {
            res += "&apos;";
        }
        else if(ch == '<') {
            res += "&lt;";
        }
        else if(ch == '>') {
            res += "&gt;";
        }
        else if(ch == '&') {
            res += "&amp;";
        }
        else {
            res += ch;
        }
    }

    if(res.empty()) {
        return res;
    }

    auto first = res.find_first_not_of(' ');
    auto last = res.find_last_not_of(' ');

    return res.substr(first, last - first + 1);
}
//<text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>    
void Text::RenderObject(const RenderContext& context) const{
    auto& out = context.out;
    out << "<text";
    RenderAttrs(out);
    out<<" x=\"" << pos_.x << "\" y=\"" << pos_.y;
    out << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\"";
    out << " font-size=\"" << size_ << "\"";
    if (!font_family_.empty()){
        out << " font-family=\""sv << font_family_ << "\""sv;
    }
    if (!font_weight_.empty()){
        out << " font-weight=" <<'"'<<font_weight_<<"\""sv;
    }
    out<<">";
    out << ConvectSymbols(data_) << "</text>";
}
    
//-----------------DOCUMENT----------------
    
// Добавляет в svg-документ объект-наследник svg::Object
void Document::AddPtr(std::unique_ptr<Object>&& obj){
    objects_.emplace_back(std::move(obj));
}

    // Выводит в ostream svg-представление документа
void Document::Render(std::ostream& out) const{
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";

    for(auto& obj : objects_) {
        obj->Render(RenderContext({out, 2, 2}));
    }
    out << "</svg>";
}
    void Document::Clear() {
		objects_.clear();
	}
}  // namespace svg

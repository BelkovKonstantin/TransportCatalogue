#include "svg.h"

namespace svg
{

	using namespace std::literals;

	std::ostream &operator<<(std::ostream &out, StrokeLineCap cap)
	{
		switch (cap)
		{
		case (StrokeLineCap::BUTT):
			out << "butt";
			break;
		case (StrokeLineCap::ROUND):
			out << "round";
			break;
		case (StrokeLineCap::SQUARE):
			out << "square";
			break;
		}
		return out;
	}

	std::ostream &operator<<(std::ostream &out, StrokeLineJoin join)
	{
		switch (join)
		{
		case (StrokeLineJoin::ARCS):
			out << "arcs";
			break;
		case (StrokeLineJoin::BEVEL):
			out << "bevel";
			break;
		case (StrokeLineJoin::MITER):
			out << "miter";
			break;
		case (StrokeLineJoin::MITER_CLIP):
			out << "miter-clip";
			break;
		case (StrokeLineJoin::ROUND):
			out << "round";
			break;
		}
		return out;
	}

	//Перегрузка оператора вывода для разных способов передачи цвета
	std::ostream &operator<<(std::ostream &out, Color color)
	{
		std::visit(OstreamColorPrinter{out}, color);
		return out;
	}

	// Выводит в ostream svg-представление
	void Object::Render(const RenderContext &context) const
	{
		context.RenderIndent();

		// Делегируем вывод тега своим подклассам
		RenderObject(context);

		context.out << std::endl;
	}

	// ---------- Circle ------------------

	Circle &Circle::SetCenter(Point center)
	{
		center_ = center;
		return *this;
	}

	Circle &Circle::SetRadius(double radius)
	{
		radius_ = radius;
		return *this;
	}

	// Выводит в ostream svg-представление круга
	void Circle::RenderObject(const RenderContext &context) const
	{
		auto &out = context.out;
		out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
		out << "r=\""sv << radius_ << "\""sv;
		RenderAttrs(context.out);
		out << "/>"sv;
	}

	Polyline &Polyline::AddPoint(Point point)
	{
		points.push_back(point);
		++size_;
		return *this;
	}

	// Выводит в ostream svg-представление ломаной линии
	void Polyline::RenderObject(const RenderContext &context) const
	{
		auto &out = context.out;
		int count = 0;
		out << "<polyline points=\"";
		if (size_ != 0)
		{
			for (auto point : points)
			{
				if (count > 0)
					out << " "sv;
				out << point.x << ","sv << point.y;
				++count;
			}
		}
		out << "\""sv;
		RenderAttrs(context.out);
		out << "/>"sv;
	}

	Text &Text::SetPosition(Point pos)
	{
		position_ = pos;
		return *this;
	}

	Text &Text::SetOffset(Point offset)
	{
		offset_ = offset;
		return *this;
	}

	Text &Text::SetFontSize(uint32_t size)
	{
		size_ = size;
		return *this;
	}

	Text &Text::SetFontFamily(std::string font_family)
	{
		font_family_ = font_family;
		return *this;
	}

	Text &Text::SetFontWeight(std::string font_weight)
	{
		font_weight_ = font_weight;
		return *this;
	}

	// Задаёт текстовое содержимое объекта (отображается внутри тега text)
	Text &Text::SetData(std::string data)
	{
		std::string temp = "\0";
		for (auto ch : data)
		{
			if ((ch == '\"') || (ch == '\'') || (ch == '<') || (ch == '&') || (ch == '>'))
			{
				if (ch == '"')
					temp += "&quot;";
				if (ch == '\'')
					temp += "&apos;";
				if (ch == '<')
					temp += "&lt;";
				if (ch == '>')
					temp += "&gt;";
				if (ch == '&')
					temp += "&amp;";
			}
			else
				temp += ch;
		}
		data_ = temp;
		return *this;
	}

	// Выводит в ostream svg-представление текста
	void Text::RenderObject(const RenderContext &context) const
	{
		auto &out = context.out;
		out << "<text";
		RenderAttrs(context.out);
		out << " x=\""sv << position_.x << "\""sv;
		out << " y=\""sv << position_.y << "\""sv;
		out << " dx=\""sv << offset_.x << "\""sv;
		out << " dy=\""sv << offset_.y << "\""sv;
		out << " font-size=\""sv << size_ << "\""sv;
		if (font_family_.has_value())
			out << " font-family=\""sv << font_family_.value() << "\""sv;
		if (font_weight_.has_value())
			out << " font-weight=\""sv << font_weight_.value() << "\""sv;
		out << ">"sv << data_ << "</text>"sv;
	}

	// Добавляет в svg-документ объект-наследник svg::Object
	void Document::AddPtr(std::unique_ptr<Object> &&obj)
	{
		objects_.push_back({std::move(obj)});
	}

	// Выводит в ostream svg-представление документа
	void Document::Render(std::ostream &out) const
	{
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
		out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
		for (auto &&object : objects_)
		{
			out << "  ";
			(*object).Render(out);
		}
		out << "</svg>";
		;
	}

} // namespace svg

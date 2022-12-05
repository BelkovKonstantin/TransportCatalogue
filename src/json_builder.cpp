#include "json_builder.h"

namespace json
{
	// Ошибка при невероятном случае обхода неправильной постройки на этапе компиляции
	void Builder::BadTry()
	{
		throw std::logic_error("Ne posledovatelno");
	}

	KeyItemContext &Builder::Key(std::string key)
	{
		if (last_start.empty())
			BadTry();
		else if (last_start.back() != "Dict")
			BadTry();
		else if (last_key_.has_value())
			BadTry();

		last_key_ = key;
		return *this;
	}

	Builder &Builder::Value(Node::Value value)
	{
		if (root_.has_value())
		{
			if (!last_key_.has_value())
			{
				if (last_start.empty())
					BadTry();
				if (last_start.back() != "Array")
					BadTry();
			}
		}

		if (!json_doc_.empty())
		{
			if (last_start.back() == "Array")
			{
				std::get<Array>(json_doc_.back()->GetValueForChange()).emplace_back(Node{});
				std::get<Array>(json_doc_.back()->GetValueForChange()).back().GetValueForChange() = value;
			}
			else
			{
				std::get<Dict>(json_doc_.back()->GetValueForChange())[last_key_.value()].GetValueForChange() =
					value;
				last_key_.reset();
			}
		}
		else
		{
			if (last_start.size() == 0)
			{
				root_ = Node{};
				root_.value().GetValueForChange() = value;
			}
			else if (last_start.back() == "Array")
			{
				std::get<Array>(root_.value().GetValueForChange()).emplace_back(Node{});
				std::get<Array>(root_.value().GetValueForChange()).back().GetValueForChange() = value;
			}
			else
			{
				std::get<Dict>(root_.value().GetValueForChange())[last_key_.value()].GetValueForChange() =
					value;
				last_key_.reset();
			}
		}
		return *this;
	}

	DictItemContext &Builder::StartDict()
	{
		if (root_.has_value())
		{
			if (!last_key_.has_value())
			{
				if (last_start.empty())
					BadTry();
				if (last_start.back() != "Array")
					BadTry();
			}
		}

		if (!root_.has_value())
		{
			root_ = Node(Dict{});
		}
		else
		{
			if (json_doc_.empty())
			{
				if (last_key_.has_value())
				{
					std::get<Dict>(root_.value().GetValueForChange())[last_key_.value()].GetValueForChange() =
						Dict{};
					json_doc_.emplace_back(
						&std::get<Dict>(root_.value().GetValueForChange()).at(last_key_.value()));
					last_key_.reset();
				}
				else
				{
					std::get<Array>(root_.value().GetValueForChange()).emplace_back(Dict{});
					json_doc_.emplace_back(&std::get<Array>(root_.value().GetValueForChange()).back());
				}
			}
			else
			{
				if (last_key_.has_value())
				{
					std::get<Dict>(json_doc_.back()->GetValueForChange())[last_key_.value()].GetValueForChange() =
						Dict{};
					json_doc_.emplace_back(
						&std::get<Dict>(json_doc_.back()->GetValueForChange()).at(last_key_.value()));
					last_key_.reset();
				}
				else
				{
					std::get<Array>(json_doc_.back()->GetValueForChange()).emplace_back(Dict{});
					json_doc_.emplace_back(&std::get<Array>(json_doc_.back()->GetValueForChange()).back());
				}
			}
		}

		last_start.emplace_back("Dict");
		return *this;
	}
	ArrayItemContext &Builder::StartArray()
	{
		if (root_.has_value())
		{
			if (!last_key_.has_value())
			{
				if (last_start.empty())
					BadTry();
				if (last_start.back() != "Array")
					BadTry();
			}
		}

		if (!root_)
		{
			root_ = Node(Array{});
		}
		else
		{
			if (json_doc_.empty())
			{
				if (last_key_.has_value())
				{
					std::get<Dict>(root_.value().GetValueForChange())[last_key_.value()].GetValueForChange() =
						Array{};
					json_doc_.emplace_back(
						&std::get<Dict>(root_.value().GetValueForChange()).at(last_key_.value()));
					last_key_.reset();
				}
				else
				{
					std::get<Array>(root_.value().GetValueForChange()).emplace_back(Array{});
					json_doc_.emplace_back(&std::get<Array>(root_.value().GetValueForChange()).back());
				}
			}
			else
			{
				if (last_key_.has_value())
				{
					std::get<Dict>(json_doc_.back()->GetValueForChange())[last_key_.value()].GetValueForChange() =
						Array{};
					json_doc_.emplace_back(
						&std::get<Dict>(json_doc_.back()->GetValueForChange()).at(last_key_.value()));
					last_key_.reset();
				}
				else
				{
					std::get<Array>(json_doc_.back()->GetValueForChange()).emplace_back(Array{});
					json_doc_.emplace_back(&std::get<Array>(json_doc_.back()->GetValueForChange()).back());
				}
			}
		}

		last_start.emplace_back("Array");
		return *this;
	}
	Builder &Builder::EndDict()
	{
		if (last_start.empty())
			BadTry();
		else if (last_start.back() != "Dict")
			BadTry();

		last_start.erase(last_start.end() - 1);
		if (!json_doc_.empty())
			json_doc_.erase(json_doc_.end() - 1);

		return *this;
	}
	Builder &Builder::EndArray()
	{
		if (last_start.empty())
			BadTry();
		else if (last_start.back() != "Array")
			BadTry();

		last_start.erase(last_start.end() - 1);
		if (!json_doc_.empty())
			json_doc_.erase(json_doc_.end() - 1);
		return *this;
	}
	Node Builder::Build()
	{
		if (!root_.has_value())
			BadTry();
		else if (!last_start.empty())
			BadTry();

		return root_.value();
	}

	KeyItemContext &DictItemContext::Key(std::string key)
	{
		return base_.value()->Key(key);
	}

	Builder &DictItemContext::EndDict()
	{
		return base_.value()->EndDict();
	}

	DictItemContext &KeyItemContext::Value(Node::Value value)
	{
		return {base_.value()->Value(value)};
	}

	DictItemContext &KeyItemContext::StartDict()
	{
		return base_.value()->StartDict();
	}

	ArrayItemContext &KeyItemContext::StartArray()
	{
		return base_.value()->StartArray();
	}

	ArrayItemContext &ArrayItemContext::Value(Node::Value value)
	{
		return base_.value()->Value(value);
	}
	DictItemContext &ArrayItemContext::StartDict()
	{
		std::cout << "test" << std::endl;
		return base_.value()->StartDict();
	}

	ArrayItemContext &ArrayItemContext::StartArray()
	{
		return base_.value()->StartArray();
	}

	Builder &ArrayItemContext::EndArray()
	{
		return base_.value()->EndArray();
	}
} // namespace json

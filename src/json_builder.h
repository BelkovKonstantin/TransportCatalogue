#pragma once
#include "json.h"
#include <sstream>
#include <fstream>
#include <optional>

namespace json
{
	/* Класс Builder, реализующий создание json документа посредством вызовов функций
	Классы {ArrayItemContext, DictItemContext, KeyItemContext} помогают контролировать правильность вызова функций на этапе компиляции*/
	class Builder;
	class ArrayItemContext;
	class KeyItemContext;

	class DictItemContext
	{
	protected:
		std::optional<Builder *> base_;

	public:
		DictItemContext() = default;
		DictItemContext(Builder &base) : base_(&base)
		{
		}
		virtual KeyItemContext &Key(std::string);
		virtual Builder &EndDict();

		virtual ~DictItemContext() = default;
	};

	class KeyItemContext
	{
	protected:
		std::optional<Builder *> base_;

	public:
		KeyItemContext() = default;
		KeyItemContext(Builder &base) : base_(&base)
		{
		}

		virtual DictItemContext &Value(Node::Value);
		virtual DictItemContext &StartDict();
		virtual ArrayItemContext &StartArray();

		virtual ~KeyItemContext() = default;
	};

	class ArrayItemContext
	{
	protected:
		std::optional<Builder *> base_;

	public:
		ArrayItemContext() = default;
		ArrayItemContext(Builder &base) : base_(&base)
		{
		}

		virtual ArrayItemContext &Value(Node::Value);
		virtual DictItemContext &StartDict();
		virtual ArrayItemContext &StartArray();
		virtual Builder &EndArray();

		virtual ~ArrayItemContext() = default;
	};

	class Builder : public DictItemContext, public KeyItemContext, public ArrayItemContext
	{
	private:
		std::optional<std::string> last_key_;
		std::vector<Node *> json_doc_;
		std::optional<Node> root_;
		std::vector<std::string> last_start;

		// Ошибка при невероятном случае обхода неправильной постройки на этапе компиляции
		void BadTry();

	public:
		Builder() = default;
		KeyItemContext &Key(std::string) override;
		Builder &Value(Node::Value);
		DictItemContext &StartDict() override;
		ArrayItemContext &StartArray() override;
		Builder &EndDict() override;
		Builder &EndArray() override;
		Node Build();

		~Builder() = default;
	};

} // namespace json

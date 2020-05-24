// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/class_rep.hpp>

namespace luabind
{
	struct value;

	struct value_vector : public luabind::vector<value>
	{
		virtual ~value_vector() = default;

		value_vector& operator,(const value& rhs)
		{
			push_back(rhs);
			return *this;
		}
	};

	struct value
	{
		friend class std::vector<value>;

		template <class T>
		value(const char* name, T v) :
			name_(name),
			val_(static_cast<int>(v))
		{
			assert(static_cast<T>(val_) == v);
		}

		const char* name_;
		int val_;

		value_vector operator,(const value& rhs) const
		{
			value_vector v;
			v.push_back(*this);
			v.push_back(rhs);
			return v;
		}

	private:
		value() = default;
	};
} // namespace luabind

namespace luabind::detail
{
	template <class From>
	struct enum_maker
	{
		explicit enum_maker(From& from) :
			from_(from)
		{}

		From& operator[](const value& val)
		{
			from_.add_static_constant(val.name_, val.val_);
			return from_;
		}

		From& operator[](const value_vector& values)
		{
			for (const auto& val : values)
				from_.add_static_constant(val.name_, val.val_);
			return from_;
		}

		From& from_;

	private:
		// C4512, assignment operator could not be generated
		void operator=(enum_maker const&);
		template <class T>
		void operator,(T const&) const;
	};
} // namespace luabind::detail

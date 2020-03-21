#ifndef STL2_TEST_CONCEPTS_NUMERIC_MODELS_HPP
#define STL2_TEST_CONCEPTS_NUMERIC_MODELS_HPP

#include <stl2/functional.hpp>
#include <functional>
#include <vector>

// Although there are only a handful of lines with "/* not desired */", this entire header is not
// desired, and should serve as good motivation for concept templates.

#define STL2_CONCEPT_TEMPLATE_FUNDAMENTAL_DEFINITION(STL2_CONCEPT)                     \
	template<class F, class Modifier = ranges::identity>                                \
	constexpr void fundamental_##STL2_CONCEPT(Modifier modifier = {}) noexcept {        \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, int, int>));                             \
		                                                                                 \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, int, unsigned int>));                    \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, unsigned int, int>));                    \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, unsigned int, unsigned int>));           \
		                                                                                 \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, int, long long>));                       \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, long long, int>));                       \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, long long, long long>));                 \
		                                                                                 \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, short, unsigned long long>));            \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, unsigned long long, short>));            \
		                                                                                 \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, int, double>));                          \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, double, int>));                          \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, double, float>));                        \
		CONCEPT_ASSERT(modifier(STL2_CONCEPT<F, float, double>));                        \
		                                                                                 \
		CONCEPT_ASSERT(not STL2_CONCEPT<F, int, std::complex<double>>);                  \
		CONCEPT_ASSERT(not STL2_CONCEPT<F, std::complex<double>, int>);                  \
		CONCEPT_ASSERT(not STL2_CONCEPT<F, std::complex<double>, std::complex<double>>); \
	}

#define CHECK_FUNDAMENTALS(STL2_CONCEPT, STL2_MODIFIER) {              \
	fundamental_##STL2_CONCEPT<int(int, int)>(STL2_MODIFIER);           \
	fundamental_##STL2_CONCEPT<int(int, unsigned int)>(STL2_MODIFIER);  \
	fundamental_##STL2_CONCEPT<int(unsigned int, int)>(STL2_MODIFIER);  \
	fundamental_##STL2_CONCEPT<int(int, short)>(STL2_MODIFIER);         \
	fundamental_##STL2_CONCEPT<int(short, int)>(STL2_MODIFIER);         \
	fundamental_##STL2_CONCEPT<int(int, double)>(STL2_MODIFIER);        \
	fundamental_##STL2_CONCEPT<int(double, int)>(STL2_MODIFIER);        \
	fundamental_##STL2_CONCEPT<int(double, double)>(STL2_MODIFIER);     \
	                                                                    \
	fundamental_##STL2_CONCEPT<double(double, double)>(STL2_MODIFIER);  \
	fundamental_##STL2_CONCEPT<double(double, float)>(STL2_MODIFIER);   \
	fundamental_##STL2_CONCEPT<double(float, double)>(STL2_MODIFIER);   \
	fundamental_##STL2_CONCEPT<double(float, float)>(STL2_MODIFIER);    \
	fundamental_##STL2_CONCEPT<double(int, double)>(STL2_MODIFIER);     \
	fundamental_##STL2_CONCEPT<double(double, int)>(STL2_MODIFIER);     \
	fundamental_##STL2_CONCEPT<double(int, short)>(STL2_MODIFIER);      \
	fundamental_##STL2_CONCEPT<double(short, int)>(STL2_MODIFIER);      \
	fundamental_##STL2_CONCEPT<double(int, int)>(STL2_MODIFIER);        \
	fundamental_##STL2_CONCEPT<void(int, int)>(std::logical_not{});     \
	fundamental_##STL2_CONCEPT<int()>(std::logical_not{});              \
	fundamental_##STL2_CONCEPT<int(int)>(std::logical_not{});           \
	fundamental_##STL2_CONCEPT<int(int, int, int)>(std::logical_not{}); \
}

struct base {};
struct derived : base {};

struct secretly_derived : protected base {};

#define CHECK_USER_TYPE(STL2_MODIFIER, STL2_CONCEPT) {                                                                                   \
	using std::vector;                                                                                                                    \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<vector<int>(vector<int>, vector<int>), vector<int>, vector<int>>);                          \
	CONCEPT_ASSERT(not STL2_CONCEPT<vector<int>(vector<int>, vector<int>), vector<int>, vector<float>>);                                  \
	CONCEPT_ASSERT(not STL2_CONCEPT<vector<int>(vector<int>, vector<int>), vector<float>, vector<int>>);                                  \
	CONCEPT_ASSERT(not STL2_CONCEPT<vector<int>(vector<int>, vector<int>), vector<float>, vector<float>>);                                \
	CONCEPT_ASSERT(not STL2_CONCEPT<vector<int>(vector<float>, vector<float>), vector<int>, vector<float>>);                              \
	CONCEPT_ASSERT(not STL2_CONCEPT<vector<int>(vector<float>, vector<float>), vector<float>, vector<int>>);                              \
	CONCEPT_ASSERT(not STL2_CONCEPT<vector<int>(vector<float>, vector<float>), vector<float>, vector<float>>);                            \
	                                                                                                                                      \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<base(base, base), base, base>);                                                             \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<base(base, base), base, derived>);                                                          \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<base(base, base), derived, base>);                                                          \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<base(base, base), derived, derived>);                                                       \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<derived(base, base), derived, derived>);                                                    \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<derived(derived, base), derived, derived>);                                                 \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<derived(base, derived), derived, derived>);                                                 \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<derived(derived, derived), derived, derived>);                                              \
	CONCEPT_ASSERT(not STL2_CONCEPT<derived(derived, derived), base, derived>);                                                           \
	CONCEPT_ASSERT(not STL2_CONCEPT<derived(derived, derived), derived, base>);                                                           \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<derived(derived, derived), derived, derived>);                                              \
	                                                                                                                                      \
	CONCEPT_ASSERT(not STL2_CONCEPT<base(base, base), secretly_derived, base>);                                                           \
	CONCEPT_ASSERT(not STL2_CONCEPT<base(base, base), base, secretly_derived>);                                                           \
	CONCEPT_ASSERT(not STL2_CONCEPT<base(base, base), secretly_derived, secretly_derived>);                                               \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(base, base), base, secretly_derived>);                                               \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(base, base), secretly_derived, base>);                                               \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(base, base), secretly_derived, secretly_derived>);                                   \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(base, secretly_derived), base, secretly_derived>);                                   \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(base, secretly_derived), secretly_derived, base>);                                   \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(base, secretly_derived), secretly_derived, secretly_derived>);                       \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(secretly_derived, base), base, secretly_derived>);                                   \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(secretly_derived, base), secretly_derived, base>);                                   \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(secretly_derived, base), secretly_derived, secretly_derived>);                       \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(secretly_derived, secretly_derived), base, secretly_derived>);                       \
	CONCEPT_ASSERT(not STL2_CONCEPT<secretly_derived(secretly_derived, secretly_derived), secretly_derived, base>);                       \
	CONCEPT_ASSERT(STL2_MODIFIER STL2_CONCEPT<secretly_derived(secretly_derived, secretly_derived), secretly_derived, secretly_derived>); \
}

#define STL2_CONCEPT_TEMPLATE_REFERENCE_DEFINITION(STL2_CONCEPT)       \
	template<class BOp, class T, class U,                               \
		ranges::Predicate<bool> monoid_modifier = ranges::identity,      \
		ranges::Predicate<bool> cv_modifier = std::logical_not<>>        \
	requires                                                            \
		not std::is_reference_v<T> and                                   \
		not std::is_reference_v<U> and                                   \
		not std::is_const_v<std::remove_reference_t<T>> and              \
		not std::is_const_v<std::remove_reference_t<U>>                  \
	constexpr void reference_##STL2_CONCEPT(monoid_modifier veto = {},  \
	                                        cv_modifier permit = {}) {  \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T&, T&>));                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, U&>));               \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&, T&>));               \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T, T&>));                \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, T>));                \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T, T>));                 \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U, T&>));                \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, U>));                \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U, U>));                 \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, T&&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&&, T&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&&, T&&>));             \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, U&&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&&, T&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&&, U&&>));             \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&, U&&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&&, U&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&&, U&&>));             \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&, U&&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&&, U&>));              \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U&&, U&&>));             \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T const&, T&>));         \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, T const&>));         \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T const&, T const&>));   \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U const&, T&>));         \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, U const&>));         \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U const&, U const&>));   \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T const&, U const&>));   \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U const&, T const&>));   \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T const&&, T&>));        \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, T const&&>));        \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T const&&, T const&&>)); \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U const&&, T&>));        \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T&, U const&&>));        \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U const&&, U const&&>)); \
		                                                                 \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, T const&&, U const&&>)); \
		CONCEPT_ASSERT(permit(STL2_CONCEPT<BOp, U const&&, T const&&>)); \
	}

#define CHECK_REFERENCE_TO_FUNDAMENTALS(STL2_CONCEPT, STL2_IS_MONOID) {                         \
	reference_##STL2_CONCEPT<int&(int&, int&), int, unsigned int>(STL2_IS_MONOID);               \
	reference_##STL2_CONCEPT<int&(int&, int&), int, char>(STL2_IS_MONOID);                       \
	reference_##STL2_CONCEPT<int&(int&, int&), int, double>(STL2_IS_MONOID);                     \
	reference_##STL2_CONCEPT<int(int&, int&), int, unsigned int>(STL2_IS_MONOID);                \
	reference_##STL2_CONCEPT<int(int&, int&), int, char>(STL2_IS_MONOID);                        \
	reference_##STL2_CONCEPT<int(int&, int&), int, double>(STL2_IS_MONOID);                      \
	reference_##STL2_CONCEPT<int&&(int&, int&), int, unsigned int>(STL2_IS_MONOID);              \
	reference_##STL2_CONCEPT<int&&(int&, int&), int, char>(STL2_IS_MONOID);                      \
	reference_##STL2_CONCEPT<int&&(int&, int&), int, double>(STL2_IS_MONOID);                    \
	reference_##STL2_CONCEPT<int const&(int&, int&), int, unsigned int>(STL2_IS_MONOID);         \
	reference_##STL2_CONCEPT<int const&(int&, int&), int, char>(STL2_IS_MONOID);                 \
	reference_##STL2_CONCEPT<int const&(int&, int&), int, double>(STL2_IS_MONOID);               \
	reference_##STL2_CONCEPT<int const&&(int&, int&), int, unsigned int>(STL2_IS_MONOID);        \
	reference_##STL2_CONCEPT<int const&&(int&, int&), int, char>(STL2_IS_MONOID);                \
	reference_##STL2_CONCEPT<int const&&(int&, int&), int, double>(STL2_IS_MONOID);              \
	                                                                                             \
	CONCEPT_ASSERT(not STL2_CONCEPT<int&(int&, int const&), int const&, int const&>);            \
	CONCEPT_ASSERT(not STL2_CONCEPT<int&(int&, int&), int const&, int const&>);                  \
	CONCEPT_ASSERT(not STL2_CONCEPT<int const&(int&, int&), int const&, int const&>);            \
	                                                                                             \
	reference_##STL2_CONCEPT<double&(int&, int&), int, unsigned int>(STL2_IS_MONOID);            \
	reference_##STL2_CONCEPT<double&(int&, int&), int, char>(STL2_IS_MONOID);                    \
	reference_##STL2_CONCEPT<double&(int&, int&), int, double>(STL2_IS_MONOID);                  \
	reference_##STL2_CONCEPT<double(int&, int&), int, unsigned int>(STL2_IS_MONOID);             \
	reference_##STL2_CONCEPT<double(int&, int&), int, char>(STL2_IS_MONOID);                     \
	reference_##STL2_CONCEPT<double(int&, int&), int, double>(STL2_IS_MONOID);                   \
	reference_##STL2_CONCEPT<double&&(int&, int&), int, unsigned int>(STL2_IS_MONOID);           \
	reference_##STL2_CONCEPT<double&&(int&, int&), int, char>(STL2_IS_MONOID);                   \
	reference_##STL2_CONCEPT<double&&(int&, int&), int, double>(STL2_IS_MONOID);                 \
	reference_##STL2_CONCEPT<double const&(int&, int&), int, unsigned int>(STL2_IS_MONOID);      \
	reference_##STL2_CONCEPT<double const&(int&, int&), int, char>(STL2_IS_MONOID);              \
	reference_##STL2_CONCEPT<double const&(int&, int&), int, double>(STL2_IS_MONOID);            \
	reference_##STL2_CONCEPT<double const&&(int&, int&), int, unsigned int>(STL2_IS_MONOID);     \
	reference_##STL2_CONCEPT<double const&&(int&, int&), int, char>(STL2_IS_MONOID);             \
	reference_##STL2_CONCEPT<double const&&(int&, int&), int, double>(STL2_IS_MONOID);           \
	                                                                                             \
	reference_##STL2_CONCEPT<int const&(int const&, int const&), int, unsigned int>(             \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<int const&(int const&, int const&), int, char>(                     \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<int const&(int const&, int const&), int, double>(                   \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<unsigned int const&(int const&, int const&), int, unsigned int>(    \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<char const&(int const&, int const&), int, char>(                    \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<double const&(int const&, int const&), int, double>(                \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
		                                                                                          \
	reference_##STL2_CONCEPT<int&(int&, double&), int, double>(std::logical_not<>{});            \
	reference_##STL2_CONCEPT<int const&(int&, double&), int, double>(std::logical_not<>{});      \
	reference_##STL2_CONCEPT<int const&(int const&, double const&), int, double>(                \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<int&(int&, double&), int, double>(std::logical_not<>{});            \
	reference_##STL2_CONCEPT<int const&(int&, double&), int, double>(std::logical_not<>{});      \
	reference_##STL2_CONCEPT<int const&(int const&, double const&), int, double>(                \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
		                                                                                          \
	/* the STL2_CONCEPT should be refined to prohibit the tests below */                         \
		                                                                                          \
	reference_##STL2_CONCEPT<int&(int const&, int const&), int, unsigned int>(                   \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<int&(int const&, int const&), int, char>(                           \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<int&(int const&, int const&), int, double>(                         \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
	reference_##STL2_CONCEPT<int&(int const&, double const&), int, double>(                      \
		STL2_IS_MONOID, STL2_IS_MONOID);                                                          \
}

#define CHECK_REFERENCE_TO_USER_TYPE(STL2_CONCEPT, STL2_IS_MONOID) {                                                             \
	using std::vector;                                                                                                            \
	reference_##STL2_CONCEPT<vector<int>&(vector<int>&, vector<int>&), vector<int>, vector<unsigned int>>(STL2_IS_MONOID);        \
	reference_##STL2_CONCEPT<vector<int>&(vector<int>&, vector<int>&), vector<int>, vector<char>>(STL2_IS_MONOID);                \
	reference_##STL2_CONCEPT<vector<int>&(vector<int>&, vector<int>&), vector<int>, vector<double>>(STL2_IS_MONOID);              \
	reference_##STL2_CONCEPT<vector<int>(vector<int>&, vector<int>&), vector<int>, vector<unsigned int>>(STL2_IS_MONOID);         \
	reference_##STL2_CONCEPT<vector<int>(vector<int>&, vector<int>&), vector<int>, vector<char>>(STL2_IS_MONOID);                 \
	reference_##STL2_CONCEPT<vector<int>(vector<int>&, vector<int>&), vector<int>, vector<double>>(STL2_IS_MONOID);               \
	reference_##STL2_CONCEPT<vector<int>&&(vector<int>&, vector<int>&), vector<int>, vector<unsigned int>>(STL2_IS_MONOID);       \
	reference_##STL2_CONCEPT<vector<int>&&(vector<int>&, vector<int>&), vector<int>, vector<char>>(STL2_IS_MONOID);               \
	reference_##STL2_CONCEPT<vector<int>&&(vector<int>&, vector<int>&), vector<int>, vector<double>>(STL2_IS_MONOID);             \
	reference_##STL2_CONCEPT<vector<int> const&(vector<int>&, vector<int>&), vector<int>, vector<unsigned int>>(STL2_IS_MONOID);  \
	reference_##STL2_CONCEPT<vector<int> const&(vector<int>&, vector<int>&), vector<int>, vector<char>>(STL2_IS_MONOID);          \
	reference_##STL2_CONCEPT<vector<int> const&(vector<int>&, vector<int>&), vector<int>, vector<double>>(STL2_IS_MONOID);        \
	reference_##STL2_CONCEPT<vector<int> const&&(vector<int>&, vector<int>&), vector<int>, vector<unsigned int>>(STL2_IS_MONOID); \
	reference_##STL2_CONCEPT<vector<int> const&&(vector<int>&, vector<int>&), vector<int>, vector<char>>(STL2_IS_MONOID);         \
	reference_##STL2_CONCEPT<vector<int> const&&(vector<int>&, vector<int>&), vector<int>, vector<double>>(STL2_IS_MONOID);       \
	                                                                                                                              \
	reference_##STL2_CONCEPT<base&(base&, base&), derived, secretly_derived>(STL2_IS_MONOID);                                     \
	reference_##STL2_CONCEPT<base(base&, base&), derived, secretly_derived>(STL2_IS_MONOID);                                      \
	reference_##STL2_CONCEPT<base&&(base&, base&), derived, secretly_derived>(STL2_IS_MONOID);                                    \
	reference_##STL2_CONCEPT<base const&(base&, base&), derived, secretly_derived>(STL2_IS_MONOID);                               \
	reference_##STL2_CONCEPT<base const&&(base&, base&), derived, secretly_derived>(STL2_IS_MONOID);                              \
}

#define STL2_CONCEPT_TEMPLATE_RVALUE_REFERENCE_DEFINITION(STL2_CONCEPT)       \
	template<class BOp, class T, class U,                                      \
		ranges::Predicate<bool> monoid_modifier = ranges::identity,             \
		ranges::Predicate<bool> cv_modifier = std::logical_not<>>               \
	requires                                                                   \
		not std::is_reference_v<T> and                                          \
		not std::is_reference_v<U> and                                          \
		not std::is_const_v<std::remove_reference_t<T>> and                     \
		not std::is_const_v<std::remove_reference_t<U>>                         \
	constexpr void rvalue_reference_##STL2_CONCEPT(monoid_modifier veto = {}) { \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T&&, T&&>));                      \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T&&, U&&>));                      \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&&, T&&>));                      \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&&, U&&>));                      \
		                                                                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T&&, T>));                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T, T&&>));                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T&&, U>));                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U, T&&>));                        \
		                                                                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&&, T>));                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T, U&&>));                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&&, U>));                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U, U&&>));                        \
		                                                                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, T&&, U&>));                       \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&, T&&>));                       \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&, U&>));                        \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&&, U&>));                       \
		CONCEPT_ASSERT(veto(STL2_CONCEPT<BOp, U&, U&&>));                       \
		                                                                        \
		CONCEPT_ASSERT(not STL2_CONCEPT<BOp, T&, T&>);                          \
		CONCEPT_ASSERT(not STL2_CONCEPT<BOp, T&&, T&>);                         \
		CONCEPT_ASSERT(not STL2_CONCEPT<BOp, T&, T&&>);                         \
		CONCEPT_ASSERT(not STL2_CONCEPT<BOp, T&, U&>);                          \
		CONCEPT_ASSERT(not STL2_CONCEPT<BOp, U&&, T&>);                         \
		CONCEPT_ASSERT(not STL2_CONCEPT<BOp, T&, U&&>);                         \
	}

#define CHECK_RVALUE_REFERENCE_TO_FUNDAMENTALS(STL2_CONCEPT, STL2_IS_MONOID) {                        \
	rvalue_reference_##STL2_CONCEPT<int&&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);           \
	rvalue_reference_##STL2_CONCEPT<int&&(int&&, int&&), int, char>(STL2_IS_MONOID);                   \
	rvalue_reference_##STL2_CONCEPT<int&&(int&&, int&&), int, double>(STL2_IS_MONOID);                 \
	rvalue_reference_##STL2_CONCEPT<int(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);             \
	rvalue_reference_##STL2_CONCEPT<int(int&&, int&&), int, char>(STL2_IS_MONOID);                     \
	rvalue_reference_##STL2_CONCEPT<int(int&&, int&&), int, double>(STL2_IS_MONOID);                   \
	rvalue_reference_##STL2_CONCEPT<int&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);            \
	rvalue_reference_##STL2_CONCEPT<int&(int&&, int&&), int, char>(STL2_IS_MONOID);                    \
	rvalue_reference_##STL2_CONCEPT<int&(int&&, int&&), int, double>(STL2_IS_MONOID);                  \
	rvalue_reference_##STL2_CONCEPT<int const&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);      \
	rvalue_reference_##STL2_CONCEPT<int const&(int&&, int&&), int, char>(STL2_IS_MONOID);              \
	rvalue_reference_##STL2_CONCEPT<int const&(int&&, int&&), int, double>(STL2_IS_MONOID);            \
	rvalue_reference_##STL2_CONCEPT<int const&&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);     \
	rvalue_reference_##STL2_CONCEPT<int const&&(int&&, int&&), int, char>(STL2_IS_MONOID);             \
	rvalue_reference_##STL2_CONCEPT<int const&&(int&&, int&&), int, double>(STL2_IS_MONOID);           \
	                                                                                                   \
	CONCEPT_ASSERT(not STL2_CONCEPT<int&(int&&, int const&&), int const&&, int const&&>);              \
	CONCEPT_ASSERT(not STL2_CONCEPT<int&(int&&, int&&), int const&&, int const&&>);                    \
	CONCEPT_ASSERT(not STL2_CONCEPT<int const&(int&&, int&&), int const&&, int const&&>);              \
	                                                                                                   \
	rvalue_reference_##STL2_CONCEPT<double&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);         \
	rvalue_reference_##STL2_CONCEPT<double&(int&&, int&&), int, char>(STL2_IS_MONOID);                 \
	rvalue_reference_##STL2_CONCEPT<double&(int&&, int&&), int, double>(STL2_IS_MONOID);               \
	rvalue_reference_##STL2_CONCEPT<double(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);          \
	rvalue_reference_##STL2_CONCEPT<double(int&&, int&&), int, char>(STL2_IS_MONOID);                  \
	rvalue_reference_##STL2_CONCEPT<double(int&&, int&&), int, double>(STL2_IS_MONOID);                \
	rvalue_reference_##STL2_CONCEPT<double&&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);        \
	rvalue_reference_##STL2_CONCEPT<double&&(int&&, int&&), int, char>(STL2_IS_MONOID);                \
	rvalue_reference_##STL2_CONCEPT<double&&(int&&, int&&), int, double>(STL2_IS_MONOID);              \
	rvalue_reference_##STL2_CONCEPT<double const&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);   \
	rvalue_reference_##STL2_CONCEPT<double const&(int&&, int&&), int, char>(STL2_IS_MONOID);           \
	rvalue_reference_##STL2_CONCEPT<double const&(int&&, int&&), int, double>(STL2_IS_MONOID);         \
	rvalue_reference_##STL2_CONCEPT<double const&&(int&&, int&&), int, unsigned int>(STL2_IS_MONOID);  \
	rvalue_reference_##STL2_CONCEPT<double const&&(int&&, int&&), int, char>(STL2_IS_MONOID);          \
	rvalue_reference_##STL2_CONCEPT<double const&&(int&&, int&&), int, double>(STL2_IS_MONOID);        \
	                                                                                                   \
	rvalue_reference_##STL2_CONCEPT<int const&(int const&&, int const&&), int, unsigned int>(          \
		STL2_IS_MONOID);                                                                                \
	rvalue_reference_##STL2_CONCEPT<int const&(int const&&, int const&&), int, char>(                  \
		STL2_IS_MONOID);                                                                                \
	rvalue_reference_##STL2_CONCEPT<int const&(int const&&, int const&&), int, double>(                \
		STL2_IS_MONOID);                                                                                \
	rvalue_reference_##STL2_CONCEPT<unsigned int const&(int const&&, int const&&), int, unsigned int>( \
		STL2_IS_MONOID);                                                                                \
	rvalue_reference_##STL2_CONCEPT<char const&(int const&&, int const&&), int, char>(                 \
		STL2_IS_MONOID);                                                                                \
	rvalue_reference_##STL2_CONCEPT<double const&(int const&&, int const&&), int, double>(             \
		STL2_IS_MONOID);                                                                                \
}

#endif // STL2_TEST_CONCEPTS_NUMERIC_MODELS_HPP

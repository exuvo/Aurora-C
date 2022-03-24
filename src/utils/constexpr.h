#ifndef SRC_UTILS_CONSTEXPR_H_
#define SRC_UTILS_CONSTEXPR_H_

// https://stackoverflow.com/questions/55288555/c-check-if-statement-can-be-evaluated-constexpr
//template<class CapturelessLambda, int=(CapturelessLambda{}(), 0)>
//constexpr bool is_constexpr(CapturelessLambda) { return true; }
//constexpr bool is_constexpr(...) { return false; }
//
//template<size_t size>
//void do_stuff() { (...) }
//void do_stuff(size_t size) { (...) }
//
//if constexpr(is_constexpr([]{ base::getData(); })) {
//		return do_stuff<base::getData()>();
//} else {
//		return do_stuff(base::getData());
//}


// if(std::is_constant_evaluated()) inside constexpr function to check if during compile- or run-time.


// https://stackoverflow.com/questions/40409323/compile-time-or-runtime-detection-within-a-constexpr-function/40413051#40413051
//uint64_t runtime(uint64_t a);
//
//constexpr uint64_t compiletime(uint64_t a) {
//	return 1;
//}
//
//struct StaticStruct {
//	static constexpr uint64_t aa_constexpr(uint64_t a) {
//		return compiletime(a);
//	}
//};
//
//template<int>
//using Void = void;
//
//template<typename F, typename A>
//constexpr auto is_a_constant_expression(F&& f, A&& a)
// -> decltype((std::forward<F>(f)(std::forward<A>(a)), std::true_type {})) {
//	return {};
//}
//constexpr std::false_type is_a_constant_expression(...) {
//	return {};
//}
//
//#define IS_A_CONSTANT_EXPRESSION(EXPR) is_a_constant_expression([](auto ty)-> Void<(decltype(ty)::EXPR, 0)>{}, StaticStruct{})
//
//#define aa(...) (IS_A_CONSTANT_EXPRESSION(aa_constexpr(__VA_ARGS__)) ? 
// StaticStruct::aa_constexpr(__VA_ARGS__) : runtime(__VA_ARGS__))


#endif /* SRC_UTILS_CONSTEXPR_H_ */

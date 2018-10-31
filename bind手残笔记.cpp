#include <iostream>
//#include <typeinfo>
//#include <functional>
#include <type_traits>
#include <tuple>
using namespace std;

template<size_t N>
struct placeholder{};

// static placeholder<1> _1; static placeholder<6>  _6;  static placeholder<11> _11; static placeholder<16> _16;
// static placeholder<2> _2; static placeholder<7>  _7;  static placeholder<12> _12; static placeholder<17> _17;
// static placeholder<3> _3; static placeholder<8>  _8;  static placeholder<13> _13; static placeholder<18> _18;
// static placeholder<4> _4; static placeholder<9>  _9;  static placeholder<14> _14; static placeholder<19> _19;
// static placeholder<5> _5; static placeholder<10> _10; static placeholder<15> _15; static placeholder<20> _20;

template <size_t... N>
struct seq { typedef seq<N..., sizeof...(N)> next_type; };
//生成序列！ 
// seq<0>::next_type == seq<0,1>
// seq<0>::next_type::next_type == seq<0,1,2>

template <typename... P>
struct gen;

template <>
struct gen<>
{
    typedef seq<> seq_type;
};

template <typename P1, typename... P>
struct gen<P1, P...>
{
    typedef typename gen<P...>::seq_type::next_type seq_type;// 返回0---输入个数-1的序列 
};

template <typename R, typename F, typename P1, typename... P>//对象成员函数 
inline typename std::enable_if<is_member_function_pointer<F>::value,R>::type invoke(F f, P1 this_obj, P... par){
    return (this_obj.*f)(par...);
}
template <typename R, typename F, typename... P>//普通函数 
inline typename std::enable_if<!is_member_function_pointer<F>::value,R>::type invoke(F f, P... par){
    return f(par...);
}

template<class F>
struct result_traits:result_traits<decltype(&F::operator())>{};
template<class R,class... Args>
struct result_traits<R(*)(Args...)>{
	typedef R type;
};	

// template<class Tp,size_t N>
// auto select(Tp tp)->decltype(get<N>(tp)){
//     return get<N>(tp);   
// }

template<class F,class... P>
class fn{
    typedef typename decay<F>::type call_type;
    typedef typename result_traits<call_type>::type result_type;
    typedef tuple<typename decay<P>::type...> args_type;//存储为tuple
    call_type _call;
    args_type _args;
    template</*class Tp,*/size_t... N>
     result_type _do_call(/*Tp& tp,*/seq<N...>){
        return invoke<result_type>(_call,get<N>(_args)...);
    }
    public:
        template<typename F1, typename... P1>//初始化 
        fn(F1 f, P1... p): _call(f), _args(p...){}
        /*template<class... Args>*/
        result_type operator()(/*Args... a*/){
            /*tuple<Args...> tp(a...);*/
            return _do_call(/*tp,*/typename gen<P...>::seq_type());
        }
};

template<class F,class... P>
fn<F,P...> my_bind(F f,P... p){//基础 
	return fn<F,P...>(forward<F>(f),forward<P>(p)...);
}

int test(double a){
    cout<<a<<endl;   
    return 100;
}

int main()
{
    auto a=my_bind(&test,1.2);
    cout<<a()<<endl;
}
